#pragma once
#include <Arduino.h>
#include "types.h"

class Calibration {
public:
    Calibration();
    void setOffsets(const CalibrationConfig& data);
    CalibrationConfig getOffsets() const;

    void apply(float rawV, float rawI, float rawP, float rawE,
               float rawF, float rawPF, int phaseIndex, PhaseReading& out) const;

    static float computeUnbalance(const PhaseReading phases[3]);

private:
    CalibrationConfig _offsets;
};
