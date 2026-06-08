#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include "config.h"

class TaskManager {
public:
    TaskManager();
    void begin();

    bool isTime(unsigned long& lastTick, unsigned long interval);
    void syncNTP();
    bool isTimeSynced() const { return _ntpSynced; }
    unsigned long getUnixTime() const;

    static void logTaskStats();

private:
    bool _ntpSynced = false;
    unsigned long _lastNtpSync = 0;
    static const unsigned long NTP_SYNC_INTERVAL = 3600000UL; // setiap 1 jam
};
