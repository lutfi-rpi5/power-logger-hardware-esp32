#pragma once

/******************************************************
 * File      : DataAcquisition.h
 * Description:
 *   Runs on Core 0 as a FreeRTOS task.
 *   Reads all 3 PZEM-004T modules every PZEM_READ_INTERVAL_MS.
 *   Applies calibration offsets and computes reactive power.
 *   Computes 3-phase voltage unbalance (NEMA standard method).
 *   Writes results into gState (protected by gStateMutex).
 *
 *   This task is deliberately separated from comms tasks
 *   so sensor reads are never delayed by WiFi/MQTT activity.
 ******************************************************/

#include <Arduino.h>
#include <PZEM004Tv30.h>
#include "config.h"
#include "PhaseData.h"
#include "StorageManager.h"

class DataAcquisition {
public:
    /**
     * @param pzems     Pointer to PZEM array (3 elements)
     * @param storage   StorageManager for calibration offsets & thresholds
     */
    DataAcquisition(PZEM004Tv30* pzems, StorageManager& storage);

    /** Start the FreeRTOS reader task on Core 0. */
    void begin();

    /** Reload calibration from EEPROM (call after webserver saves new values). */
    void reloadCalibration();

    /** Reload voltage thresholds from EEPROM (call after webserver saves new values). */
    void reloadThresholds();

    /** Manual single-shot read (use for testing; task handles production reads). */
    void readOnce();

    /** Trigger energy reset on all PZEMs (blocking, ~700 ms total). */
    bool resetAllEnergy();

private:
    PZEM004Tv30*      _pzems;
    StorageManager&   _storage;
    CalibrationConfig _cal;
    ThresholdConfig   _thresholds;

    static void _taskFn(void* param);  // FreeRTOS entry

    LineStatus _computeStatus(float voltage) const;

    /**
     * Compute 3-phase voltage unbalance percentage (NEMA method):
     *   UNBALANCE = (MAX_DEV / VAVG) × 100
     * where MAX_DEV = max(|VR-VAVG|, |VS-VAVG|, |VT-VAVG|)
     * Returns 0 if any phase is invalid.
     */
    static float _computeUnbalance(float vR, float vS, float vT,
                                   bool validR, bool validS, bool validT);

    void _processReading(int idx, float v, float i, float p, float s, float pf, float f, float e);
};
