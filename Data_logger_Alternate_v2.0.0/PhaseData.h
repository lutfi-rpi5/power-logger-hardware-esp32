#pragma once

/******************************************************
 * File      : PhaseData.h
 * Description:
 *   Shared data structures for the entire system.
 *   All data is protected by a FreeRTOS mutex.
 *   Use getStateCopy() for safe read access from any task.
 ******************************************************/

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

// ─── Line Status ──────────────────────────────────────────
enum class LineStatus : uint8_t {
    OK    = 0,  // Voltage within normal range
    UNDER = 1,  // Voltage below VOLTAGE_OK_MIN
    OVER  = 2,  // Voltage above VOLTAGE_OVER_MAX
    LOST  = 3   // No voltage detected / NaN reading
};

inline const char* lineStatusStr(LineStatus s) {
    switch (s) {
        case LineStatus::OK:    return "OK";
        case LineStatus::UNDER: return "UNDER";
        case LineStatus::OVER:  return "OVER";
        case LineStatus::LOST:  return "LOST";
        default:                return "ERR";
    }
}

// ─── Per-Phase Measurement ────────────────────────────────
struct PhaseReading {
    float voltage       = 0.0f;   // V
    float current       = 0.0f;   // A
    float activePower   = 0.0f;   // W
    float apparentPower = 0.0f;   // VA
    float reactivePower = 0.0f;   // VAr  (computed: sqrt(S²-P²))
    float powerFactor   = 0.0f;   // 0.0–1.0
    float frequency     = 0.0f;   // Hz
    float energyWh      = 0.0f;   // Wh  (cumulative, from PZEM)
    LineStatus status   = LineStatus::LOST;
    bool valid          = false;  // false if PZEM returned NaN
    unsigned long timestampMs = 0;
};

// ─── Full System State ────────────────────────────────────
struct SystemState {
    PhaseReading phases[3];       // Index 0=R, 1=S, 2=T

    // 3-Phase unbalance (computed from voltages R,S,T)
    float unbalance          = 0.0f;  // % voltage unbalance (NEMA definition)
    float thresholdUnbalanceMax = 3.0f; // runtime max unbalance threshold (%)

    // Connectivity
    bool   wifiConnected  = false;
    bool   mqttConnected  = false;
    int32_t wifiRSSI      = 0;

    // System info
    uint32_t freeHeapBytes  = 0;
    uint32_t uptimeSeconds  = 0;

    // Web server / AP
    bool   webServerActive = false;
    char   apSSID[32]      = "";
    char   apPass[32]      = "";
    char   apIP[16]        = "";
};

// ─── Global instances (defined in Data_Logger_Monitor.ino) ─
extern SystemState       gState;
extern SemaphoreHandle_t gStateMutex;

// ─── Thread-safe accessor ─────────────────────────────────
/**
 * Returns a snapshot copy of the global system state.
 * Safe to call from any task/context.
 * Returns stale copy (from last successful lock) on timeout.
 */
inline SystemState getStateCopy() {
    static SystemState cached;
    if (xSemaphoreTake(gStateMutex, pdMS_TO_TICKS(50)) == pdTRUE) {
        cached = gState;
        xSemaphoreGive(gStateMutex);
    }
    return cached;
}

/**
 * Thread-safe updater – write a single field without replacing
 * the whole struct. Use a lambda for the mutation.
 * Example:
 *   updateState([](SystemState& s){ s.wifiConnected = true; });
 */
template<typename F>
inline void updateState(F fn) {
    if (xSemaphoreTake(gStateMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        fn(gState);
        xSemaphoreGive(gStateMutex);
    }
}
