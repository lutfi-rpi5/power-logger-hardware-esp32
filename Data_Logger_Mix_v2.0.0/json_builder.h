#ifndef JSON_BUILDER_H
#define JSON_BUILDER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include "types.h"

// ============================================================
// MQTT JSON payload builder
// ============================================================

class JSONBuilder {
public:
    String build(const SystemState& state, uint32_t seq, unsigned long ts);
    static size_t estimateSize();
};

#endif
