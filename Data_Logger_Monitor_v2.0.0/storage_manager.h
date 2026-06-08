#ifndef STORAGE_MANAGER_H
#define STORAGE_MANAGER_H

#include <Arduino.h>
#include "types.h"

#define MAX_WIFI_ENTRIES 10
#define PREF_NAMESPACE   "pwrlog"

class StorageManager {
public:
    void begin();

    // WiFi network list
    int  getWifiCount();
    WiFiEntry getWifiEntry(int index);
    bool addWifiEntry(const char* ssid, const char* pass);
    bool deleteWifiEntry(int index);

    // MQTT config
    MQTTConfig getMQTTConfig();
    bool saveMQTTConfig(const MQTTConfig& cfg);

    // Calibration offsets
    CalibrationData getCalibration();
    bool saveCalibration(const CalibrationData& cal);

    // Thresholds
    ThresholdData getThresholds();
    bool saveThresholds(const ThresholdData& thr);

    // Factory reset
    void factoryReset();

    // Publish sequence counter
    uint32_t getSequence();
    void incrementSequence();

private:
    uint32_t _seq;
};

#endif
