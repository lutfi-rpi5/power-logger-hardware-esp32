#include "task_manager.h"
#include "diagnostics.h"

TaskManager::TaskManager() {}

void TaskManager::begin() {
    diag.info("TASK", "Task manager ready");
}

bool TaskManager::isTime(unsigned long& lastTick, unsigned long interval) {
    unsigned long now = millis();
    if (now - lastTick >= interval) {
        lastTick = now;
        return true;
    }
    return false;
}

void TaskManager::syncNTP() {
    unsigned long now = millis();
    if (_ntpSynced && (now - _lastNtpSync < NTP_SYNC_INTERVAL)) return;

    diag.info("NTP", "Syncing time...");
    configTime(7 * 3600, 0, NTP_SERVER1, NTP_SERVER2);

    time_t t = 0;
    int retries = 0;
    while (t < 100000 && retries < 20) {
        delay(250);
        time(&t);
        retries++;
    }

    if (t > 100000) {
        _ntpSynced = true;
        _lastNtpSync = now;
        struct tm* ti = localtime(&t);
        diag.info("NTP", "Synced: %04d-%02d-%02d %02d:%02d:%02d",
                  ti->tm_year + 1900, ti->tm_mon + 1, ti->tm_mday,
                  ti->tm_hour, ti->tm_min, ti->tm_sec);
    } else {
        diag.warn("NTP", "Sync failed, will retry later");
    }
}

unsigned long TaskManager::getUnixTime() const {
    if (!_ntpSynced) return 0;
    time_t t;
    time(&t);
    return (unsigned long)t;
}

void TaskManager::logTaskStats() {
    #if CONFIG_FREERTOS_USE_TRACE_FACILITY
        char buf[512];
        vTaskList(buf);
        Serial.println(buf);
    #endif
    diag.info("TASK", "Free heap: %u | Max alloc: %u",
              ESP.getFreeHeap(), ESP.getMaxAllocHeap());
}
