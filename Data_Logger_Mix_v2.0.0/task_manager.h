#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include "config.h"

/**
 * @file task_manager.h
 * @brief Utility class for millis-based scheduling and NTP time synchronisation.
 *
 * Provides two core services:
 * 1. isTime() — a millis()-based interval scheduler that eliminates the
 *    need for blocking delay() in periodic tasks.
 * 2. syncNTP() — NTP client that periodically synchronises the ESP32
 *    system time (for future epoch-based MQTT timestamps).
 *
 * The NTP sync uses configTime() with a blocking retry loop (5 s max).
 * This is acceptable because it is called only when WiFi is connected
 * and at most once per hour.
 */

/**
 * @class TaskManager
 * @brief Scheduler and NTP sync utility.
 *
 * Stateless helper — can be used globally. isTime() works with any
 * unsigned long variable that persists between calls (e.g. a static
 * local or member variable).
 */
class TaskManager {
public:
    /**
     * @brief Construct the task manager.
     */
    TaskManager();

    /**
     * @brief Initialise. Currently a no-op, reserved for future setup.
     */
    void begin();

    /**
     * @brief Non-blocking interval timer using millis().
     *
     * Returns true once per interval, then resets the reference timestamp.
     * Typical usage:
     * @code
     * static unsigned long lastTick = 0;
     * if (taskMgr.isTime(lastTick, 5000)) {
     *     // This block executes every 5 seconds
     * }
     * @endcode
     *
     * @param lastTick Reference to the last-tick timestamp (updated in-place).
     * @param interval Interval in milliseconds.
     * @return true when the interval has elapsed (fires once per period).
     */
    bool isTime(unsigned long& lastTick, unsigned long interval);

    /**
     * @brief Synchronise the ESP32 system clock via NTP.
     *
     * Calls configTime() with the configured NTP servers and timezone
     * offset (WIB = UTC+7). Uses a blocking retry loop with up to 20
     * attempts at 250 ms intervals.
     *
     * @note Only executes if WiFi is connected and the last sync is
     *       older than NTP_SYNC_INTERVAL (1 hour).
     * @note This method contains delay() calls (~5 s total blocking).
     *       It must NOT be called from the sensor task (Core 0).
     */
    void syncNTP();

    /**
     * @brief Check if NTP has ever successfully synchronised.
     * @return true if at least one successful NTP sync has completed.
     */
    bool isTimeSynced() const { return _ntpSynced; }

    /**
     * @brief Get the current Unix epoch timestamp.
     * @return Unix timestamp (seconds since 1970-01-01), or 0 if not synced.
     */
    unsigned long getUnixTime() const;

    /**
     * @brief Print FreeRTOS task statistics and heap usage to Serial.
     */
    static void logTaskStats();

private:
    bool _ntpSynced = false;           ///< true after first successful NTP sync
    unsigned long _lastNtpSync = 0;    ///< millis() timestamp of last NTP sync

    /// Minimum interval between NTP sync attempts (1 hour in ms).
    static const unsigned long NTP_SYNC_INTERVAL = 3600000UL;
};
