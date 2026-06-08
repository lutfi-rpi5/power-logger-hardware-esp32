#pragma once

/******************************************************
 * File      : StorageManager.h
 * Description:
 *   Persistent storage for all runtime-configurable
 *   settings using ESP32 Preferences (NVS flash).
 *
 *   Stored settings:
 *     - Known WiFi networks (up to MAX_WIFI_NETWORKS)
 *     - MQTT broker connection parameters
 *     - SSL/TLS CA certificate
 *     - Per-phase calibration offsets (voltage & current)
 *     - Access Point credentials
 *     - Line status voltage thresholds (LOST/UNDER/OVER/Unbalance)
 ******************************************************/

#include <Arduino.h>
#include <Preferences.h>
#include "config.h"

// ─── WiFi Network Entry ───────────────────────────────────
struct WiFiEntry {
    char ssid[64];
    char pass[64];
};

// ─── MQTT Configuration ───────────────────────────────────
struct MQTTConfig {
    char   server[128];
    int    port;
    int    sslPort;
    int    wsPort;
    int    wssPort;
    char   user[64];
    char   pass[64];
    char   topic[64];    // prefix, e.g. "lutpiii/"
    bool   useSSL;
    bool   useWS;
    char   caCert[2048]; // PEM certificate string
};

// ─── Calibration Offsets ─────────────────────────────────
struct CalibrationConfig {
    float voltageOffset[3];  // additive offset per phase (R, S, T)
    float currentOffset[3];
};

// ─── Access Point Config ──────────────────────────────────
struct APConfig {
    char ssid[32];
    char pass[32];
};

// ─── Line Status Threshold Config (stored in EEPROM) ─────
struct ThresholdConfig {
    float voltageLost;       // Voltage below this → LOST   (default: VOLTAGE_LOST_MAX)
    float voltageUnder;      // Voltage below this → UNDER  (default: VOLTAGE_OK_MIN)
    float voltageOver;       // Voltage above this → OVER   (default: VOLTAGE_OVER_MAX)
    float unbalanceMax;      // Unbalance above this % → warning (default: VOLTAGE_UNBALANCE_MAX)
};

// ─────────────────────────────────────────────────────────
class StorageManager {
public:
    StorageManager() = default;

    /** Must be called once in setup() */
    void begin();

    // --- WiFi Networks ---
    int       getWifiCount() const;
    WiFiEntry getWifiEntry(int index) const;
    bool      addWifiEntry(const char* ssid, const char* pass);
    bool      deleteWifiEntry(int index);
    void      clearAllWifi();

    // --- MQTT Config ---
    MQTTConfig getMQTTConfig() const;
    void       saveMQTTConfig(const MQTTConfig& cfg);

    // --- Calibration ---
    CalibrationConfig getCalibration() const;
    void              saveCalibration(const CalibrationConfig& cfg);

    // --- AP Config ---
    APConfig getAPConfig() const;
    void     saveAPConfig(const APConfig& cfg);

    // --- Threshold Config ---
    ThresholdConfig getThresholds() const;
    void            saveThresholds(const ThresholdConfig& cfg);

    /** Wipe all stored settings back to defaults */
    void factoryReset();

    /** Print all stored settings to Serial (debug) */
    void dump() const;

private:
    mutable Preferences _prefs;

    void _open(bool readOnly = false) const;
    void _close() const;

    // Key builders
    static void _wifiSSIDKey(int i, char* buf);
    static void _wifiPassKey(int i, char* buf);
};
