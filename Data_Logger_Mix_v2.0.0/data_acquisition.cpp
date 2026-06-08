#include "data_acquisition.h"
#include "diagnostics.h"
#include <math.h>
#include "esp_task_wdt.h"

/**
 * @file data_acquisition.cpp
 * @brief PZEM-004T sensor data acquisition on Core 0 FreeRTOS task.
 *
 * This file implements the hardware interface to three PZEM-004T v3.0
 * energy monitors connected over UART (Modbus RTU). The acquisition
 * runs at 1 Hz on Core 0 to ensure deterministic timing independent
 * of Core 1's WiFi/MQTT/display workload.
 *
 * Thread safety model:
 * - Core 0 (this task): Reads sensors → applies calibration →
 *   writes PhaseReading[3] to gState via updateState().
 * - Core 1 (loop): Reads gState snapshots via getStateCopy().
 * - Mutex collisions are rare at 1 Hz update rate.
 *
 * @note In FAKE_DATA_ENABLED mode, sensors are not accessed and
 *       synthetic data is generated instead.
 */

/**
 * @brief Construct the data acquisition module.
 * @param pzems   Pointer to an array of 3 initialised PZEM004Tv30 objects.
 * @param storage Reference to StorageManager for config initialisation.
 */
DataAcquisition::DataAcquisition(PZEM004Tv30* pzems, StorageManager& storage)
    : _pzems(pzems), _storage(storage)
{
    // Load initial calibration offsets and thresholds from NVS
    _cal        = _storage.getCalibration();
    _thresholds = _storage.getThresholds();
    _calibrator.setOffsets(_cal);
}

/**
 * @brief Start the PZEM reader task on Core 0 (PRO_CPU).
 *
 * Creates a FreeRTOS task with:
 * - Stack: TASK_STACK_PZEM (4096 words)
 * - Priority: TASK_PRIO_PZEM (3, higher than default)
 * - Core: CORE_SENSOR (0)
 *
 * The task registers itself with the Task Watchdog Timer (WDT)
 * and runs readOnce() at PZEM_READ_INTERVAL_MS intervals.
 */
void DataAcquisition::begin() {
    xTaskCreatePinnedToCore(
        _taskFn, "PZEMReader", TASK_STACK_PZEM,
        this, TASK_PRIO_PZEM, nullptr, CORE_SENSOR
    );
    diag.info("DAQ", "Reader task started on Core 0");
}

/**
 * @brief Reload calibration offsets from NVS.
 * Called by the webserver callback after the user saves new calibration values.
 */
void DataAcquisition::reloadCalibration() {
    _cal = _storage.getCalibration();
    _calibrator.setOffsets(_cal);
    diag.info("DAQ", "Calibration reloaded");
}

/**
 * @brief Reload line status thresholds from NVS and update gState.
 * Called by the webserver callback after thresholds are saved.
 */
void DataAcquisition::reloadThresholds() {
    _thresholds = _storage.getThresholds();
    float unbalMax = _thresholds.unbalanceMax;
    // Update the shared state so the OLED can display the [!] warning at
    // the correct threshold immediately without waiting for next DAQ cycle.
    updateState([unbalMax](SystemState& s) {
        s.thresholdUnbalanceMax = unbalMax;
    });
    diag.info("DAQ", "Thresholds reloaded: LOST=%.1f UNDER=%.1f OVER=%.1f UNBAL=%.2f%%",
        _thresholds.voltageLost, _thresholds.voltageUnder,
        _thresholds.voltageOver, _thresholds.unbalanceMax);
}

/**
 * @brief FreeRTOS task entry point (static).
 *
 * Task lifecycle:
 * 1. Register with WDT.
 * 2. Loop forever:
 *    a. Feed WDT.
 *    b. Read sensors (or generate fake data).
 *    c. Compute voltage unbalance.
 *    d. Update gState (uptime, heap, unbalance).
 *    e. Sleep until next 1 Hz tick.
 *
 * @param param Pointer to DataAcquisition instance (this).
 */
void DataAcquisition::_taskFn(void* param) {
    DataAcquisition* self = static_cast<DataAcquisition*>(param);

    // Register this task with the WDT so a stall triggers a reboot
    esp_task_wdt_add(xTaskGetCurrentTaskHandle());

    TickType_t lastWake = xTaskGetTickCount();

    while (true) {
        esp_task_wdt_reset();       // Feed the watchdog

        self->readOnce();           // Read all 3 PZEM or generate fake data

        // ── Compute unbalance from the just-updated state ────────────
        SystemState snap = getStateCopy();
        float vR = snap.phases[0].valid ? snap.phases[0].voltage : 0.0f;
        float vS = snap.phases[1].valid ? snap.phases[1].voltage : 0.0f;
        float vT = snap.phases[2].valid ? snap.phases[2].voltage : 0.0f;
        float unbal = _computeUnbalance(vR, vS, vT,
                                         snap.phases[0].valid,
                                         snap.phases[1].valid,
                                         snap.phases[2].valid);
        float unbalMax = self->_thresholds.unbalanceMax;

        // ── Update shared state with aggregate data ─────────────────
        updateState([&](SystemState& s) {
            s.unbalance             = unbal;
            s.thresholdUnbalanceMax = unbalMax;
            s.uptimeSeconds         = millis() / 1000;
            s.freeHeapBytes         = ESP.getFreeHeap();
        });

        // Sleep until the next 1 Hz interval (uses vTaskDelayUntil for
        // precise period timing, compensating for execution time).
        vTaskDelayUntil(&lastWake, pdMS_TO_TICKS(PZEM_READ_INTERVAL_MS));
    }
}

/**
 * @brief Execute one complete sensor read cycle.
 *
 * In FAKE_DATA_ENABLED mode, synthetic readings are generated at
 * realistic ranges. In production mode, each PZEM sensor is queried
 * via Modbus RTU for V, I, P, E, PF, F.
 *
 * Apparent power (S) is computed as V × I (not read from PZEM directly,
 * as the PZEM-004T v3.0 does not provide S in its register map).
 */
void DataAcquisition::readOnce() {
#if FAKE_DATA_ENABLED
    generateFakeData();
    for (int i = 0; i < NUM_PHASES; i++) {
        _processReading(i, _fakeV[i], _fakeI[i], _fakeP[i], _fakeS[i], _fakePF[i], _fakeF[i], _fakeE[i]);
    }
#else
    for (int i = 0; i < NUM_PHASES; i++) {
        // Read raw values from PZEM via Modbus RTU.
        // These calls are blocking (~50–100 ms per read per sensor).
        // Total cycle time for 3 sensors: ~150–300 ms.
        float v  = _pzems[i].voltage();
        float a  = _pzems[i].current();
        float p  = _pzems[i].power();
        float e  = _pzems[i].energy();
        float pf = _pzems[i].pf();
        float f  = _pzems[i].frequency();
        float va = (isnan(v) || isnan(a)) ? NAN : v * a;

        _processReading(i, v, a, p, va, pf, f, e);
    }
#endif
}

/**
 * @brief Generate synthetic PZEM readings for testing without hardware.
 *
 * Data ranges (typical household/light industrial):
 * - Voltage: 200.0 – 203.99 V (slightly low, to trigger UNDER testing)
 * - Current: 10.00 – 14.99 A
 * - Active power: V × I × 0.85 (PF ≈ 0.85)
 * - Apparent power: V × I
 * - Energy: 10000 – 10999 Wh (cumulative, simulates run-time accumulation)
 * - Frequency: 49.90 – 50.09 Hz (grid-frequency range)
 * - Power factor: 0.80 – 0.99 (inductive load typical)
 */
void DataAcquisition::generateFakeData() {
    for (int i = 0; i < NUM_PHASES; i++) {
        _fakeV[i]  = 200.0f + (float)random(0, 400) / 100.0f;
        _fakeI[i]  = 10.0f + (float)random(0, 500) / 100.0f;
        _fakeP[i]  = _fakeV[i] * _fakeI[i] * 0.85f;
        _fakeS[i]  = _fakeV[i] * _fakeI[i];
        _fakeE[i]  = 10000.0f + (float)random(0, 1000);
        _fakeF[i]  = 49.9f + (float)random(0, 20) / 100.0f;
        _fakePF[i] = 0.80f + (float)random(0, 200) / 1000.0f;
    }
    diag.info("DAQ", "Fake data generated for all phases");
}

/**
 * @brief Process a single phase reading — calibrate and persist to gState.
 *
 * For each phase:
 * 1. Check if V or I is NaN (sensor communication failure) → LOST.
 * 2. Apply Calibration::apply() for offset + full power triangle recompute.
 * 3. Compute line status from calibrated voltage and configured thresholds.
 * 4. Write the complete PhaseReading to gState via updateState().
 *
 * @param idx Phase index (0=R, 1=S, 2=T).
 * @param v   Raw voltage (V)
 * @param i   Raw current (A)
 * @param p   Raw active power (W)
 * @param s   Raw apparent power (VA) — pre-computed as V × I
 * @param pf  Raw power factor
 * @param f   Raw frequency (Hz)
 * @param e   Raw cumulative energy (Wh)
 */
void DataAcquisition::_processReading(int idx,
    float v, float i, float p, float s, float pf, float f, float e)
{
    PhaseReading r;

    if (isnan(v) || isnan(i)) {
        // ── Sensor communication failure ─────────────────────────────
        // Preserve the last known energy value instead of resetting to 0,
        // preventing a single glitch from resetting accumulated totals.
        r.valid  = false;
        r.status = LineStatus::LOST;
        SystemState prev = getStateCopy();
        r.energyWh = prev.phases[idx].energyWh;
    } else {
        // ── Valid reading: apply calibration + recompute ─────────────
        _calibrator.apply(v, i, p, e, f, pf, idx, r);

        // Classify line status based on calibrated voltage
        r.status = _computeStatus(r.voltage);
    }

    r.timestampMs = millis();

    // Write to shared state (mutex-protected)
    updateState([&](SystemState& st) {
        st.phases[idx] = r;
    });
}

/**
 * @brief Classify phase status based on calibrated voltage.
 *
 * Threshold hierarchy (defined in config.h / NVS):
 * ```
 * V < voltageLost (80 V)     → LOST  — phase dead or sensor missing
 * V < voltageUnder (180 V)   → UNDER — brown-out condition
 * V > voltageOver (240 V)    → OVER  — over-voltage
 * else                       → OK    — normal operating range
 * ```
 *
 * @param voltage Calibrated phase voltage (V).
 * @return LineStatus classification.
 */
LineStatus DataAcquisition::_computeStatus(float voltage) const {
    if (voltage < _thresholds.voltageLost)  return LineStatus::LOST;
    if (voltage < _thresholds.voltageUnder) return LineStatus::UNDER;
    if (voltage > _thresholds.voltageOver)  return LineStatus::OVER;
    return LineStatus::OK;
}

/**
 * @brief Compute voltage unbalance using the NEMA MG-1 method.
 *
 * Standard: NEMA MG-1 Section 14.36 / IEEE Std 141.
 * Used by SPLN (Indonesia) for three-phase supply quality.
 *
 * Note: If any phase is marked invalid, returns 0.0 to avoid
 * misleading unbalance calculations from missing data.
 *
 * @param vR Voltage phase R (V)
 * @param vS Voltage phase S (V)
 * @param vT Voltage phase T (V)
 * @param validR Phase R validity (must be true for all three)
 * @param validS Phase S validity
 * @param validT Phase T validity
 * @return Unbalance percentage, or 0.0 if any phase is invalid.
 */
float DataAcquisition::_computeUnbalance(float vR, float vS, float vT,
                                          bool validR, bool validS, bool validT)
{
    // Require all phases valid for a meaningful unbalance calculation
    if (!validR || !validS || !validT) return 0.0f;

    float vAvg = (vR + vS + vT) / 3.0f;
    // Guard against division by zero (all phases at 0 V)
    if (vAvg < 1.0f) return 0.0f;

    float devR = fabsf(vR - vAvg);
    float devS = fabsf(vS - vAvg);
    float devT = fabsf(vT - vAvg);
    float maxDev = devR;
    if (devS > maxDev) maxDev = devS;
    if (devT > maxDev) maxDev = devT;

    return (maxDev / vAvg) * 100.0f;
}

/**
 * @brief Reset cumulative energy counters on all three PZEM modules.
 *
 * Sends the Modbus reset command to each PZEM. A 200 ms delay is
 * required between resets per the PZEM-004T v3.0 datasheet to allow
 * the EEPROM write cycle to complete.
 *
 * @return true if all three resets were acknowledged.
 */
bool DataAcquisition::resetAllEnergy() {
    diag.info("DAQ", "Resetting energy on all PZEMs...");
    bool allOk = true;
    for (int i = 0; i < NUM_PHASES; i++) {
        bool ok = _pzems[i].resetEnergy();
        diag.info("DAQ", "  PZEM[%d] reset: %s", i, ok ? "OK" : "FAIL");
        if (!ok) allOk = false;
        delay(200);  // PZEM-004T v3.0 EEPROM write time per datasheet
    }
    return allOk;
}
