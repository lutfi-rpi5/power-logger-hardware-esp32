#ifndef DIAGNOSTICS_H
#define DIAGNOSTICS_H

#include <Arduino.h>

// ============================================================
// Runtime diagnostics with configurable log levels
// ============================================================
// No more commenting/uncommenting code — set log level at
// runtime via `diagnostics_setLevel()`.
// ============================================================

enum LogLevel : uint8_t {
    LOG_NONE  = 0,
    LOG_ERROR = 1,
    LOG_WARN  = 2,
    LOG_INFO  = 3,
    LOG_DEBUG = 4
};

class Diagnostics {
public:
    void begin(LogLevel level = LOG_INFO);

    void setLevel(LogLevel level);
    LogLevel getLevel() const;

    void error(const char* tag, const char* fmt, ...);
    void warn(const char* tag, const char* fmt, ...);
    void info(const char* tag, const char* fmt, ...);
    void debug(const char* tag, const char* fmt, ...);

    // Heap / timing helpers
    static void printHeap(const char* tag);
    static void printTaskList();

private:
    LogLevel _level;
    void _log(LogLevel msgLevel, const char* tag, const char* fmt, va_list args);
};

extern Diagnostics diag;

#endif
