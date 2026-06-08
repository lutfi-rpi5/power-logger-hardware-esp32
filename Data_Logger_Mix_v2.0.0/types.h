#pragma once
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

/**
 * @file types.h
 * @brief Shared data structures, enumerations, and thread-safe state access helpers.
 *
 * This file defines the core data types used across all modules:
 * - LineStatus: Per-phase health classification (OK / UNDER / OVER / LOST).
 * - PhaseReading: Calibrated measurement values for a single phase.
 * - SystemState: Snapshot of the entire device state, protected by a mutex.
 * - CalibrationConfig: Persistable V/I offset values per phase.
 * - ThresholdConfig: Persistable line status threshold values.
 *
 * Thread safety model:
 * - gState (global SystemState) is read/written exclusively through
 *   getStateCopy() and updateState(), both of which acquire gStateMutex.
 * - Core 0 (sensor task) writes PhaseReading data at 1 Hz.
 * - Core 1 (loop) reads snapshots and writes WiFi/MQTT/webserver meta fields.
 */

/**
 * @enum LineStatus
 * @brief Health status classification for a single phase line.
 *
 * Determined by comparing calibrated voltage against the configured
 * thresholds (see @ref config.h VOLTAGE_LOST_MAX, VOLTAGE_OK_MIN,
 * VOLTAGE_OVER_MAX).
 */
enum class LineStatus : uint8_t {
    OK    = 0,  ///< Voltage within normal operating range
    UNDER = 1,  ///< Voltage below normal minimum but above lost threshold (brown-out)
    OVER  = 2,  ///< Voltage exceeds safe maximum (over-voltage)
    LOST  = 3   ///< Voltage too low for meaningful measurement, or sensor not responding
};

/**
 * @brief Convert LineStatus enum to a human-readable string.
 * @param s The LineStatus value
 * @return Pointer to a static string: "OK", "UNDER", "OVER", "LOST", or "ERR"
 */
inline const char* lineStatusStr(LineStatus s) {
    switch (s) {
        case LineStatus::OK:    return "OK";
        case LineStatus::UNDER: return "UNDER";
        case LineStatus::OVER:  return "OVER";
        case LineStatus::LOST:  return "LOST";
        default:                return "ERR";
    }
}

/**
 * @struct PhaseReading
 * @brief Calibrated measurement data for a single power phase.
 *
 * All values are calibrated (offsets applied) and rounded to match
 * the MQTT JSON schema precision. The valid flag distinguishes real
 * zero readings from sensor failures.
 */
struct PhaseReading {
    float voltage       = 0.0f;  ///< Phase-to-neutral voltage (V), 1 decimal
    float current       = 0.0f;  ///< Line current (A), 2 decimals
    float activePower   = 0.0f;  ///< Real/active power (W), 1 decimal
    float apparentPower = 0.0f;  ///< Apparent power (VA), 1 decimal
    float reactivePower = 0.0f;  ///< Reactive power (VAr), 1 decimal
    float powerFactor   = 0.0f;  ///< Power factor (cos φ), 2 decimals (0.00–1.00)
    float frequency     = 0.0f;  ///< Line frequency (Hz), 1 decimal
    float energyWh      = 0.0f;  ///< Cumulative active energy (Wh), 1 decimal
    LineStatus status   = LineStatus::LOST;  ///< Line health classification
    bool valid          = false; ///< Data validity flag. false = sensor error / LOST
    unsigned long timestampMs = 0; ///< Local millis() timestamp when reading was taken
};

/**
 * @struct SystemState
 * @brief Complete device state snapshot, shared between cores via mutex.
 *
 * This struct is the single source of truth for all real-time data.
 * Core 0 (sensor task) writes the phases[] array at 1 Hz.
 * Core 1 (loop) writes connectivity and metadata fields.
 */
struct SystemState {
    PhaseReading phases[3];           ///< Per-phase readings: [0]=R, [1]=S, [2]=T
    float unbalance              = 0.0f;  ///< Voltage unbalance percentage (NEMA method)
    float thresholdUnbalanceMax  = 3.0f;  ///< Unbalance warning threshold loaded from NVS (%)

    bool   wifiConnected  = false;  ///< WiFi connection status
    bool   mqttConnected  = false;  ///< MQTT broker connection status
    int32_t wifiRSSI      = 0;      ///< WiFi signal strength (dBm)

    uint32_t freeHeapBytes  = 0;    ///< Free heap memory at last sample (bytes)
    uint32_t uptimeSeconds  = 0;    ///< Seconds elapsed since boot

    bool   webServerActive = false; ///< Web configuration server is running (AP mode)
    char   apSSID[32]      = "";    ///< Active AP SSID (when webserver is on)
    char   apPass[32]      = "";    ///< Active AP password
    char   apIP[16]        = "";    ///< Active AP IP address string (e.g. "192.168.4.1")
};

/**
 * @name Global State & Mutex
 * Declared extern here, defined in Data_Logger_Mix_v2.0.0.ino.
 * @{
 */
extern SystemState       gState;       ///< Global device state (mutex-protected)
extern SemaphoreHandle_t gStateMutex;  ///< FreeRTOS mutex guarding gState access
/// @}

/**
 * @brief Thread-safe snapshot read of the global state.
 *
 * Attempts to acquire gStateMutex with a 50 ms timeout. If the mutex is
 * held by the writer (Core 0), returns the last cached snapshot instead
 * of blocking the caller indefinitely.
 *
 * @return Copy of the current SystemState (may be slightly stale if
 *         the writer is busy, which is acceptable at 1 Hz update rate).
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
 * @brief Thread-safe state update via a lambda/function object.
 *
 * Acquires gStateMutex with a 100 ms timeout, then passes a mutable
 * reference to the provided callback. The callback should modify gState
 * directly through the reference.
 *
 * Usage:
 * @code
 * updateState([&](SystemState& s) {
 *     s.wifiConnected = true;
 *     s.wifiRSSI      = WiFi.RSSI();
 * });
 * @endcode
 *
 * @param fn Callable that accepts SystemState& and modifies it.
 */
template<typename F>
inline void updateState(F fn) {
    if (xSemaphoreTake(gStateMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        fn(gState);
        xSemaphoreGive(gStateMutex);
    }
}

/**
 * @struct CalibrationConfig
 * @brief Additive calibration offsets per phase, persisted in NVS.
 *
 * These offsets are applied in the @ref Calibration::apply() method.
 * All derived power parameters (S, P, Q, PF, Wh) are recomputed
 * proportionally based on the corrected V and I values.
 */
struct CalibrationConfig {
    float voltageOffset[3];  ///< Voltage offset per phase (V). Applied as: V_cal = V_raw + offset
    float currentOffset[3];  ///< Current offset per phase (A). Applied as: I_cal = I_raw + offset
};

/**
 * @struct ThresholdConfig
 * @brief Line status classification thresholds, persisted in NVS.
 *
 * These thresholds are used by DataAcquisition::_computeStatus() to
 * classify each phase as OK, UNDER, OVER, or LOST. They are configurable
 * via the web calibration page.
 */
struct ThresholdConfig {
    float voltageLost;    ///< Voltage below this value → LOST status (V)
    float voltageUnder;   ///< Voltage below this value (but > lost) → UNDER status (V)
    float voltageOver;    ///< Voltage above this value → OVER status (V)
    float unbalanceMax;   ///< Unbalance above this triggers OLED [!] warning (%)
};
