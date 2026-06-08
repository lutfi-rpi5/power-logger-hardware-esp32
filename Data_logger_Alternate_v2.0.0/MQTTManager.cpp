/******************************************************
 * File      : MQTTManager.cpp
 *
 * Non-blocking MQTT connect — 5-state machine dengan
 * DNS resolution di FreeRTOS background task.
 *
 * Root cause reboot (analisis lengkap):
 * ─────────────────────────────────────
 * client.connect(hostname, port) di IDF5 melakukan:
 *   1. DNS resolution (getaddrinfo) → BLOCKING di caller task
 *   2. TCP socket connect          → BLOCKING di caller task
 *
 * Tanpa internet, getaddrinfo() menunggu DNS timeout
 * dari semua server (~5–8 detik). Selama itu loop()
 * tidak berjalan → WDT tidak di-feed → reboot.
 *
 * Fix yang diterapkan:
 * ─────────────────────────────────────
 * 1. _initWatchdog() di .ino → reconfigure WDT timeout
 *    ke 30s (sebelumnya default IDF5 = 5s)
 *    → menghilangkan false-trigger untuk DNS blocking pendek
 *
 * 2. State DNS_RESOLVING → DNS resolution berjalan di
 *    FreeRTOS task terpisah (Core 0, prio rendah).
 *    Loop task tidak pernah block untuk DNS.
 *    Setelah IP didapat, TCP connect ke IPAddress langsung
 *    (bukan hostname) → tidak ada DNS lagi saat connect.
 *
 * Alur state machine:
 *   IDLE → DNS_RESOLVING → TCP_CONNECTING → MQTT_CONNECTING
 *                ↓               ↓                ↓
 *             COOLDOWN       COOLDOWN         COOLDOWN
 ******************************************************/

#include "MQTTManager.h"
#include "esp_task_wdt.h"

static const size_t JSON_BUF_SIZE = 1024;

// ── Static member definitions ─────────────────────────────
MQTTManager::DnsTaskParam MQTTManager::_dnsParam = {};

// ─── Constructor ──────────────────────────────────────────

MQTTManager::MQTTManager(StorageManager& storage)
    : _storage(storage)
    , _mqtt(_plainClient)
{}

// ─── Begin ────────────────────────────────────────────────

void MQTTManager::begin() {
    reloadConfig();
    Serial.printf("[MQTT] Manager ready. Broker: %s:%d  SSL:%d\n",
        _cfg.server, _cfg.port, _cfg.useSSL);
}

void MQTTManager::reloadConfig() {
    _cfg = _storage.getMQTTConfig();
    _setupClient();
    _activeClient()->stop();
    _connState         = ConnState::IDLE;
    _dnsResolved       = false;
    _dnsTaskRunning    = false;
    _tcpConnectStarted = false;
    updateState([](SystemState& s){ s.mqttConnected = false; });
}

// ─── Active client selector ───────────────────────────────

Client* MQTTManager::_activeClient() {
    return _cfg.useSSL
        ? static_cast<Client*>(&_secureClient)
        : static_cast<Client*>(&_plainClient);
}

// ─── TLS setup ────────────────────────────────────────────

bool MQTTManager::_setupClient() {
    if (_cfg.useSSL) {
        if (strlen(_cfg.caCert) > 10) {
            _secureClient.setCACert(_cfg.caCert);
        } else {
            _secureClient.setInsecure();
            Serial.println("[MQTT] WARNING: SSL tanpa CA cert (insecure mode)");
        }
        _mqtt.setClient(_secureClient);
    } else {
        _mqtt.setClient(_plainClient);
    }
    int port = _cfg.useSSL ? _cfg.sslPort : _cfg.port;
    _mqtt.setServer(_cfg.server, port);
    _mqtt.setBufferSize(1024);
    _mqtt.setKeepAlive(60);
    return true;
}

// ─── DNS Background Task ──────────────────────────────────
//
// Berjalan di Core 0, prioritas sangat rendah (0).
// Panggil WiFi.hostByName() yang blocking — tapi sekarang
// di task terpisah, bukan di loop task.
// Hasilnya ditulis ke _dnsParam.resolvedAddr.
// Task menghapus dirinya sendiri (vTaskDelete(NULL)) setelah selesai.

void MQTTManager::_dnsTask(void* param) {
    DnsTaskParam* p = static_cast<DnsTaskParam*>(param);

    IPAddress result;
    // ESP32 Arduino 3.3.0: hostByName(hostname, result) — 2 argumen saja.
    // Tidak ada parameter timeout di versi ini.
    // Timeout ditangani oleh DNS_TIMEOUT_MS di state machine (DNS_RESOLVING).
    bool ok = WiFi.hostByName(p->hostname, result);

    if (ok && (uint32_t)result != 0) {
        p->resolvedAddr = (uint32_t)result;
    } else {
        p->resolvedAddr = 0;  // gagal
    }
    p->done = true;

    vTaskDelete(NULL);  // task hapus diri sendiri
}

// ─── Main tick ────────────────────────────────────────────

void MQTTManager::tick() {
    // Feed WDT di awal tick — pastikan tidak miss feed
    esp_task_wdt_reset();

    // Tanpa WiFi, reset state machine
    if (!WiFi.isConnected()) {
        if (_connState != ConnState::IDLE) {
            _activeClient()->stop();
            // Jika DNS task masih jalan, biarkan selesai sendiri
            // (_dnsParam.done akan jadi true, task hapus diri)
            _connState      = ConnState::IDLE;
            _dnsResolved    = false;
            _dnsTaskRunning = false;
        }
        updateState([](SystemState& s){ s.mqttConnected = false; });
        return;
    }

    // Broker belum dikonfigurasi
    if (strlen(_cfg.server) == 0) return;

    if (_mqtt.connected()) {
        _connState = ConnState::CONNECTED;
        _mqtt.loop();
        unsigned long now = millis();
        if (now - _lastPublishMs >= MQTT_PUBLISH_INTERVAL_MS) {
            _lastPublishMs = now;
            _publishTelemetry(getStateCopy());
        }
        updateState([](SystemState& s){ s.mqttConnected = true; });
    } else {
        updateState([](SystemState& s){ s.mqttConnected = false; });
        _tickConnect();
    }
}

// ─── State Machine ────────────────────────────────────────

void MQTTManager::_tickConnect() {
    unsigned long now  = millis();
    int port = _cfg.useSSL ? _cfg.sslPort : _cfg.port;

    switch (_connState) {

    // ── IDLE: mulai siklus baru ───────────────────────────
    case ConnState::IDLE: {
        if (now - _lastReconnectMs < COOLDOWN_MS) return;
        _lastReconnectMs = now;

        // Bersihkan state sebelumnya
        _activeClient()->stop();
        _dnsResolved       = false;
        _dnsTaskRunning    = false;
        _tcpConnectStarted = false;

        // Cek apakah hostname sudah berupa IP address langsung
        // (user mungkin isi config dengan "1.2.3.4")
        IPAddress directIP;
        if (directIP.fromString(_cfg.server)) {
            // Sudah berupa IP, skip DNS
            _resolvedIP  = directIP;
            _dnsResolved = true;
            Serial.printf("[MQTT] Broker adalah IP langsung: %s — skip DNS.\n",
                          _cfg.server);
            _connState  = ConnState::TCP_CONNECTING;
            _tcpStartMs = now;
        } else {
            // Perlu DNS resolution — mulai background task
            Serial.printf("[MQTT] DNS resolving: %s ...\n", _cfg.server);
            memset(&_dnsParam, 0, sizeof(_dnsParam));
            strncpy(_dnsParam.hostname, _cfg.server, sizeof(_dnsParam.hostname) - 1);
            _dnsParam.done         = false;
            _dnsParam.resolvedAddr = 0;

            // Buat FreeRTOS task di Core 0, prio 0 (paling rendah)
            // Stack 3072: cukup untuk hostByName + WiFi stack calls
            BaseType_t created = xTaskCreatePinnedToCore(
                _dnsTask, "MQTTdns", 3072, &_dnsParam, 0, nullptr, 0
            );

            if (created == pdPASS) {
                _dnsTaskRunning = true;
                _connState      = ConnState::DNS_RESOLVING;
                _dnsStartMs     = now;
            } else {
                // Gagal buat task (heap habis?) → cooldown
                Serial.println("[MQTT] Gagal buat DNS task. Cooldown.");
                _connState       = ConnState::COOLDOWN;
                _cooldownStartMs = now;
            }
        }
        break;
    }

    // ── DNS_RESOLVING: tunggu background task selesai ─────
    case ConnState::DNS_RESOLVING:
        if (_dnsParam.done) {
            _dnsTaskRunning = false;
            if (_dnsParam.resolvedAddr != 0) {
                _resolvedIP = IPAddress(_dnsParam.resolvedAddr);
                _dnsResolved = true;
                Serial.printf("[MQTT] DNS OK: %s → %s\n",
                              _cfg.server, _resolvedIP.toString().c_str());
                _connState  = ConnState::TCP_CONNECTING;
                _tcpStartMs = now;
            } else {
                // DNS gagal: tidak ada internet, hostname tidak valid, dll.
                // TIDAK reboot. Cooldown, lalu retry.
                Serial.printf("[MQTT] DNS gagal: %s tidak dapat di-resolve. "
                              "Tidak ada internet? Cooldown %lus.\n",
                              _cfg.server, COOLDOWN_MS / 1000);
                _connState       = ConnState::COOLDOWN;
                _cooldownStartMs = now;
            }
        } else if (now - _dnsStartMs >= DNS_TIMEOUT_MS) {
            // Task jalan terlalu lama — sesuatu bermasalah
            // Task akan hapus dirinya sendiri, kita tidak bisa kill paksa
            // Cukup tunggu done=true di tick berikutnya atau reset paksa
            Serial.println("[MQTT] DNS timeout eksternal. Cooldown.");
            _dnsTaskRunning  = false;
            _connState       = ConnState::COOLDOWN;
            _cooldownStartMs = now;
        }
        // Selama menunggu: return. Loop tetap jalan, WDT tetap di-feed.
        break;

    // ── TCP_CONNECTING: connect ke IP (bukan hostname) ─────────────────────
    // Bug yang diperbaiki:
    //   - connect() TIDAK boleh dipanggil ulang setiap tick.
    //     WiFiClientSecure::connect() di ESP32 3.3.0 adalah SYNCHRONOUS:
    //     return 1 = berhasil, return 0 = gagal (bukan "in-progress").
    //     Memanggil ulang tiap tick → socket lama belum close, socket baru
    //     dibuat → semua gagal → TCP_TIMEOUT_MS tercapai tanpa pernah connect.
    //   - return 0 dari connect() = langsung gagal → COOLDOWN saat itu juga.
    //     Tidak perlu tunggu TCP_TIMEOUT_MS (hanya untuk kasus edge lain).
    case ConnState::TCP_CONNECTING:
        if (!_dnsResolved) {
            _connState = ConnState::IDLE;
            break;
        }

        if (!_tcpConnectStarted) {
            // Panggil connect() SEKALI SAJA saat pertama masuk state ini
            _tcpConnectStarted = true;
            _activeClient()->stop();   // pastikan socket sebelumnya bersih

            Serial.printf("[MQTT] TCP connect → %s:%d ...",
                          _resolvedIP.toString().c_str(), port);

            // WiFiClientSecure::connect(IPAddress, port) = SYNCHRONOUS di ESP32 3.3.0
            // Blok hingga TCP+SSL handshake selesai atau gagal.
            // Tidak ada DNS di sini (sudah resolved) → tidak akan blocking lama.
            // WDT 30s cukup untuk SSL handshake normal (biasanya < 3 detik).
            if (_activeClient()->connect(_resolvedIP, port)) {
                // Berhasil — langsung ke MQTT_CONNECTING
                Serial.printf("[MQTT] TCP+SSL connected ke %s:%d ✓",
                              _resolvedIP.toString().c_str(), port);
                _connState         = ConnState::MQTT_CONNECTING;
                _mqttStartMs       = now;
                _tcpConnectStarted = false;
            } else {
                // Gagal langsung — broker tidak menerima koneksi,
                // SSL cert mismatch, port salah, firewall, dll.
                Serial.printf("[MQTT] TCP connect gagal ke %s:%d (cek port/SSL config). Cooldown.",
                              _resolvedIP.toString().c_str(), port);
                _activeClient()->stop();
                _connState         = ConnState::COOLDOWN;
                _cooldownStartMs   = now;
                _tcpConnectStarted = false;
            }
        } else {
            // connect() sudah dipanggil tapi kita sampai sini lagi —
            // seharusnya tidak terjadi karena connect() synchronous.
            // Safety: timeout fallback.
            if (now - _tcpStartMs >= TCP_TIMEOUT_MS) {
                Serial.println("[MQTT] TCP_CONNECTING timeout (unexpected). Cooldown.");
                _activeClient()->stop();
                _connState         = ConnState::COOLDOWN;
                _cooldownStartMs   = now;
                _tcpConnectStarted = false;
            }
        }
        break;

    // ── MQTT_CONNECTING: TCP ready, kirim MQTT CONNECT ───
    case ConnState::MQTT_CONNECTING: {
        if (!_activeClient()->connected()) {
            // TCP drop sebelum MQTT selesai
            Serial.println("[MQTT] TCP drop saat MQTT handshake. Cooldown.");
            _connState       = ConnState::COOLDOWN;
            _cooldownStartMs = now;
            break;
        }

        String clientId = String(DEVICE_ID) + "-" + String(random(0xFFFF), HEX);
        bool ok = (strlen(_cfg.user) > 0)
            ? _mqtt.connect(clientId.c_str(), _cfg.user, _cfg.pass)
            : _mqtt.connect(clientId.c_str());

        if (ok) {
            Serial.println("[MQTT] Connected to broker ✓");
            updateState([](SystemState& s){ s.mqttConnected = true; });
            _connState = ConnState::CONNECTED;
        } else if (now - _mqttStartMs >= MQTT_TIMEOUT_MS) {
            Serial.printf("[MQTT] MQTT handshake timeout (state=%d). Cooldown.\n",
                          _mqtt.state());
            _activeClient()->stop();
            _connState       = ConnState::COOLDOWN;
            _cooldownStartMs = now;
        }
        break;
    }

    // ── CONNECTED: handle oleh tick(), seharusnya tidak sampai sini ──────
    case ConnState::CONNECTED:
        Serial.println("[MQTT] Koneksi terputus. Cooldown sebelum reconnect.");
        _activeClient()->stop();
        _connState       = ConnState::COOLDOWN;
        _cooldownStartMs = now;
        break;

    // ── COOLDOWN: tunggu sebelum retry ───────────────────
    case ConnState::COOLDOWN:
        if (now - _cooldownStartMs >= COOLDOWN_MS) {
            Serial.println("[MQTT] Cooldown selesai. Akan retry...");
            _connState       = ConnState::IDLE;
            _lastReconnectMs = 0;
        }
        break;
    }
}

bool MQTTManager::isConnected() {
    return _mqtt.connected();
}

// ─── Publish ──────────────────────────────────────────────

void MQTTManager::_publishTelemetry(const SystemState& state) {
    static char buf[JSON_BUF_SIZE];
    _buildJson(state, buf, sizeof(buf));
    char topic[128];
    snprintf(topic, sizeof(topic), "%stellemetry", _cfg.topic);
    bool ok = _mqtt.publish(topic, buf, false);
    if (ok) {
        Serial.printf("[MQTT] Published %d bytes → %s\n", (int)strlen(buf), topic);
    } else {
        Serial.println("[MQTT] Publish failed");
    }
}

// ─── JSON Builder ─────────────────────────────────────────

void MQTTManager::_buildJson(const SystemState& state, char* buf, size_t len) {
    StaticJsonDocument<1024> doc;
    JsonObject dev = doc.createNestedObject("device");
    dev["id"]     = DEVICE_ID;
    dev["fw"]     = FW_VERSION;
    dev["uptime"] = state.uptimeSeconds;
    dev["heap"]   = state.freeHeapBytes;
    dev["rssi"]   = state.wifiRSSI;

    JsonObject phases = doc.createNestedObject("phases");
    const char* labels[3] = {"R", "S", "T"};
    for (int i = 0; i < 3; i++) {
        JsonObject ph = phases.createNestedObject(labels[i]);
        _addPhase(ph, state.phases[i]);
    }
    doc["unbalance"] = serialized(String(state.unbalance, 2));
    doc["ts"]        = millis();
    serializeJson(doc, buf, len);
}

void MQTTManager::_addPhase(JsonObject& obj, const PhaseReading& r) {
    if (!r.valid) { obj["status"] = "LOST"; return; }
    obj["v"]      = serialized(String(r.voltage,       1));
    obj["i"]      = serialized(String(r.current,       2));
    obj["p"]      = serialized(String(r.activePower,   1));
    obj["s"]      = serialized(String(r.apparentPower, 1));
    obj["q"]      = serialized(String(r.reactivePower, 1));
    obj["pf"]     = serialized(String(r.powerFactor,   2));
    obj["f"]      = serialized(String(r.frequency,     1));
    obj["e"]      = serialized(String(r.energyWh,      1));
    obj["status"] = lineStatusStr(r.status);
}
