#pragma once
#include <Arduino.h>

/**
 * @file diagnostics.h
 * @brief Leveled logging system that replaces raw Serial.print/printf calls.
 *
 * Provides five log levels (NONE → DEBUG) with tagged, timestamp-style
 * output. Designed for embedded debugging where serial bandwidth and
 * flash space are constrained.
 *
 * Usage:
 * @code
 * diag.begin(LOG_INFO);
 * diag.info("TAG", "Boot complete. Heap: %u", ESP.getFreeHeap());
 * diag.warn("WIFI", "Connection timeout after %lu ms", elapsed);
 * diag.error("WDT", "Heap critical — rebooting!");
 * @endcode
 *
 * Output format:
 * ```
 * [I][TAG] Informational message here
 * [W][WIFI] Warning with value 42
 * [E][WDT] Error — critical condition
 * ```
 *
 * @note A global instance `diag` is declared extern and defined in
 *       diagnostics.cpp. All modules include this header to access it.
 */

/**
 * @enum LogLevel
 * @brief Verbosity levels for the diagnostics logger.
 *
 * Higher numbers include all lower levels. For example, LOG_INFO
 * outputs ERROR + WARN + INFO but not DEBUG.
 */
enum LogLevel : uint8_t {
    LOG_NONE  = 0,  ///< Suppress all output
    LOG_ERROR = 1,  ///< Error messages only (non-recoverable failures)
    LOG_WARN  = 2,  ///< Error + Warning messages
    LOG_INFO  = 3,  ///< Error + Warning + Informational (default)
    LOG_DEBUG = 4   ///< All messages including verbose debug output
};

/**
 * @class Diagnostics
 * @brief Singleton logger with level gating and printf-style formatting.
 *
 * Thread safety: Not guaranteed — intended for single-core Serial output
 * from the loop task (Core 1). The DAQ task (Core 0) should avoid heap
 * operations including sprintf; use diag.debug() sparingly or not at all
 * from Core 0.
 */
class Diagnostics {
public:
    /**
     * @brief Initialise the logger and open the hardware serial port.
     * @param level Initial log verbosity (default: LOG_INFO).
     * @note Calls Serial.begin(115200) with a 100 ms stabilisation delay.
     */
    void begin(LogLevel level = LOG_INFO);

    /**
     * @brief Change the log verbosity at runtime.
     * @param level New LogLevel (LOG_NONE through LOG_DEBUG).
     */
    void setLevel(LogLevel level);

    /**
     * @brief Get the current log verbosity level.
     * @return Current LogLevel value.
     */
    LogLevel getLevel() const;

    /**
     * @name Logging Methods
     * Each accepts a tag (3–6 chars recommended) and a printf-style format
     * string with variadic arguments.
     * @{
     */
    void error(const char* tag, const char* fmt, ...);  ///< [E] Non-recoverable failures
    void warn(const char* tag, const char* fmt, ...);   ///< [W] Recoverable issues
    void info(const char* tag, const char* fmt, ...);   ///< [I] Normal operational messages
    void debug(const char* tag, const char* fmt, ...);  ///< [D] Verbose debugging (compile out in production)
    /// @}

    /**
     * @brief Log the current free heap size at DEBUG level.
     * @param tag Module tag for the log line.
     */
    static void printHeap(const char* tag);

    /**
     * @brief Print FreeRTOS task list (requires CONFIG_FREERTOS_USE_TRACE_FACILITY).
     * Output includes Task Name, State, Priority, Stack, and Task Number.
     */
    static void printTaskList();

private:
    LogLevel _level;  ///< Current verbosity threshold

    /**
     * @brief Internal formatting and output method.
     * @param msgLevel Severity of this message
     * @param tag      Module identifier
     * @param fmt      printf-style format string
     * @param args     Variadic argument list
     */
    void _log(LogLevel msgLevel, const char* tag, const char* fmt, va_list args);
};

/// Global diagnostics instance. Defined in diagnostics.cpp.
extern Diagnostics diag;
