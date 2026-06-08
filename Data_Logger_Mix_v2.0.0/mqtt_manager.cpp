#include "mqtt_manager.h"
#include "diagnostics.h"
#include "esp_task_wdt.h"

static const size_t JSON_BUF_SIZE = 1024;

MQTTManager::DnsTaskParam MQTTManager::_dnsParam = {};

MQTTManager::MQTTManager(StorageManager& storage)
    : _storage(storage), _mqtt(_plainClient)
{}

void MQTTManager::begin() {
    reloadConfig();
    diag.info("MQTT", "Manager ready. Broker: %s  port:%d  SSL:%d",
        _cfg.server, _resolvePort(), _cfg.useSSL);
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

int MQTTManager::_resolvePort() const {
    return _cfg.useSSL ? _cfg.sslPort : _cfg.port;
}

Client* MQTTManager::_activeClient() {
    return _cfg.useSSL
        ? static_cast<Client*>(&_secureClient)
        : static_cast<Client*>(&_plainClient);
}

bool MQTTManager::_setupClient() {
    if (_cfg.useSSL) {
        if (strlen(_cfg.caCert) > 10) {
            _secureClient.setCACert(_cfg.caCert);
        } else {
            _secureClient.setInsecure();
            diag.warn("MQTT", "SSL tanpa CA cert (insecure mode)");
        }
        _mqtt.setClient(_secureClient);
    } else {
        _mqtt.setClient(_plainClient);
    }
    int port = _resolvePort();
    _mqtt.setServer(_cfg.server, port);
    _mqtt.setBufferSize(1024);
    _mqtt.setKeepAlive(60);
    return true;
}

void MQTTManager::_dnsTask(void* param) {
    DnsTaskParam* p = static_cast<DnsTaskParam*>(param);
    IPAddress result;
    bool ok = WiFi.hostByName(p->hostname, result);
    if (ok && (uint32_t)result != 0) {
        p->resolvedAddr = (uint32_t)result;
    } else {
        p->resolvedAddr = 0;
    }
    p->done = true;
    vTaskDelete(NULL);
}

void MQTTManager::tick() {
    esp_task_wdt_reset();

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

    if (strlen(_cfg.server) == 0) return;

    if (_mqtt.connected()) {
        _connState = ConnState::CONNECTED;
        _mqtt.loop();
        unsigned long now = millis();
        if (now - _lastPublishMs >= MQTT_PUBLISH_INTERVAL_MS) {
            _lastPublishMs = now;
            _publishSeq++;
            _publishTelemetry(getStateCopy());
        }
        updateState([](SystemState& s){ s.mqttConnected = true; });
    } else {
        updateState([](SystemState& s){ s.mqttConnected = false; });
        _tickConnect();
    }
}

void MQTTManager::_tickConnect() {
    unsigned long now  = millis();
    int port = _resolvePort();

    switch (_connState) {

    case ConnState::IDLE: {
        if (now - _lastReconnectMs < COOLDOWN_MS) return;
        _lastReconnectMs = now;

        _activeClient()->stop();
        _dnsResolved       = false;
        _dnsTaskRunning    = false;
        _tcpConnectStarted = false;

        IPAddress directIP;
        if (directIP.fromString(_cfg.server)) {
            _resolvedIP  = directIP;
            _dnsResolved = true;
            diag.info("MQTT", "Broker adalah IP langsung: %s — skip DNS.", _cfg.server);
            _connState  = ConnState::TCP_CONNECTING;
            _tcpStartMs = now;
        } else {
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

    case ConnState::MQTT_CONNECTING: {
        if (!_activeClient()->connected()) {
            diag.warn("MQTT", "TCP drop saat MQTT handshake. Cooldown.");
            _connState       = ConnState::COOLDOWN;
            _cooldownStartMs = now;
            break;
        }

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

    case ConnState::CONNECTED:
        diag.warn("MQTT", "Koneksi terputus. Cooldown.");
        _activeClient()->stop();
        _connState       = ConnState::COOLDOWN;
        _cooldownStartMs = now;
        break;

    case ConnState::COOLDOWN:
        if (now - _cooldownStartMs >= COOLDOWN_MS) {
            diag.info("MQTT", "Cooldown selesai. Akan retry...");
            _connState       = ConnState::IDLE;
            _lastReconnectMs = 0;
        }
        break;
    }
}

void MQTTManager::_publishTelemetry(const SystemState& state) {
    JSONBuilder builder;
    String json = builder.build(state, _publishSeq, millis());

    char topic[128];
    snprintf(topic, sizeof(topic), "%s/%s/%s", _cfg.prefix, _cfg.topic, DEVICE_ID);
    bool ok = _mqtt.publish(topic, json.c_str(), false);
    if (ok) {
        diag.info("MQTT", "Published %d bytes → %s", json.length(), topic);
    } else {
        diag.warn("MQTT", "Publish failed");
    }
}

bool MQTTManager::isConnected() {
    return _mqtt.connected();
}
