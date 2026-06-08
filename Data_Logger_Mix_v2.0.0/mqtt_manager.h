#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "config.h"
#include "types.h"
#include "storage_manager.h"
#include "json_builder.h"

class MQTTManager {
public:
    explicit MQTTManager(StorageManager& storage);
    void begin();
    void tick();
    bool isConnected();
    void reloadConfig();

private:
    StorageManager&   _storage;
    MQTTConfig        _cfg;

    WiFiClient        _plainClient;
    WiFiClientSecure  _secureClient;
    PubSubClient      _mqtt;

    unsigned long _lastReconnectMs  = 0;
    unsigned long _lastPublishMs    = 0;
    uint32_t      _publishSeq       = 0;

    enum class ConnState : uint8_t {
        IDLE, DNS_RESOLVING, TCP_CONNECTING, MQTT_CONNECTING, CONNECTED, COOLDOWN
    };
    ConnState     _connState       = ConnState::IDLE;
    unsigned long _dnsStartMs      = 0;
    unsigned long _tcpStartMs      = 0;
    unsigned long _mqttStartMs     = 0;
    unsigned long _cooldownStartMs = 0;
    IPAddress     _resolvedIP;
    bool          _dnsResolved     = false;
    bool          _dnsTaskRunning  = false;
    bool          _tcpConnectStarted = false;

    static constexpr unsigned long DNS_TIMEOUT_MS  = 8000;
    static constexpr unsigned long TCP_TIMEOUT_MS  = 6000;
    static constexpr unsigned long MQTT_TIMEOUT_MS = 4000;
    static constexpr unsigned long COOLDOWN_MS     = MQTT_RECONNECT_INTERVAL_MS;

    struct DnsTaskParam {
        char     hostname[128];
        uint32_t resolvedAddr;
        volatile bool done;
    };
    static DnsTaskParam _dnsParam;
    static void _dnsTask(void* param);

    int     _resolvePort() const;
    Client* _activeClient();
    bool    _setupClient();
    void    _tickConnect();
    void    _publishTelemetry(const SystemState& state);
};
