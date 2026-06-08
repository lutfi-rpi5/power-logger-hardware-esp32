#pragma once
#include <Arduino.h>
#include <PZEM004Tv30.h>
#include "config.h"
#include "types.h"
#include "storage_manager.h"
#include "calibration.h"

class DataAcquisition {
public:
    DataAcquisition(PZEM004Tv30* pzems, StorageManager& storage);
    void begin();
    void reloadCalibration();
    void reloadThresholds();
    void readOnce();
    bool resetAllEnergy();

private:
    PZEM004Tv30*      _pzems;
    StorageManager&   _storage;
    CalibrationConfig _cal;
    ThresholdConfig   _thresholds;
    Calibration       _calibrator;

    void generateFakeData();
    float _fakeV[3], _fakeI[3], _fakeP[3], _fakeS[3];
    float _fakeE[3], _fakeF[3], _fakePF[3];

    static void _taskFn(void* param);
    LineStatus _computeStatus(float voltage) const;
    static float _computeUnbalance(float vR, float vS, float vT,
                                    bool validR, bool validS, bool validT);
    void _processReading(int idx, float v, float i, float p, float s, float pf, float f, float e);
};
