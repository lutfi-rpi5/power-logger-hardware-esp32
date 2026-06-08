#include "json_builder.h"
#include "config.h"
#include <math.h>

/**
 * @file json_builder.cpp
 * @brief MQTT telemetry JSON payload construction.
 *
 * Builds a structured JSON string conforming to the v2.x schema:
 * @code
 * {
 *   "seq": 1247,
 *   "device": { "id":"3ph-logger-001", "fw":"v2.1.0", ... },
 *   "phases": {
 *     "R": { "valid":true, "v":220.1, "i":12.34, ... },
 *     "S": { ... },
 *     "T": { ... },
 *     "unbalance": 2.50
 *   },
 *   "ts": 1717001234
 * }
 * @endcode
 *
 * @note All numeric values are rounded to match the schema precision
 *       using roundf(value * 10^N) / 10^N.
 * @note Uses StaticJsonDocument<1024> — sufficient for three phases
 *       with full device info. If the schema is extended, increase
 *       JSON_DOC_SIZE accordingly.
 */

/// Maximum JSON document size in bytes. Sufficient for the full telemetry
/// payload including all three phases, device info, and metadata.
static const size_t JSON_DOC_SIZE = 1024;

/**
 * @brief Build a complete MQTT telemetry JSON payload.
 *
 * @param state Current SystemState snapshot.
 * @param seq   Monotonic sequence number for gap detection.
 * @param ts    Timestamp (currently millis(), future: NTP Unix epoch).
 * @return String containing the serialised JSON payload.
 *
 * @note Invalid phases (valid == false) still send all fields with
 *       zero values and status "LOST" to maintain a consistent schema
 *       on the backend.
 */
String JSONBuilder::build(const SystemState& state, uint32_t seq, unsigned long ts) {
    StaticJsonDocument<JSON_DOC_SIZE> doc;

    // ── Sequence number (monotonic, for gap detection) ────────────────
    doc["seq"] = seq;

    // ── Device info block ─────────────────────────────────────────────
    JsonObject device = doc.createNestedObject("device");
    device["id"]     = DEVICE_ID;
    device["fw"]     = FW_VERSION;
    device["uptime"] = state.uptimeSeconds;
    device["heap"]   = state.freeHeapBytes;
    device["rssi"]   = state.wifiRSSI;

    // ── Per-phase data ────────────────────────────────────────────────
    JsonObject phases = doc.createNestedObject("phases");
    const char* labels[3] = {"R", "S", "T"};
    for (int i = 0; i < 3; i++) {
        JsonObject p = phases.createNestedObject(labels[i]);
        const PhaseReading& r = state.phases[i];

        // Round each field to the schema-defined decimal precision:
        //   V:  1 decimal  (round to 10×)
        //   I:  2 decimals (round to 100×)
        //   P:  1 decimal
        //   S:  1 decimal
        //   Q:  1 decimal
        //   PF: 2 decimals
        //   F:  1 decimal
        //   E:  1 decimal
        p["valid"]   = r.valid;
        p["v"]       = roundf(r.voltage       * 10.0f) / 10.0f;
        p["i"]       = roundf(r.current       * 100.0f) / 100.0f;
        p["p"]       = roundf(r.activePower   * 10.0f) / 10.0f;
        p["s"]       = roundf(r.apparentPower * 10.0f) / 10.0f;
        p["q"]       = roundf(r.reactivePower * 10.0f) / 10.0f;
        p["pf"]      = roundf(r.powerFactor   * 100.0f) / 100.0f;
        p["f"]       = roundf(r.frequency     * 10.0f) / 10.0f;
        p["e"]       = roundf(r.energyWh      * 10.0f) / 10.0f;

        // Line status as human-readable string
        switch (r.status) {
            case LineStatus::OK:    p["status"] = "OK";    break;
            case LineStatus::UNDER: p["status"] = "UNDER"; break;
            case LineStatus::OVER:  p["status"] = "OVER";  break;
            default:                p["status"] = "LOST";  break;
        }
    }

    // ── Voltage unbalance (aggregate of all three phases) ─────────────
    // Placed inside the "phases" object per the target schema.
    phases["unbalance"] = roundf(state.unbalance * 100.0f) / 100.0f;

    // ── Timestamp ─────────────────────────────────────────────────────
    // Currently uses millis() as a placeholder. Will switch to
    // taskMgr.getUnixTime() once NTP sync is more reliable.
    doc["ts"] = ts;

    String output;
    serializeJson(doc, output);
    return output;
}

/**
 * @brief Return the estimated document size for StaticJsonDocument.
 * @return 1024 bytes — sufficient for the full telemetry payload.
 */
size_t JSONBuilder::estimateSize() {
    return JSON_DOC_SIZE;
}
