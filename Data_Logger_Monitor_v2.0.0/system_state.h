#ifndef SYSTEM_STATE_H
#define SYSTEM_STATE_H

#include "types.h"
#include <Arduino.h>

// ============================================================
// Global system state — holds runtime status for all modules
// ============================================================

struct SystemState {
    // Core state
    AppState appState;
    OLEDMode oledMode;
    uint32_t bootTime;         // millis() at boot
    uint32_t lastPublishSeq;   // last published seq number

    // Connectivity
    bool wifiConnected;
    bool mqttConnected;
    bool apActive;
    int8_t rssi;               // WiFi RSSI in dBm

    // Timing
    uint32_t uptimeSeconds() const {
        return (millis() - bootTime) / 1000;
    }

    uint32_t freeHeap() const {
        return ESP.getFreeHeap();
    }

    // Phase data
    PhaseData phases[3];
    float unbalancePercent;

    // Diagnostics
    bool fakeDataMode;         // inject random data when true
};

#endif
