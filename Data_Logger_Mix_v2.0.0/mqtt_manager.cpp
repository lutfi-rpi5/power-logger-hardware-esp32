#include "mqtt_manager.h"
#include "diagnostics.h"
#include "esp_task_wdt.h"

/**
 * @file mqtt_manager.cpp
 * @brief Non-blocking MQTT client with 5-state connection machine.
 *
 * Root cause addressed: PubSubClient's `connect(hostname, port)` performs
 * synchronous DNS resolution that blocks for 5–8 s on some networks,
 * triggering the ESP32 Task Watchdog Timer (default 5 s).
 *
 * Solution:
 * 1. DNS resolution runs in a dedicated FreeRTOS task on Core 0 at
 *    priority 0 — the main loop is never blocked.
 * 2. TCP connects directly to the resolved IPAddress (no hostname call).
 * 3. WDT timeout increased to 30 s (config.h) as a safety net.
 *
 * State machine:
 * ```
 * IDLE ──(cooldown expired)──→ DNS_RESOLVING ──(DNS done)──→ TCP_CONNECTING
 *                                                               │
 *                                                          (TCP ok)
 *                                                               ▼
 *                                        MQTT_CONNECTING ──→ CONNECTED
 *                                            │                    │
 *                                            └── (fail) ──────────┘
 *                                                     │
 *                                                     ▼
 *                                                  COOLDOWN ──→ IDLE
 * ```
 */

/// Static DNS task parameter block (singleton, one resolution at a time).
MQTTManager::DnsTaskParam MQTTManager::_dnsParam = {};

MQTTManager::MQTTManager(StorageManager& storage)
    : _storage(storage), _mqtt(_plainClient)
{}

/**
 * @brief Initialise the MQTT manager.
 *
 * Loads config from NVS and configures the WiFiClient (SSL cert if
 * applicable). Does NOT start a connection — the first connect cycle
 * begins on the next tick().
 */
void MQTTManager::begin() {
    reloadConfig();
    diag.info("MQTT", "Manager ready. Broker: %s  port:%d  SSL:%d",
        _cfg.server, _resolvePort(), _cfg.useSSL);
}

/**
 * @brief Reload MQTT configuration from NVS.
 *
 * Stops any active connection, resets the state machine to IDLE,
 * and loads fresh config from NVS. Called by the webserver callback
 * when the user saves new MQTT settings.
 */
void MQTTManager::reloadConfig() {
    _cfg = _storage.getMQTTConfig();
    _setupClient();
    _activeClient()->stop();     // Drop any existing connection
    _connState         = ConnState::IDLE;
    _dnsResolved       = false;
    _dnsTaskRunning    = false;
    _tcpConnectStarted = false;
    updateState([](SystemState& s){ s.mqttConnected = false; });
}

/**
 * @brief Resolve the effective port based on SSL/WS config.
 * @return Port number for the current connection mode.
 */
int MQTTManager::_resolvePort() const {
    return _cfg.useSSL ? _cfg.sslPort : _cfg.port;
}

/**
 * @brief Get the active WiFiClient based on useSSL.
 * @return Pointer to either _plainClient or _secureClient.
 */
Client* MQTTManager::_activeClient() {
    return _cfg.useSSL
        ? static_cast<Client*>(&_secureClient)
        : static_cast<Client*>(&_plainClient);
}

/**
 * @brief Configure the WiFiClient and attach it to PubSubClient.
 *
 * For SSL connections: if a CA certificate is stored (>10 chars),
 * use it for server verification; otherwise fall back to insecure
 * mode (no certificate validation).
 *
 * @return true on success.
 */
bool MQTTManager::_setupClient() {
    if (_cfg.useSSL) {
        if (strlen(_cfg.caCert) > 10) {
            _secureClient.setCACert(_cfg.caCert);
        } else {
            _secureClient.setInsecure();  // No cert validation
            diag.warn("MQTT", "SSL tanpa CA cert (insecure mode)");
        }
        _mqtt.setClient(_secureClient);
    } else {
        _mqtt.setClient(_plainClient);
    }
    int port = _resolvePort();
    _mqtt.setServer(_cfg.server, port);
    _mqtt.setBufferSize(1024);    // Sufficient for JSON payloads up to ~1 KB
    _mqtt.setKeepAlive(60);       // 60 s keepalive interval
    return true;
}

/**
 * @brief FreeRTOS task for background DNS resolution.
 *
 * Runs on Core 0 at priority 0 (lowest). Resolves the broker hostname
 * and stores the result in the shared DnsTaskParam struct. Self-deletes
 * on completion.
 *
 * @param param Pointer to DnsTaskParam.
 */
void MQTTManager::_dnsTask(void* param) {
    DnsTaskParam* p = static_cast<DnsTaskParam*>(param);
    IPAddress result;
    bool ok = WiFi.hostByName(p->hostname, result);
    if (ok && (uint32_t)result != 0) {
        p->resolvedAddr = (uint32_t)result;
    } else {
        p->resolvedAddr = 0;  // Resolution failed
    }
    p->done = true;
    vTaskDelete(NULL);  // Self-delete (task function must not return)
}

/**
 * @brief Periodic handler — call from main loop every iteration.
 *
 * Handles:
 * 1. Connection lifecycle (via _tickConnect() if not connected).
 * 2. MQTT keepalive loop (mqtt.loop()) when connected.
 * 3. Telemetry publishing at MQTT_PUBLISH_INTERVAL_MS intervals.
 *
 * When WiFi is disconnected, drops to IDLE immediately to avoid
 * stale connection attempts.
 */
void MQTTManager::tick() {
    esp_task_wdt_reset();  // Feed WDT — MQTT operations may be long

    // ── Without WiFi, MQTT cannot connect ────────────────────────────
    if (!WiFi.isConnected()) {
        if (_connState != ConnState::IDLE) {
            _activeClient()->stop();
            _connState      = ConnState::IDLE;
            _dnsResolved    = false;
            _dnsTaskRunning = false;
        }
        updateState([](SystemState& s){ s.mqttConnected = false; });
        return;
    }

    // No broker configured — nothing to do
    if (strlen(_cfg.server) == 0) return;

    // ── Connected: keepalive + publish ───────────────────────────────
    if (_mqtt.connected()) {
        _connState = ConnState::CONNECTED;
        _mqtt.loop();  // Process MQTT keepalive and incoming

        unsigned long now = millis();
        if (now - _lastPublishMs >= MQTT_PUBLISH_INTERVAL_MS) {
            _lastPublishMs = now;
            _publishSeq++;
            _publishTelemetry(getStateCopy());
        }
        updateState([](SystemState& s){ s.mqttConnected = true; });
    } else {
        updateState([](SystemState& s){ s.mqttConnected = false; });
        _tickConnect();  // Advance the connection state machine
    }
}

/**
 * @brief MQTT connection state machine.
 *
 * Transitions between states based on timing and external events
 * (DNS completion, TCP connect result, MQTT CONNACK).
 *
 * All states are non-blocking — each call executes one small step
 * and returns. The state machine may take several tick() iterations
 * to complete a full connection cycle.
 */
void MQTTManager::_tickConnect() {
    unsigned long now  = millis();
    int port = _resolvePort();

    switch (_connState) {

    // ─── STATE: IDLE ──────────────────────────────────────────────────
    // Wait for cooldown, then start a new cycle.
    case ConnState::IDLE: {
        if (now - _lastReconnectMs < COOLDOWN_MS) return;
        _lastReconnectMs = now;

        // Reset state for fresh connection cycle
        _activeClient()->stop();
        _dnsResolved       = false;
        _dnsTaskRunning    = false;
        _tcpConnectStarted = false;

        // Check if broker is an IP literal (skip DNS)
        IPAddress directIP;
        if (directIP.fromString(_cfg.server)) {
            _resolvedIP  = directIP;
            _dnsResolved = true;
            diag.info("MQTT", "Broker adalah IP langsung: %s — skip DNS.", _cfg.server);
            _connState  = ConnState::TCP_CONNECTING;
            _tcpStartMs = now;
        } else {
            // Start DNS resolution in a separate task
            diag.info("MQTT", "DNS resolving: %s ...", _cfg.server);
            memset(&_dnsParam, 0, sizeof(_dnsParam));
            strncpy(_dnsParam.hostname, _cfg.server, sizeof(_dnsParam.hostname) - 1);
            _dnsParam.done         = false;
            _dnsParam.resolvedAddr = 0;

            BaseType_t created = xTaskCreatePinnedToCore(
                _dnsTask, "MQTTdns", 3072, &_dnsParam, 0, nullptr, 0
            );

            if (created == pdPASS) {
                _dnsTaskRunning = true;
                _connState      = ConnState::DNS_RESOLVING;
                _dnsStartMs     = now;
            } else {
                diag.warn("MQTT", "Gagal buat DNS task. Cooldown.");
                _connState       = ConnState::COOLDOWN;
                _cooldownStartMs = now;
            }
        }
        break;
    }

    // ─── STATE: DNS_RESOLVING ────────────────────────────────────────
    // Wait for the background DNS task to complete or time out.
    case ConnState::DNS_RESOLVING:
        if (_dnsParam.done) {
            _dnsTaskRunning = false;
            if (_dnsParam.resolvedAddr != 0) {
                _resolvedIP = IPAddress(_dnsParam.resolvedAddr);
                _dnsResolved = true;
                diag.info("MQTT", "DNS OK: %s → %s", _cfg.server, _resolvedIP.toString().c_str());
                _connState  = ConnState::TCP_CONNECTING;
                _tcpStartMs = now;
            } else {
                diag.warn("MQTT", "DNS gagal: %s tidak dapat di-resolve. Cooldown %lus.",
                    _cfg.server, COOLDOWN_MS / 1000);
                _connState       = ConnState::COOLDOWN;
                _cooldownStartMs = now;
            }
        } else if (now - _dnsStartMs >= DNS_TIMEOUT_MS) {
            diag.warn("MQTT", "DNS timeout eksternal. Cooldown.");
            _dnsTaskRunning  = false;
            _connState       = ConnState::COOLDOWN;
            _cooldownStartMs = now;
        }
        break;

    // ─── STATE: TCP_CONNECTING ───────────────────────────────────────
    // Connect directly to the resolved IP (no hostname lookup).
    // This is a synchronous call but is quick (typically < 1 s).
    case ConnState::TCP_CONNECTING:
        if (!_dnsResolved) { _connState = ConnState::IDLE; break; }

        if (!_tcpConnectStarted) {
            _tcpConnectStarted = true;
            _activeClient()->stop();
            diag.info("MQTT", "TCP connect → %s:%d ...",
                _resolvedIP.toString().c_str(), port);

            if (_activeClient()->connect(_resolvedIP, port)) {
                diag.info("MQTT", "TCP+SSL connected ke %s:%d ✓",
                    _resolvedIP.toString().c_str(), port);
                _connState         = ConnState::MQTT_CONNECTING;
                _mqttStartMs       = now;
                _tcpConnectStarted = false;
            } else {
                int lastErr = _cfg.useSSL ? _secureClient.lastError(nullptr, 0) : 0;
                diag.warn("MQTT", "TCP connect gagal ke %s:%d (SSL=%d err=%d). Cooldown.",
                    _resolvedIP.toString().c_str(), port, _cfg.useSSL, lastErr);
                _activeClient()->stop();
                _connState         = ConnState::COOLDOWN;
                _cooldownStartMs   = now;
                _tcpConnectStarted = false;
            }
        } else {
            if (now - _tcpStartMs >= TCP_TIMEOUT_MS) {
                diag.warn("MQTT", "TCP_CONNECTING timeout. Cooldown.");
                _activeClient()->stop();
                _connState         = ConnState::COOLDOWN;
                _cooldownStartMs   = now;
                _tcpConnectStarted = false;
            }
        }
        break;

    // ─── STATE: MQTT_CONNECTING ─────────────────────────────────────
    // Send MQTT CONNECT packet and wait for CONNACK.
    case ConnState::MQTT_CONNECTING: {
        if (!_activeClient()->connected()) {
            diag.warn("MQTT", "TCP drop saat MQTT handshake. Cooldown.");
            _connState       = ConnState::COOLDOWN;
            _cooldownStartMs = now;
            break;
        }

        // Generate unique client ID: device_id + random hex suffix
        String clientId = String(DEVICE_ID) + "-" + String(random(0xFFFF), HEX);
        bool ok = (strlen(_cfg.user) > 0)
            ? _mqtt.connect(clientId.c_str(), _cfg.user, _cfg.pass)
            : _mqtt.connect(clientId.c_str());

        if (ok) {
            diag.info("MQTT", "Connected to broker ✓");
            updateState([](SystemState& s){ s.mqttConnected = true; });
            _connState = ConnState::CONNECTED;
        } else {
            int st = _mqtt.state();
            if (now - _mqttStartMs >= MQTT_TIMEOUT_MS) {
                diag.warn("MQTT", "MQTT handshake timeout (state=%d). Cooldown.", st);
                _activeClient()->stop();
                _connState       = ConnState::COOLDOWN;
                _cooldownStartMs = now;
            } else if (st < 0) {
                diag.warn("MQTT", "MQTT connect failed (state=%d). Cooldown.", st);
                _activeClient()->stop();
                _connState       = ConnState::COOLDOWN;
                _cooldownStartMs = now;
            }
        }
        break;
    }

    // ─── STATE: CONNECTED ───────────────────────────────────────────
    // Unexpected drop while in CONNECTED state → cooldown.
    case ConnState::CONNECTED:
        diag.warn("MQTT", "Koneksi terputus. Cooldown.");
        _activeClient()->stop();
        _connState       = ConnState::COOLDOWN;
        _cooldownStartMs = now;
        break;

    // ─── STATE: COOLDOWN ─────────────────────────────────────────────
    // Wait for cooldown, then return to IDLE to retry.
    case ConnState::COOLDOWN:
        if (now - _cooldownStartMs >= COOLDOWN_MS) {
            diag.info("MQTT", "Cooldown selesai. Akan retry...");
            _connState       = ConnState::IDLE;
            _lastReconnectMs = 0;
        }
        break;
    }
}

/**
 * @brief Build and publish a telemetry JSON payload.
 *
 * Topic format: `{prefix}/{topic}/{DEVICE_ID}`
 * Example: "lutpiii/telemetry/3ph-logger-001"
 *
 * The payload is built by JSONBuilder and published as a non-retained
 * message. QoS 0 (at most once) — fire-and-forget.
 *
 * @param state Current SystemState snapshot.
 */
void MQTTManager::_publishTelemetry(const SystemState& state) {
    JSONBuilder builder;
    String json = builder.build(state, _publishSeq, millis());

    char topic[128];
    snprintf(topic, sizeof(topic), "%s/%s/%s", _cfg.prefix, _cfg.topic, DEVICE_ID);
    bool ok = _mqtt.publish(topic, json.c_str(), false);  // non-retained
    if (ok) {
        diag.info("MQTT", "Published %d bytes → %s", json.length(), topic);
    } else {
        diag.warn("MQTT", "Publish failed");
    }
}

/**
 * @brief Check MQTT connection status.
 * @return true if PubSubClient::connected().
 */
bool MQTTManager::isConnected() {
    return _mqtt.connected();
}
