#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <functional>
#include "config.h"
#include "types.h"
#include "storage_manager.h"
#include "json_builder.h"

/**
 * @file mqtt_manager.h
 * @brief Non-blocking MQTT client with a 5-state connection state machine.
 *
 * The MQTTManager addresses the root cause of IDF5 watchdog resets:
 * PubSubClient's client.connect(hostname, port) performs a synchronous
 * DNS resolution that can block for 5–8 seconds, triggering the 5 s
 * default WDT. This implementation:
 *
 * 1. **DNS resolution in a separate FreeRTOS task** — the main loop is
 *    never blocked waiting for DNS.
 * 2. **TCP connect to resolved IPAddress directly** — no hostname lookup
 *    during connect().
 * 3. **WDT timeout increased to 30 seconds** — provides headroom for
 *    any remaining synchronous operations.
 *
 * State machine:
 * ```
 * IDLE ──→ DNS_RESOLVING ──→ TCP_CONNECTING ──→ MQTT_CONNECTING ──→ CONNECTED
 *   │           │                 │                     │               │
 *   └───────────┴─────────────────┴─────────────────────┴── COOLDOWN ──┘
 * ```
 */

/**
 * @class MQTTManager
 * @brief Non-blocking MQTT client with async DNS and automatic reconnect.
 *
 * Call tick() from the main loop at every iteration. The manager handles
 * connection state transitions, periodic MQTT.loop() keepalive, and
 * telemetry publishing at MQTT_PUBLISH_INTERVAL_MS intervals.
 *
 * Supports both plain TCP and SSL/TLS connections with optional CA
 * certificate verification.
 */
/**
 * @brief Callback type for MQTT command messages.
 * Receives parsed command name and optional phase argument.
 *
 * @param cmd   Command string ("reboot", "reset_energy")
 * @param phase Phase argument ("all" or empty string)
 */
using MqttCommandCallback = std::function<void(const String& cmd, const String& phase)>;

class MQTTManager {
public:
    /**
     * @brief Construct the manager with an NVS storage reference.
     * @param storage Reference to StorageManager for MQTT config.
     */
    explicit MQTTManager(StorageManager& storage);

    /**
     * @brief Initialise the manager and load config from NVS.
     * Does NOT start a connection — the first retry cycle begins on tick().
     */
    void begin();

    /**
     * @brief Periodic handler called from the main loop (Core 1).
     *
     * Tasks:
     * - Update gState.mqttConnected based on PubSubClient state.
     * - Call _tickConnect() to advance the connection state machine.
     * - Call MQTT.loop() keepalive when connected.
     * - Publish telemetry JSON at the configured interval.
     */
    void tick();

    /**
     * @brief Check current connection status.
     * @return true if PubSubClient::connected() returns true.
     */
    bool isConnected();

    /**
     * @brief Reload MQTT configuration from NVS and reset the connection state.
     * Triggers a full disconnect followed by a fresh connection cycle.
     */
    void reloadConfig();

    /**
     * @brief Register a callback for MQTT command messages.
     *
     * The callback is invoked when a JSON command is received on the
     * command topic (prefix/cmd/DEVICE_ID).
     *
     * @param cb Callable with signature void(const String& cmd, const String& phase)
     */
    void setCommandCallback(MqttCommandCallback cb);

private:
    StorageManager&   _storage;  ///< NVS storage for MQTT config
    MQTTConfig        _cfg;      ///< Current MQTT broker configuration

    WiFiClient        _plainClient;     ///< TCP client for non-SSL connections
    WiFiClientSecure  _secureClient;     ///< SSL/TLS client (optional CA cert)
    PubSubClient      _mqtt;            ///< PubSubClient wrapping the active client

    unsigned long _lastReconnectMs  = 0;  ///< millis() of last reconnect attempt
    unsigned long _lastPublishMs    = 0;  ///< millis() of last telemetry publish
    uint32_t      _publishSeq       = 0;  ///< Monotonic sequence counter for MQTT payloads

    /**
     * @enum ConnState
     * @brief MQTT connection state machine states.
     */
    enum class ConnState : uint8_t {
        IDLE,            ///< Waiting for cooldown to expire before retry
        DNS_RESOLVING,   ///< DNS resolution in a background FreeRTOS task
        TCP_CONNECTING,  ///< TCP socket connect to resolved IP
        MQTT_CONNECTING, ///< MQTT CONNECT packet sent, waiting for CONNACK
        CONNECTED,       ///< Fully connected, normal operation
        COOLDOWN         ///< Cooldown after a failed attempt
    };

    ConnState     _connState       = ConnState::IDLE;  ///< Current state
    unsigned long _dnsStartMs      = 0;  ///< DNS resolution start time
    unsigned long _tcpStartMs      = 0;  ///< TCP connect start time
    unsigned long _mqttStartMs     = 0;  ///< MQTT connect start time
    unsigned long _cooldownStartMs = 0;  ///< Cooldown period start time
    IPAddress     _resolvedIP;           ///< Resolved broker IP address
    bool          _dnsResolved     = false;  ///< DNS resolution completed successfully
    bool          _dnsTaskRunning  = false;  ///< DNS FreeRTOS task is still active
    bool          _tcpConnectStarted = false;  ///< TCP connect() has been called

    /// @name State Machine Timeouts (milliseconds)
    /// @{
    static constexpr unsigned long DNS_TIMEOUT_MS  = 8000;   ///< Max DNS resolution time
    static constexpr unsigned long TCP_TIMEOUT_MS  = 6000;   ///< Max TCP connect time
    static constexpr unsigned long MQTT_TIMEOUT_MS = 4000;   ///< Max MQTT CONNACK wait
    static constexpr unsigned long COOLDOWN_MS     = MQTT_RECONNECT_INTERVAL_MS;  ///< Retry delay after failure
    /// @}

    /**
     * @struct DnsTaskParam
     * @brief Parameter block passed to the DNS resolution FreeRTOS task.
     */
    struct DnsTaskParam {
        char     hostname[128];         ///< Hostname to resolve (null-terminated)
        uint32_t resolvedAddr;          ///< Resolved IP address in network byte order
        volatile bool done;             ///< Set to true by the task when resolution completes
    };

    static DnsTaskParam _dnsParam;  ///< Singleton DNS task parameter block

    /**
     * @brief FreeRTOS task function for background DNS resolution.
     * @param param Pointer to DnsTaskParam.
     */
    static void _dnsTask(void* param);

    /**
     * @brief Determine the port based on SSL/WS configuration.
     * @return Port number for the current connection type.
     */
    int     _resolvePort() const;

    /**
     * @brief Get a pointer to the appropriate WiFiClient based on useSSL flag.
     * @return WiFiClient* (either _plainClient or _secureClient).
     */
    Client* _activeClient();

    /**
     * @brief Configure the WiFiClient with SSL certs (if applicable)
     *        and attach it to PubSubClient.
     * @return true on success.
     */
    bool    _setupClient();

    /**
     * @brief Advance the MQTT connection state machine (one step per call).
     * Called from tick() when not connected.
     */
    void    _tickConnect();

    /**
     * @brief Build and publish a telemetry JSON payload.
     * @param state Current SystemState snapshot to publish.
     */
    void    _publishTelemetry(const SystemState& state);

    // ── MQTT Command Subscription ─────────────────────────────────────

    /**
     * @brief Subscribe to the command topic (prefix/cmd/DEVICE_ID).
     * Called once after MQTT connects successfully.
     */
    void _subscribeToCmd();

    /**
     * @brief Instance method to handle an incoming MQTT message.
     * Parses JSON and dispatches to _cmdCallback if registered.
     *
     * @param topic   The MQTT topic string.
     * @param payload Raw payload bytes.
     * @param length  Payload length in bytes.
     */
    void _onMqttMessage(char* topic, byte* payload, unsigned int length);

    /**
     * @brief Static callback wrapper for PubSubClient.
     * Dispatches to the singleton _instance's _onMqttMessage().
     */
    static void _staticMqttCallback(char* topic, byte* payload, unsigned int length);

    MqttCommandCallback _cmdCallback = nullptr;  ///< Registered command handler
    static MQTTManager* _instance;               ///< Singleton pointer for static callback
    bool                _cmdSubscribed = false;   ///< True after first subscribe to cmd topic
};
