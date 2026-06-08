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
    device["uptime"] = state.uptimeSeconds;
    device["heap"]   = state.freeHeapBytes;
    device["rssi"]   = state.wifiRSSI;

    // Phases
    JsonObject phases = doc.createNestedObject("phases");
    const char* labels[3] = {"R", "S", "T"};
    for (int i = 0; i < 3; i++) {
        JsonObject p = phases.createNestedObject(labels[i]);
        const PhaseReading& r = state.phases[i];
        p["valid"]   = r.valid;
        p["v"]       = roundf(r.voltage       * 10.0f) / 10.0f;
        p["i"]       = roundf(r.current       * 100.0f) / 100.0f;
        p["p"]       = roundf(r.activePower   * 10.0f) / 10.0f;
        p["s"]       = roundf(r.apparentPower * 10.0f) / 10.0f;
        p["q"]       = roundf(r.reactivePower * 10.0f) / 10.0f;
        p["pf"]      = roundf(r.powerFactor   * 100.0f) / 100.0f;
        p["f"]       = roundf(r.frequency     * 10.0f) / 10.0f;
        p["e"]       = roundf(r.energyWh      * 10.0f) / 10.0f;
        switch (r.status) {
            case LineStatus::OK:    p["status"] = "OK";    break;
            case LineStatus::UNDER: p["status"] = "UNDER"; break;
            case LineStatus::OVER:  p["status"] = "OVER";  break;
            default:                p["status"] = "LOST";  break;
        }
    }

    // Unbalance (inside phases object, aggregate of 3 phases)
    phases["unbalance"] = roundf(state.unbalance * 100.0f) / 100.0f;

    // Timestamp
    doc["ts"] = ts;

    String output;
    serializeJson(doc, output);
    return output;
}

size_t JSONBuilder::estimateSize() {
    return 1024;
}
