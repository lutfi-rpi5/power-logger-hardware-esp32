#pragma once
#include <Arduino.h>
#include <Preferences.h>
#include "config.h"
#include "types.h"

struct WiFiEntry {
    char ssid[64];
    char pass[64];
};

struct MQTTConfig {
    char   server[128];
    int    port;
    int    sslPort;
    int    wsPort;
    int    wssPort;
    char   user[64];
    char   pass[64];
    char   prefix[64];
    char   topic[64];
    bool   useSSL;
    bool   useWS;
    char   caCert[2048];
};

struct APConfig {
    char ssid[32];
    char pass[32];
};

class StorageManager {
public:
    StorageManager() = default;
    void begin();

    int       getWifiCount() const;
    WiFiEntry getWifiEntry(int index) const;
    bool      addWifiEntry(const char* ssid, const char* pass);
    bool      deleteWifiEntry(int index);
    void      clearAllWifi();

    MQTTConfig       getMQTTConfig() const;
    void             saveMQTTConfig(const MQTTConfig& cfg);

    CalibrationConfig getCalibration() const;
    void              saveCalibration(const CalibrationConfig& cfg);

    APConfig   getAPConfig() const;
    void       saveAPConfig(const APConfig& cfg);

    ThresholdConfig getThresholds() const;
    void            saveThresholds(const ThresholdConfig& cfg);

    void factoryReset();
    void dump() const;

private:
    mutable Preferences _prefs;
    void _open(bool readOnly = false) const;
    void _close() const;
    static void _wifiSSIDKey(int i, char* buf);
    static void _wifiPassKey(int i, char* buf);
};
