#include "task_manager.h"
#include "diagnostics.h"

/**
 * @file task_manager.cpp
 * @brief Millis-based scheduler and NTP time synchronisation utility.
 */

TaskManager::TaskManager() {}

/**
 * @brief Initialise the task manager.
 * Currently a no-op; reserved for future setup operations.
 */
void TaskManager::begin() {
    diag.info("TASK", "Task manager ready");
}

/**
 * @brief Non-blocking periodic timer using millis().
 *
 * Returns true once when the configured interval has elapsed since the
 * last call that returned true. The reference timestamp is updated
 * in-place so the next period starts immediately.
 *
 * @param lastTick Reference to the caller's last-tick variable (updated in-place).
 * @param interval Period in milliseconds.
 * @return true if the interval has elapsed.
 */
bool TaskManager::isTime(unsigned long& lastTick, unsigned long interval) {
    unsigned long now = millis();
    if (now - lastTick >= interval) {
        lastTick = now;
        return true;
    }
    return false;
}

/**
 * @brief Synchronise the ESP32 system clock via NTP.
 *
 * Calls configTime() with the configured NTP servers and timezone
 * offset (WIB = UTC+7). Uses a polling loop with up to @ref NTP_RETRIES
 * attempts at @ref NTP_RETRY_DELAY_MS intervals.
 *
 * Reference: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/system_time.html
 *
 * @note Blocking: the polling loop can take up to ~5 seconds (20 × 250 ms).
 *       This method should NOT be called from the sensor task (Core 0).
 * @note Syncs at most once per @ref NTP_SYNC_INTERVAL (1 hour).
 *
 * Timezone: WIB (Western Indonesian Time) = UTC+7, no DST.
 * For other timezones, adjust the configTime() call:
 *   configTime(timezoneOffset * 3600, 0, server1, server2);
 */
void TaskManager::syncNTP() {
    unsigned long now = millis();

    // ── Rate-limit: skip if we synced less than 1 hour ago ───────────
    if (_ntpSynced && (now - _lastNtpSync < NTP_SYNC_INTERVAL)) return;

    diag.info("NTP", "Syncing time...");

    // Configure NTP: WIB = UTC+7 (7 × 3600 = 25200 seconds offset)
    configTime(7 * 3600, 0, NTP_SERVER1, NTP_SERVER2);

    // ── Poll until time is valid or retries exhausted ─────────────────
    // A valid Unix timestamp is > 100000 (well past 1970).
    // Typical sync takes 1–3 seconds on a good network.
    time_t t = 0;
    static const int NTP_RETRIES = 20;          // Max polling attempts
    static const int NTP_RETRY_DELAY_MS = 250;  // Delay between polls (ms)
    int retries = 0;
    while (t < 100000 && retries < NTP_RETRIES) {
        delay(NTP_RETRY_DELAY_MS);  // Allow time for the NTP response
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
        diag.warn("NTP", "Sync failed after %d attempts, will retry later", NTP_RETRIES);
    }
}

/**
 * @brief Get the current Unix epoch timestamp.
 * @return Seconds since 1970-01-01 00:00:00 UTC, or 0 if NTP not synced.
 */
unsigned long TaskManager::getUnixTime() const {
    if (!_ntpSynced) return 0;
    time_t t;
    time(&t);
    return (unsigned long)t;
}

/**
 * @brief Log FreeRTOS task statistics and heap information.
 *
 * Requires CONFIG_FREERTOS_USE_TRACE_FACILITY to be enabled for
 * vTaskList() output. Heap info is always available.
 */
void TaskManager::logTaskStats() {
    #if CONFIG_FREERTOS_USE_TRACE_FACILITY
        char buf[512];
        vTaskList(buf);
        Serial.println(buf);
    #endif
    diag.info("TASK", "Free heap: %u | Max alloc: %u",
              ESP.getFreeHeap(), ESP.getMaxAllocHeap());
}
