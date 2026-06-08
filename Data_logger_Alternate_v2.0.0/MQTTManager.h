#pragma once

/******************************************************
 * File      : MQTTManager.h
 * Description:
 *   MQTT client wrapper with:
 *   - Non-blocking reconnect (timer-based)
 *   - JSON payload builder (ArduinoJson)
 *   - SSL/TLS support via WiFiClientSecure
 *   - Publish to: {topic}telemetry
 *
 *   JSON Structure (published to MQTT):
 *   {
 *     "device": { "id", "fw", "uptime", "heap", "rssi" },
 *     "phases": {
 *       "R": { "v","i","p","s","q","pf","f","e","status" },
 *       "S": { ... },
 *       "T": { ... }
 *     },
 *     "ts": <millis>
 *   }
 *
 *   Library required: PubSubClient, ArduinoJson
 ******************************************************/

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "config.h"
#include "PhaseData.h"
#include "StorageManager.h"

class MQTTManager {
public:
    explicit MQTTManager(StorageManager& storage);

    /** Call once in setup() after WiFi begin. */
    void begin();

    /**
     * Call every loop(). Fully non-blocking:
     *   - PubSubClient::loop() (keep-alive ping)
     *   - Non-blocking TCP+MQTT connect state machine
     *   - Periodic JSON publish
     *   - Never blocks loop() — WDT safe even without internet
     */
    void tick();

    /** True if currently connected to broker. */
    bool isConnected();

    /** Reload config from EEPROM (call after webserver saves changes). */
    void reloadConfig();

private:
    StorageManager&   _storage;
    MQTTConfig        _cfg;

    WiFiClient        _plainClient;
    WiFiClientSecure  _secureClient;
    PubSubClient      _mqtt;           // points to one of the above

    unsigned long _lastReconnectMs  = 0;
    unsigned long _lastPublishMs    = 0;

    // ── Non-blocking connect state machine ──────────────────────────────────
    //
    // Root cause reboot yang sudah diidentifikasi:
    //   client.connect(hostname, port) → DNS resolution BLOCKING di loop task
    //   Tanpa internet, DNS timeout ~5–6 detik.
    //   WDT default IDF5 = 5 detik → WDT fire sebelum DNS selesai → reboot.
    //
    // Dua perbaikan:
    //   1. WDT timeout di-reconfigure ke 30s (_initWatchdog di .ino)
    //   2. DNS resolution dipindah ke state DNS_RESOLVING menggunakan
    //      FreeRTOS task background → loop TIDAK pernah block untuk DNS.
    //      Setelah IP diketahui, connect ke IPAddress langsung (tanpa DNS).
    //
    // Alur state:
    //   IDLE → DNS_RESOLVING → TCP_CONNECTING → MQTT_CONNECTING → CONNECTED
    //               ↓               ↓                 ↓               ↓
    //            (timeout/fail)  (timeout/fail)   (timeout/fail)  (drop)
    //               └───────────────────────────────────┴── COOLDOWN ─┘
    //
    enum class ConnState : uint8_t {
        IDLE,            // tidak ada usaha connect
        DNS_RESOLVING,   // menunggu DNS resolution di background task
        TCP_CONNECTING,  // menunggu TCP handshake (connect ke IP langsung)
        MQTT_CONNECTING, // TCP tersambung, kirim MQTT CONNECT dan tunggu CONNACK
        CONNECTED,       // fully connected
        COOLDOWN         // gagal, tunggu sebelum retry
    };
    ConnState     _connState       = ConnState::IDLE;
    unsigned long _dnsStartMs         = 0;
    unsigned long _tcpStartMs         = 0;
    unsigned long _mqttStartMs        = 0;
    unsigned long _cooldownStartMs    = 0;
    IPAddress     _resolvedIP;               // hasil DNS resolution
    bool          _dnsResolved        = false;
    bool          _dnsTaskRunning     = false;
    bool          _tcpConnectStarted  = false;  // guard: connect() hanya boleh dipanggil 1x

    static constexpr unsigned long DNS_TIMEOUT_MS  = 8000;   // DNS max 8s
    static constexpr unsigned long TCP_TIMEOUT_MS  = 6000;   // TCP handshake max 6s
    static constexpr unsigned long MQTT_TIMEOUT_MS = 4000;   // MQTT CONNACK max 4s
    static constexpr unsigned long COOLDOWN_MS     = MQTT_RECONNECT_INTERVAL_MS;

    // DNS background task support
    struct DnsTaskParam {
        char     hostname[128];
        uint32_t resolvedAddr;   // 0 = gagal, non-zero = berhasil
        volatile bool done;
    };
    static DnsTaskParam _dnsParam;    // static: hanya 1 instance, lifecycle terkontrol
    static void _dnsTask(void* param);

    Client* _activeClient();
    bool    _setupClient();
    void    _tickConnect();
    void    _publishTelemetry(const SystemState& state);
    void    _buildJson(const SystemState& state, char* buf, size_t len);
    void    _addPhase(JsonObject& obj, const PhaseReading& r);
};