#pragma once
#include <Arduino.h>
#include <PZEM004Tv30.h>
#include "config.h"
#include "types.h"
#include "storage_manager.h"
#include "calibration.h"

/**
 * @file data_acquisition.h
 * @brief PZEM-004T sensor data acquisition running as a dedicated FreeRTOS task on Core 0.
 *
 * The DataAcquisition class owns the PZEM reader FreeRTOS task ("PZEMReader")
 * which runs at 1 Hz on Core 0 (PRO_CPU) with priority 3. It:
 * 1. Reads raw voltage, current, power, energy, power factor, and frequency
 *    from all three PZEM-004T sensors via Modbus RTU.
 * 2. Applies calibration offsets (V/I additive model) via the Calibration class.
 * 3. Computes line status (OK/UNDER/OVER/LOST) based on calibrated voltage.
 * 4. Computes voltage unbalance percentage (NEMA method).
 * 5. Writes the complete state snapshot to gState under mutex protection.
 *
 * In FAKE_DATA_ENABLED mode, synthetic data is generated instead of reading
 * real sensors, for testing without hardware.
 */

/**
 * @class DataAcquisition
 * @brief Orchestrates PZEM sensor reading, calibration, and state updates.
 *
 * The begin() method starts a FreeRTOS task pinned to Core 0. All PZEM
 * communication and calibration happens in that task context.
 */
class DataAcquisition {
public:
    /**
     * @brief Construct the data acquisition module.
     * @param pzems   Pointer to an array of 3 PZEM004Tv30 objects.
     * @param storage Reference to StorageManager for calibration/threshold load.
     */
    DataAcquisition(PZEM004Tv30* pzems, StorageManager& storage);

    /**
     * @brief Start the PZEM reader task on Core 0.
     * Creates a FreeRTOS task pinned to CORE_SENSOR (Core 0).
     * Non-blocking — returns immediately after task creation.
     */
    void begin();

    /**
     * @brief Reload calibration offsets from NVS and apply them to the calibrator.
     * Called by the webserver callback after calibration save.
     */
    void reloadCalibration();

    /**
     * @brief Reload line status thresholds from NVS and update gState.
     * Called by the webserver callback after threshold save.
     */
    void reloadThresholds();

    /**
     * @brief Execute one complete PZEM read cycle.
     *
     * In FAKE_DATA_ENABLED mode, generates synthetic readings.
     * Otherwise, reads all three PZEM sensors via Modbus.
     * Results are written to gState via updateState().
     */
    void readOnce();

    /**
     * @brief Reset cumulative energy counters on all three PZEM modules.
     * @return true if all three resets succeeded.
     * @note Contains delay(200) between resets per PZEM datasheet requirements.
     */
    bool resetAllEnergy();

private:
    PZEM004Tv30*      _pzems;       ///< Array of 3 PZEM004Tv30 objects
    StorageManager&   _storage;     ///< NVS storage for config
    CalibrationConfig _cal;         ///< Cached calibration offsets
    ThresholdConfig   _thresholds;  ///< Cached line status thresholds
    Calibration       _calibrator;  ///< Calibration engine for V/I offset + recompute

    /// @name Fake Data State (only used when FAKE_DATA_ENABLED=true)
    /// @{
    void generateFakeData();
    float _fakeV[3], _fakeI[3], _fakeP[3], _fakeS[3];
    float _fakeE[3], _fakeF[3], _fakePF[3];
    /// @}

    /**
     * @brief FreeRTOS task entry point for the PZEM reader.
     * Runs readOnce() at PZEM_READ_INTERVAL_MS intervals (1 Hz).
     * Updates gState with unbalance, uptime, and heap info.
     * Feeds the watchdog timer each cycle.
     *
     * @param param Pointer to the DataAcquisition instance (this).
     */
    static void _taskFn(void* param);

    /**
     * @brief Classify a phase based on calibrated voltage.
     *
     * Threshold hierarchy:
     * - voltage < voltageLost  → LOST
     * - voltage < voltageUnder → UNDER
     * - voltage > voltageOver  → OVER
     * - otherwise              → OK
     *
     * @param voltage Calibrated phase voltage (V).
     * @return LineStatus classification.
     */
    LineStatus _computeStatus(float voltage) const;

    /**
     * @brief Compute voltage unbalance using the NEMA method.
     *
     * NEMA MG-1:
     * V_avg = (V_R + V_S + V_T) / 3
     * max_dev = max(|V_R - V_avg|, |V_S - V_avg|, |V_T - V_avg|)
     * unbalance = (max_dev / V_avg) × 100
     *
     * Phases with 0 V (no load or sensor connected) are included in the
     * calculation. Only returns 0.0 if all three voltages are below 1 V
     * (no meaningful data on any phase).
     *
     * @param vR Voltage phase R (V)
     * @param vS Voltage phase S (V)
     * @param vT Voltage phase T (V)
     * @return Unbalance percentage (0.0–200.0).
     */
    static float _computeUnbalance(float vR, float vS, float vT);

    /**
     * @brief Process a single phase reading: calibrate and write to gState.
     *
     * If V or I is NaN, the phase is marked LOST and the previous energy
     * value is preserved. Otherwise, Calibration::apply() performs the
     * full offset + recompute pipeline.
     *
     * @param idx Phase index (0=R, 1=S, 2=T).
     * @param v   Raw voltage (V)
     * @param i   Raw current (A)
     * @param p   Raw active power (W)
     * @param s   Raw apparent power (VA) — computed as V × I
     * @param pf  Raw power factor
     * @param f   Raw frequency (Hz)
     * @param e   Raw cumulative energy (Wh)
     */
    void _processReading(int idx, float v, float i, float p, float s, float pf, float f, float e);
};
