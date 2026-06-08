#pragma once
#include <Arduino.h>
#include <Preferences.h>
#include "config.h"
#include "types.h"

/**
 * @file storage_manager.h
 * @brief Persistent configuration storage using ESP32 NVS (Non-Volatile Storage)
 *        via the Preferences library.
 *
 * Manages read/write access to four configuration sections:
 * - WiFi network list (up to MAX_WIFI_NETWORKS entries).
 * - MQTT broker settings (server, ports, credentials, topic prefix, SSL cert).
 * - PZEM calibration offsets (per-phase V/I).
 * - Line status thresholds (LOST/UNDER/OVER voltage limits, unbalance max).
 * - AP (Access Point) config for the web configuration portal.
 *
 * All values are key-value pairs in the "dlcfg" namespace. On factory reset,
 * the entire namespace is cleared and defaults from config.h are used.
 */

/**
 * @struct WiFiEntry
 * @brief A single stored WiFi network credential.
 */
struct WiFiEntry {
    char ssid[64];  ///< Network SSID (null-terminated)
    char pass[64];  ///< Network password (null-terminated)
};

/**
 * @struct MQTTConfig
 * @brief MQTT broker connection parameters, persistable in NVS.
 */
struct MQTTConfig {
    char   server[128];  ///< Broker hostname or IP address
    int    port;         ///< MQTT TCP port (non-SSL)
    int    sslPort;      ///< MQTT SSL port
    int    wsPort;       ///< MQTT WebSocket port (reference only)
    int    wssPort;      ///< MQTT WebSocket Secure port (reference only)
    char   user[64];     ///< MQTT username (empty = anonymous)
    char   pass[64];     ///< MQTT password (empty = no auth)
    char   prefix[64];   ///< Topic prefix (e.g. "lutpiii")
    char   topic[64];    ///< Topic name (e.g. "telemetry"); full topic: prefix/topic/device_id
    bool   useSSL;       ///< Enable SSL/TLS connection
    bool   useWS;        ///< Enable WebSocket (not implemented; reserved)
    char   caCert[2048]; ///< CA certificate for SSL verification (PEM format)
};

/**
 * @struct APConfig
 * @brief Soft-AP configuration for the web configuration portal.
 */
struct APConfig {
    char ssid[32];  ///< AP SSID
    char pass[32];  ///< AP password (min 8 chars)
};

/**
 * @class StorageManager
 * @brief NVS-backed persistent configuration storage.
 *
 * Wraps the ESP32 Preferences library with typed accessors and automatic
 * default fallback when NVS keys are missing (e.g. after factory reset
 * or first boot).
 *
 * Thread safety: Not guaranteed — intended to be called only from Core 1
 * (loop task). Each public method opens and closes the Preferences
 * namespace, minimising the window for concurrent access issues.
 */
class StorageManager {
public:
    /**
     * @brief Default constructor. Call begin() before any other method.
     */
    StorageManager() = default;

    /**
     * @brief Initialise NVS storage and log current state.
     * Opens and immediately closes the namespace to verify NVS is working.
     */
    void begin();

    // ── WiFi Network List ────────────────────────────────────────────

    /**
     * @brief Get the number of stored WiFi networks.
     * @return Count of entries (0 = no networks stored).
     */
    int       getWifiCount() const;

    /**
     * @brief Get a WiFi entry by index.
     * @param index 0-based index into the stored list.
     * @return WiFiEntry struct (ssid/pass). Empty strings if index is out of range.
     */
    WiFiEntry getWifiEntry(int index) const;

    /**
     * @brief Add a new WiFi network to the end of the list.
     * @param ssid Network SSID.
     * @param pass Network password.
     * @return true if added successfully, false if list is full.
     */
    bool      addWifiEntry(const char* ssid, const char* pass);

    /**
     * @brief Delete a WiFi entry by index, shifting subsequent entries up.
     * @param index 0-based index to delete.
     * @return true if deleted, false if index is out of range.
     */
    bool      deleteWifiEntry(int index);

    /**
     * @brief Remove all stored WiFi networks from NVS.
     */
    void      clearAllWifi();

    // ── MQTT Configuration ───────────────────────────────────────────

    /**
     * @brief Load the MQTT config from NVS (with defaults fallback).
     * @return MQTTConfig struct with current or default values.
     */
    MQTTConfig       getMQTTConfig() const;

    /**
     * @brief Save the MQTT config to NVS.
     * @param cfg MQTTConfig to persist.
     */
    void             saveMQTTConfig(const MQTTConfig& cfg);

    // ── Calibration Offsets ─────────────────────────────────────────

    /**
     * @brief Load the per-phase V/I calibration offsets from NVS.
     * @return CalibrationConfig with offset arrays.
     */
    CalibrationConfig getCalibration() const;

    /**
     * @brief Save the per-phase V/I calibration offsets to NVS.
     * @param cfg CalibrationConfig to persist.
     */
    void              saveCalibration(const CalibrationConfig& cfg);

    // ── AP Configuration ────────────────────────────────────────────

    /**
     * @brief Load the AP config from NVS (with defaults fallback).
     * @return APConfig struct.
     */
    APConfig   getAPConfig() const;

    /**
     * @brief Save the AP config to NVS.
     * @param cfg APConfig to persist.
     */
    void       saveAPConfig(const APConfig& cfg);

    // ── Line Status Thresholds ──────────────────────────────────────

    /**
     * @brief Load the line status thresholds from NVS (with defaults fallback).
     * @return ThresholdConfig struct.
     */
    ThresholdConfig getThresholds() const;

    /**
     * @brief Save the line status thresholds to NVS.
     * @param cfg ThresholdConfig to persist.
     */
    void            saveThresholds(const ThresholdConfig& cfg);

    // ── Factory Reset ───────────────────────────────────────────────

    /**
     * @brief Erase ALL stored configuration from NVS and force default values.
     * @note Does NOT trigger a reboot — caller must initiate ESP.restart().
     */
    void factoryReset();

    /**
     * @brief Log the entire NVS content at INFO level for debugging.
     */
    void dump() const;

private:
    mutable Preferences _prefs;  ///< ESP32 Preferences instance (mutable for const getters)

    /**
     * @brief Open the NVS namespace in read-only or read-write mode.
     * @param readOnly If true, opens in read-only mode.
     */
    void _open(bool readOnly = false) const;

    /**
     * @brief Close the NVS namespace.
     */
    void _close() const;

    /**
     * @brief Generate the Preferences key string for a WiFi SSID at a given index.
     * @param i  Index (0–MAX_WIFI_NETWORKS-1).
     * @param buf Output buffer (min 16 chars).
     */
    static void _wifiSSIDKey(int i, char* buf);

    /**
     * @brief Generate the Preferences key string for a WiFi password at a given index.
     * @param i  Index (0–MAX_WIFI_NETWORKS-1).
     * @param buf Output buffer (min 16 chars).
     */
    static void _wifiPassKey(int i, char* buf);
};
