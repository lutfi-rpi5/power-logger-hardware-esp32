#include "mqtt_manager.h"
#include "config.h"
#include "diagnostics.h"
#include <WiFi.h>

MQTTManager::MQTTManager()
    : _configured(false)
    , _lastAttemptMs(0)
    , _connecting(false)
{}

void MQTTManager::begin() {
    _client.onConnect([this](bool sessionPresent) { _onConnect(sessionPresent); });
    _client.onDisconnect([this](AsyncMqttClientDisconnectReason reason) { _onDisconnect(reason); });
    diag.info("MQTT", "MQTT manager initialized");
}

void MQTTManager::setConfig(const MQTTConfig& cfg) {
    _cfg = cfg;
    _configured = true;
}

void MQTTManager::update() {
    if (!_configured) return;

    // Auto-reconnect
    if (!_client.connected() && !_connecting && WiFi.isConnected()) {
        unsigned long now = millis();
        if (now - _lastAttemptMs >= MQTT_RECONNECT_INTERVAL_MS) {
            beginConnect();
        }
    }
}

bool MQTTManager::beginConnect() {
    if (!_configured || _connecting) return false;

    _connecting = true;
    _lastAttemptMs = millis();

    _client.setServer(_cfg.server, (_cfg.useSSL ? _cfg.sslPort : _cfg.port));
    _client.setCredentials(_cfg.user, _cfg.pass);

    // Generate unique client ID
    char clientId[32];
    snprintf(clientId, sizeof(clientId), "%s-%06x", DEVICE_ID, (unsigned int)ESP.getEfuseMac());

    _client.setClientId(clientId);

    if (_cfg.useSSL && strlen(_cfg.caCert) > 0) {
        // Note: AsyncMqttClient + WiFiClientSecure needs special handling
        diag.info("MQTT", "Connecting with SSL to %s:%d", _cfg.server, _cfg.sslPort);
    } else {
        diag.info("MQTT", "Connecting to %s:%d", _cfg.server, _cfg.port);
    }

    _client.connect();
    return true;
}

void MQTTManager::disconnect() {
    _client.disconnect();
    _connecting = false;
}

bool MQTTManager::isConnected() const {
    return _client.connected();
}

String MQTTManager::_buildTopic() const {
    String topic = _cfg.prefix;
    if (topic.length() > 0 && topic[topic.length() - 1] != '/') topic += "/";
    topic += _cfg.topic;
    topic += "/";
    topic += DEVICE_ID;
    return topic;
}

bool MQTTManager::publish(const char* payload) {
    if (!_client.connected()) {
        diag.warn("MQTT", "Cannot publish, not connected");
        return false;
    }

    String topic = _buildTopic();
    uint16_t packetId = _client.publish(topic.c_str(), 0, false, payload);
    if (packetId > 0) {
        diag.debug("MQTT", "Published to %s (%d bytes, PID=%u)", topic.c_str(), strlen(payload), packetId);
        return true;
    }
    diag.warn("MQTT", "Publish failed to %s", topic.c_str());
    return false;
}

void MQTTManager::_onConnect(bool sessionPresent) {
    _connecting = false;
    diag.info("MQTT", "Connected to broker (sessionPresent=%d)", sessionPresent);
}

void MQTTManager::_onDisconnect(AsyncMqttClientDisconnectReason reason) {
    _connecting = false;
    diag.warn("MQTT", "Disconnected, reason=%d", (int8_t)reason);
}
