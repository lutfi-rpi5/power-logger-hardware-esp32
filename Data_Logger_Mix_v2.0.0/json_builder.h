#ifndef JSON_BUILDER_H
#define JSON_BUILDER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include "types.h"

/**
 * @file json_builder.h
 * @brief MQTT telemetry JSON payload builder.
 *
 * Constructs a structured JSON payload matching the target schema:
 * @code
 * {
 *   "seq": 1247,
 *   "device": { "id": "...", "fw": "...", "uptime": ..., "heap": ..., "rssi": ... },
 *   "phases": {
 *     "R": { "valid": true, "v": 220.1, "i": 12.34, ... },
 *     "S": { ... },
 *     "T": { ... },
 *     "unbalance": 4.25
 *   },
 *   "ts": 1717001234
 * }
 * @endcode
 *
 * Numeric values are sent as raw numbers (not strings) with controlled
 * decimal precision via roundf(). Invalid phases still send the full
 * schema with zero values and "valid": false.
 *
 * @note Requires ArduinoJson v6.21+.
 */

/**
 * @class JSONBuilder
 * @brief Builds a serialised JSON string from SystemState.
 *
 * Stateless — the build() method can be called from any context.
 * The estimateSize() helper provides the StaticJsonDocument size
 * required for the internal allocation.
 */
class JSONBuilder {
public:
    /**
     * @brief Build a complete MQTT telemetry JSON payload.
     *
     * @param state Current SystemState snapshot to serialise.
     * @param seq   Monotonically increasing sequence number for gap detection.
     * @param ts    Timestamp value (currently millis(), future: Unix epoch).
     * @return String containing the serialised JSON payload.
     */
    String build(const SystemState& state, uint32_t seq, unsigned long ts);

    /**
     * @brief Get the estimated StaticJsonDocument size in bytes.
     * @return 1024 (sufficient for the full payload with three phases).
     */
    static size_t estimateSize();
};

#endif
