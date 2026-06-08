#include "diagnostics.h"
#include <stdarg.h>

Diagnostics diag;

static const char* levelPrefix(LogLevel lvl) {
    switch (lvl) {
        case LOG_ERROR: return "[E]";
        case LOG_WARN:  return "[W]";
        case LOG_INFO:  return "[I]";
        case LOG_DEBUG: return "[D]";
        default:        return "[?]";
    }
}

void Diagnostics::begin(LogLevel level) {
    _level = level;
    Serial.begin(115200);
    // Small delay to let Serial stabilize on ESP32
    delay(100);
    info("DIAG", "Diagnostics initialized, level=%d", _level);
}

void Diagnostics::setLevel(LogLevel level) {
    _level = level;
}

LogLevel Diagnostics::getLevel() const {
    return _level;
}

void Diagnostics::_log(LogLevel msgLevel, const char* tag, const char* fmt, va_list args) {
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
    va_list args;
    va_start(args, fmt);
    _log(LOG_ERROR, tag, fmt, args);
    va_end(args);
}

void Diagnostics::warn(const char* tag, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    _log(LOG_WARN, tag, fmt, args);
    va_end(args);
}

void Diagnostics::info(const char* tag, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    _log(LOG_INFO, tag, fmt, args);
    va_end(args);
}

void Diagnostics::debug(const char* tag, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    _log(LOG_DEBUG, tag, fmt, args);
    va_end(args);
}

void Diagnostics::printHeap(const char* tag) {
    diag.debug(tag, "Free heap: %u bytes", ESP.getFreeHeap());
}

void Diagnostics::printTaskList() {
    // Only available in FreeRTOS builds
    #if CONFIG_FREERTOS_USE_TRACE_FACILITY
        char buf[512];
        vTaskList(buf);
        Serial.println(buf);
    #endif
}
