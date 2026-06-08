#include "diagnostics.h"
#include <stdarg.h>

/**
 * @file diagnostics.cpp
 * @brief Implementation of the leveled logging system.
 *
 * Output format: [LEVEL][TAG] Message\n
 * Example: [I][MAIN] Setup complete.
 */

/// Global diagnostics instance available to all modules.
Diagnostics diag;

/**
 * @brief Convert a LogLevel to a single-character prefix string.
 * @param lvl The log level.
 * @return Pointer to a static string: "[E]", "[W]", "[I]", "[D]", or "[?]".
 */
static const char* levelPrefix(LogLevel lvl) {
    switch (lvl) {
        case LOG_ERROR: return "[E]";
        case LOG_WARN:  return "[W]";
        case LOG_INFO:  return "[I]";
        case LOG_DEBUG: return "[D]";
        default:        return "[?]";
    }
}

/**
 * @brief Initialise serial port and set the log level.
 * @param level Initial verbosity threshold (default LOG_INFO).
 *
 * @note Includes a 100 ms delay to allow the USB serial to stabilise
 *       before any log output. This is one of the few intentional delay()
 *       calls in the firmware.
 */
void Diagnostics::begin(LogLevel level) {
    _level = level;
    Serial.begin(115200);
    delay(100);  // Allow USB serial to stabilise before first output
    info("DIAG", "Diagnostics initialized, level=%d", _level);
}

void Diagnostics::setLevel(LogLevel level) { _level = level; }
LogLevel Diagnostics::getLevel() const { return _level; }

/**
 * @brief Core logging method — format, level-gate, and output to Serial.
 *
 * @param msgLevel Severity of the current message.
 * @param tag      Module identifier (e.g. "MAIN", "WIFI", "MQTT").
 * @param fmt      printf-style format string.
 * @param args     Variadic argument list (already started by caller).
 */
void Diagnostics::_log(LogLevel msgLevel, const char* tag, const char* fmt, va_list args) {
    // Silently drop messages below the configured verbosity threshold
    if (msgLevel > _level) return;

    char buf[256];
    vsnprintf(buf, sizeof(buf), fmt, args);
    Serial.print(levelPrefix(msgLevel));
    Serial.print("[");
    Serial.print(tag);
    Serial.print("] ");
    Serial.println(buf);
}

void Diagnostics::error(const char* tag, const char* fmt, ...) {
    va_list args; va_start(args, fmt);
    _log(LOG_ERROR, tag, fmt, args); va_end(args);
}

void Diagnostics::warn(const char* tag, const char* fmt, ...) {
    va_list args; va_start(args, fmt);
    _log(LOG_WARN, tag, fmt, args); va_end(args);
}

void Diagnostics::info(const char* tag, const char* fmt, ...) {
    va_list args; va_start(args, fmt);
    _log(LOG_INFO, tag, fmt, args); va_end(args);
}

void Diagnostics::debug(const char* tag, const char* fmt, ...) {
    va_list args; va_start(args, fmt);
    _log(LOG_DEBUG, tag, fmt, args); va_end(args);
}

/**
 * @brief Log current free heap size at DEBUG level.
 * @param tag Module tag for the log line.
 */
void Diagnostics::printHeap(const char* tag) {
    diag.debug(tag, "Free heap: %u bytes", ESP.getFreeHeap());
}

/**
 * @brief Print FreeRTOS task list via vTaskList().
 *
 * Requires CONFIG_FREERTOS_USE_TRACE_FACILITY to be enabled in
 * the ESP32 board configuration (Tools → Core Debug Level).
 * Output includes: Task Name, State, Priority, Stack Remaining, Task#.
 */
void Diagnostics::printTaskList() {
    #if CONFIG_FREERTOS_USE_TRACE_FACILITY
        char buf[512];
        vTaskList(buf);
        Serial.println(buf);
    #endif
}
