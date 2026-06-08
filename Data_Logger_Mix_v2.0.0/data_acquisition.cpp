#include "data_acquisition.h"
#include "diagnostics.h"
#include <math.h>
#include "esp_task_wdt.h"

DataAcquisition::DataAcquisition(PZEM004Tv30* pzems, StorageManager& storage)
    : _pzems(pzems), _storage(storage)
{
    _cal        = _storage.getCalibration();
    _thresholds = _storage.getThresholds();
    _calibrator.setOffsets(_cal);
}

void DataAcquisition::begin() {
    xTaskCreatePinnedToCore(
        _taskFn, "PZEMReader", TASK_STACK_PZEM,
        this, TASK_PRIO_PZEM, nullptr, CORE_SENSOR
    );
    diag.info("DAQ", "Reader task started on Core 0");
}

void DataAcquisition::reloadCalibration() {
    _cal = _storage.getCalibration();
    _calibrator.setOffsets(_cal);
    diag.info("DAQ", "Calibration reloaded");
}

void DataAcquisition::reloadThresholds() {
    _thresholds = _storage.getThresholds();
    float unbalMax = _thresholds.unbalanceMax;
    updateState([unbalMax](SystemState& s) {
        s.thresholdUnbalanceMax = unbalMax;
    });
    diag.info("DAQ", "Thresholds reloaded: LOST=%.1f UNDER=%.1f OVER=%.1f UNBAL=%.2f%%",
        _thresholds.voltageLost, _thresholds.voltageUnder,
        _thresholds.voltageOver, _thresholds.unbalanceMax);
}

void DataAcquisition::_taskFn(void* param) {
    DataAcquisition* self = static_cast<DataAcquisition*>(param);
    esp_task_wdt_add(xTaskGetCurrentTaskHandle());
    TickType_t lastWake = xTaskGetTickCount();

    while (true) {
        esp_task_wdt_reset();
        self->readOnce();

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

        vTaskDelayUntil(&lastWake, pdMS_TO_TICKS(PZEM_READ_INTERVAL_MS));
    }
}

void DataAcquisition::readOnce() {
#if FAKE_DATA_ENABLED
    generateFakeData();
    for (int i = 0; i < NUM_PHASES; i++) {
        _processReading(i, _fakeV[i], _fakeI[i], _fakeP[i], _fakeS[i], _fakePF[i], _fakeF[i], _fakeE[i]);
    }
#else
    for (int i = 0; i < NUM_PHASES; i++) {
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

void DataAcquisition::_processReading(int idx,
    float v, float i, float p, float s, float pf, float f, float e)
{
    PhaseReading r;

    if (isnan(v) || isnan(i)) {
        r.valid  = false;
        r.status = LineStatus::LOST;
        SystemState prev = getStateCopy();
        r.energyWh = prev.phases[idx].energyWh;
    } else {
        // Gunakan calibration class untuk apply offset + recompute derived params
        _calibrator.apply(v, i, p, e, f, pf, idx, r);

        // Status threshold ditentukan setelah kalibrasi
        r.status = _computeStatus(r.voltage);
    }

    r.timestampMs = millis();

    updateState([&](SystemState& st) {
        st.phases[idx] = r;
    });
}

LineStatus DataAcquisition::_computeStatus(float voltage) const {
    if (voltage < _thresholds.voltageLost)  return LineStatus::LOST;
    if (voltage < _thresholds.voltageUnder) return LineStatus::UNDER;
    if (voltage > _thresholds.voltageOver)  return LineStatus::OVER;
    return LineStatus::OK;
}

float DataAcquisition::_computeUnbalance(float vR, float vS, float vT,
                                          bool validR, bool validS, bool validT)
{
    if (!validR || !validS || !validT) return 0.0f;
    float vAvg = (vR + vS + vT) / 3.0f;
    if (vAvg < 1.0f) return 0.0f;
    float devR = fabsf(vR - vAvg);
    float devS = fabsf(vS - vAvg);
    float devT = fabsf(vT - vAvg);
    float maxDev = devR;
    if (devS > maxDev) maxDev = devS;
    if (devT > maxDev) maxDev = devT;
    return (maxDev / vAvg) * 100.0f;
}

bool DataAcquisition::resetAllEnergy() {
    diag.info("DAQ", "Resetting energy on all PZEMs...");
    bool allOk = true;
    for (int i = 0; i < NUM_PHASES; i++) {
        bool ok = _pzems[i].resetEnergy();
        diag.info("DAQ", "  PZEM[%d] reset: %s", i, ok ? "OK" : "FAIL");
        if (!ok) allOk = false;
        delay(200);
    }
    return allOk;
}
