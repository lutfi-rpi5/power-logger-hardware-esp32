#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <Arduino.h>
#include <AsyncMqttClient.h>
#include "types.h"

// ============================================================
// Async MQTT manager — non-blocking pub/sub
// ============================================================
// Connects in background. Does not block boot. Self-heals
// with automatic reconnect.
// ============================================================

class MQTTManager {
public:
    MQTTManager();

    void begin();
    void setConfig(const MQTTConfig& cfg);
    void update();  // call every loop

    bool beginConnect();
    void disconnect();
    bool isConnected() const;

    bool publish(const char* payload);

private:
    AsyncMqttClient _client;
    MQTTConfig _cfg;
    bool _configured;
    unsigned long _lastAttemptMs;
    bool _connecting;

    void _onConnect(bool sessionPresent);
    void _onDisconnect(AsyncMqttClientDisconnectReason reason);
    String _buildTopic() const;
};

#endif
