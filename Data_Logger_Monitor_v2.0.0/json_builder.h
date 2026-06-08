#ifndef JSON_BUILDER_H
#define JSON_BUILDER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include "types.h"
#include "system_state.h"

// ============================================================
// MQTT JSON payload builder
// ============================================================
// Builds the structured JSON payload for publishing:
// {
//   "seq": 1247,
//   "device": { "id": "...", "fw": "...", "uptime": ..., "heap": ..., "rssi": ... },
//   "phases": { "R": { ... }, "S": { ... }, "T": { ... } },
//   "unbalance": ...,
//   "ts": 1717001234
// }
// All numeric values as raw numbers (not strings).
// ============================================================

class JSONBuilder {
public:
    // Build full payload. Returns empty string on failure.
    // Caller must provide a sufficiently large buffer (recommended: 1024 bytes).
    String build(const SystemState& state, uint32_t seq, unsigned long ts);

    // Estimate required buffer size
    static size_t estimateSize();
};

#endif
