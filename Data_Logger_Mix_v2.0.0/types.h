#pragma once
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

enum class LineStatus : uint8_t {
    OK    = 0,
    UNDER = 1,
    OVER  = 2,
    LOST  = 3
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

struct PhaseReading {
    float voltage       = 0.0f;
    float current       = 0.0f;
    float activePower   = 0.0f;
    float apparentPower = 0.0f;
    float reactivePower = 0.0f;
    float powerFactor   = 0.0f;
    float frequency     = 0.0f;
    float energyWh      = 0.0f;
    LineStatus status   = LineStatus::LOST;
    bool valid          = false;
    unsigned long timestampMs = 0;
};

struct SystemState {
    PhaseReading phases[3];
    float unbalance              = 0.0f;
    float thresholdUnbalanceMax  = 3.0f;

    bool   wifiConnected  = false;
    bool   mqttConnected  = false;
    int32_t wifiRSSI      = 0;

    uint32_t freeHeapBytes  = 0;
    uint32_t uptimeSeconds  = 0;

    bool   webServerActive = false;
    char   apSSID[32]      = "";
    char   apPass[32]      = "";
    char   apIP[16]        = "";
};

extern SystemState       gState;
extern SemaphoreHandle_t gStateMutex;

inline SystemState getStateCopy() {
    static SystemState cached;
    if (xSemaphoreTake(gStateMutex, pdMS_TO_TICKS(50)) == pdTRUE) {
        cached = gState;
        xSemaphoreGive(gStateMutex);
    }
    return cached;
}

template<typename F>
inline void updateState(F fn) {
    if (xSemaphoreTake(gStateMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        fn(gState);
        xSemaphoreGive(gStateMutex);
    }
}

struct CalibrationConfig {
    float voltageOffset[3];
    float currentOffset[3];
};

struct ThresholdConfig {
    float voltageLost;
    float voltageUnder;
    float voltageOver;
    float unbalanceMax;
};
