/******************************************************
 * File      : DataAcquisition.cpp
 ******************************************************/

#include "DataAcquisition.h"
#include <math.h>
#include "esp_task_wdt.h"

DataAcquisition::DataAcquisition(PZEM004Tv30* pzems, StorageManager& storage)
    : _pzems(pzems), _storage(storage)
{
    _cal        = _storage.getCalibration();
    _thresholds = _storage.getThresholds();
}

void DataAcquisition::begin() {
    xTaskCreatePinnedToCore(
        _taskFn,
        "PZEMReader",
        TASK_STACK_PZEM,
        this,
        TASK_PRIO_PZEM,
        nullptr,
        CORE_SENSOR
    );
    Serial.println("[DAQ] Reader task started on Core 0");
}

void DataAcquisition::reloadCalibration() {
    _cal = _storage.getCalibration();
    Serial.println("[DAQ] Calibration reloaded");
}

void DataAcquisition::reloadThresholds() {
    _thresholds = _storage.getThresholds();
    // Propagate runtime threshold to gState so DisplayOLED can read it
    float unbalMax = _thresholds.unbalanceMax;
    updateState([unbalMax](SystemState& s) {
        s.thresholdUnbalanceMax = unbalMax;
    });
    Serial.printf("[DAQ] Thresholds reloaded: LOST=%.1f UNDER=%.1f OVER=%.1f UNBAL=%.2f%%\n",
        _thresholds.voltageLost, _thresholds.voltageUnder,
        _thresholds.voltageOver, _thresholds.unbalanceMax);
}

// ─── FreeRTOS Task Entry ──────────────────────────────────

void DataAcquisition::_taskFn(void* param) {
    DataAcquisition* self = static_cast<DataAcquisition*>(param);

    // Register this task with the task watchdog
    esp_task_wdt_add(xTaskGetCurrentTaskHandle());  // IDF5: eksplisit handle task ini

    TickType_t lastWake = xTaskGetTickCount();

    while (true) {
        // Feed watchdog
        esp_task_wdt_reset();

        self->readOnce();

        // Compute 3-phase unbalance from latest readings
        {
            SystemState snap = getStateCopy();
            float vR = snap.phases[0].valid ? snap.phases[0].voltage : 0.0f;
            float vS = snap.phases[1].valid ? snap.phases[1].voltage : 0.0f;
            float vT = snap.phases[2].valid ? snap.phases[2].voltage : 0.0f;
            float unbal = _computeUnbalance(vR, vS, vT,
                                             snap.phases[0].valid,
                                             snap.phases[1].valid,
                                             snap.phases[2].valid);
            float unbalMax = self->_thresholds.unbalanceMax;

            updateState([&](SystemState& s) {
                s.unbalance             = unbal;
                s.thresholdUnbalanceMax = unbalMax;
                s.uptimeSeconds         = millis() / 1000;
                s.freeHeapBytes         = ESP.getFreeHeap();
            });
        }

        // Precise periodic delay (accounts for execution time)
        vTaskDelayUntil(&lastWake, pdMS_TO_TICKS(PZEM_READ_INTERVAL_MS));
    }
}

// ─── Single Read Cycle ────────────────────────────────────

void DataAcquisition::readOnce() {
    for (int i = 0; i < NUM_PHASES; i++) {
        float v  = _pzems[i].voltage();
        float a  = _pzems[i].current();
        float p  = _pzems[i].power();
        float e  = _pzems[i].energy();   // Wh
        float pf = _pzems[i].pf();
        float f  = _pzems[i].frequency();
        float va = (isnan(v) || isnan(a)) ? NAN : v * a;  // apparent power

        _processReading(i, v, a, p, va, pf, f, e);
    }
}

// ─── Process & Write One Phase ────────────────────────────

void DataAcquisition::_processReading(int idx,
    float v, float i, float p, float s, float pf, float f, float e)
{
    PhaseReading r;

    // Check validity (PZEM returns NaN on read failure)
    if (isnan(v) || isnan(i)) {
        r.valid  = false;
        r.status = LineStatus::LOST;
        // Keep last energy value (don't zero it on transient failure)
        SystemState prev = getStateCopy();
        r.energyWh = prev.phases[idx].energyWh;
    } else {
        // Apply calibration offsets
        r.voltage  = v + _cal.voltageOffset[idx];
        r.current  = i + _cal.currentOffset[idx];

        // Clamp negatives to zero (offset might underflow at low readings)
        if (r.voltage < 0) r.voltage = 0;
        if (r.current < 0) r.current = 0;

        r.activePower   = isnan(p)  ? 0 : p;
        r.apparentPower = isnan(s)  ? 0 : s;
        r.powerFactor   = isnan(pf) ? 0 : pf;
        r.frequency     = isnan(f)  ? 0 : f;
        r.energyWh      = isnan(e)  ? 0 : e;

        // Reactive power: Q = sqrt(S² - P²)
        float s2 = r.apparentPower * r.apparentPower;
        float p2 = r.activePower   * r.activePower;
        r.reactivePower = (s2 >= p2) ? sqrtf(s2 - p2) : 0.0f;

        r.status = _computeStatus(r.voltage);
        r.valid  = true;
    }

    r.timestampMs = millis();

    // Write into shared state
    updateState([&](SystemState& st) {
        st.phases[idx] = r;
    });
}

// ─── Status Threshold (reads from EEPROM-loaded _thresholds) ──────────────────

LineStatus DataAcquisition::_computeStatus(float voltage) const {
    if (voltage < _thresholds.voltageLost)  return LineStatus::LOST;
    if (voltage < _thresholds.voltageUnder) return LineStatus::UNDER;
    if (voltage > _thresholds.voltageOver)  return LineStatus::OVER;
    return LineStatus::OK;
}

// ─── Voltage Unbalance (NEMA method) ─────────────────────
//
// Algorithm:
//   VAVG = (VR + VS + VT) / 3
//   MAX_DEV = max(|VR-VAVG|, |VS-VAVG|, |VT-VAVG|)
//   UNBALANCE (%) = (MAX_DEV / VAVG) × 100
//
// Returns 0 if any phase is invalid or VAVG == 0.

float DataAcquisition::_computeUnbalance(float vR, float vS, float vT,
                                          bool validR, bool validS, bool validT)
{
    // All three phases must be valid for a meaningful result
    if (!validR || !validS || !validT) return 0.0f;

    float vAvg = (vR + vS + vT) / 3.0f;
    if (vAvg < 1.0f) return 0.0f;  // avoid divide-by-zero at near-zero voltages

    float devR = fabsf(vR - vAvg);
    float devS = fabsf(vS - vAvg);
    float devT = fabsf(vT - vAvg);

    float maxDev = devR;
    if (devS > maxDev) maxDev = devS;
    if (devT > maxDev) maxDev = devT;

    return (maxDev / vAvg) * 100.0f;
}

// ─── Energy Reset ─────────────────────────────────────────

bool DataAcquisition::resetAllEnergy() {
    Serial.println("[DAQ] Resetting energy on all PZEMs...");
    bool allOk = true;
    for (int i = 0; i < NUM_PHASES; i++) {
        bool ok = _pzems[i].resetEnergy();
        Serial.printf("[DAQ]   PZEM[%d] reset: %s\n", i, ok ? "OK" : "FAIL");
        if (!ok) allOk = false;
        delay(200);  // inter-device pause for UART bus
    }
    return allOk;
}
