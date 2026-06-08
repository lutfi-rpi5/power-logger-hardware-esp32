#include "json_builder.h"
#include "config.h"
#include <math.h>

String JSONBuilder::build(const SystemState& state, uint32_t seq, unsigned long ts) {
    StaticJsonDocument<1024> doc;

    // Sequence
    doc["seq"] = seq;

    // Device info
    JsonObject device = doc.createNestedObject("device");
    device["id"]     = DEVICE_ID;
    device["fw"]     = FW_VERSION;
    device["uptime"] = state.uptimeSeconds();
    device["heap"]   = state.freeHeap();
    device["rssi"]   = state.rssi;

    // Phases
    JsonObject phases = doc.createNestedObject("phases");
    const char* phaseLabels[3] = {"R", "S", "T"};

    for (int i = 0; i < 3; i++) {
        JsonObject p = phases.createNestedObject(phaseLabels[i]);
        const PhaseData& pd = state.phases[i];

        p["valid"]   = pd.valid;
        p["v"]       = roundf(pd.voltage * 10.0f) / 10.0f;       // 1 decimal
        p["i"]       = roundf(pd.current * 100.0f) / 100.0f;     // 2 decimals
        p["p"]       = roundf(pd.power * 10.0f) / 10.0f;         // 1 decimal
        p["s"]       = roundf(pd.apparent * 10.0f) / 10.0f;      // 1 decimal
        p["q"]       = roundf(pd.reactive * 10.0f) / 10.0f;      // 1 decimal
        p["pf"]      = roundf(pd.pf * 100.0f) / 100.0f;          // 2 decimals
        p["f"]       = roundf(pd.frequency * 10.0f) / 10.0f;     // 1 decimal
        p["e"]       = roundf(pd.energy * 10.0f) / 10.0f;        // 1 decimal (Wh)

        switch (pd.status) {
            case LineStatus::OK:    p["status"] = "OK";    break;
            case LineStatus::UNDER: p["status"] = "UNDER"; break;
            case LineStatus::OVER:  p["status"] = "OVER";  break;
            default:                p["status"] = "LOST";  break;
        }
    }

    // Unbalance (root level, aggregate of 3 phases)
    doc["unbalance"] = roundf(state.unbalancePercent * 100.0f) / 100.0f;

    // Timestamp
    doc["ts"] = ts;

    String output;
    serializeJson(doc, output);
    return output;
}

size_t JSONBuilder::estimateSize() {
    return 1024;
}
