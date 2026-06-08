#include "calibration.h"
#include <math.h>

Calibration::Calibration() {
    for (int i = 0; i < 3; i++) {
        _offsets.voltageOffset[i] = 0.0f;
        _offsets.currentOffset[i] = 0.0f;
    }
}

void Calibration::setOffsets(const CalibrationConfig& data) { _offsets = data; }
CalibrationConfig Calibration::getOffsets() const { return _offsets; }

void Calibration::apply(float rawV, float rawI, float rawP, float rawE,
                         float rawF, float rawPF, int phaseIndex,
                         PhaseReading& out) const
{
    float vCal = rawV + _offsets.voltageOffset[phaseIndex];
    float iCal = rawI + _offsets.currentOffset[phaseIndex];
    if (vCal < 0.0f) vCal = 0.0f;
    if (iCal < 0.0f) iCal = 0.0f;

    float apparent  = vCal * iCal;
    float power     = rawP;

    if (isnan(rawV) || rawV <= 0.0f || isnan(rawI)) {
        out.valid       = false;
        out.voltage     = 0.0f;
        out.current     = 0.0f;
        out.activePower = 0.0f;
        out.apparentPower = 0.0f;
        out.reactivePower = 0.0f;
        out.powerFactor = 0.0f;
        out.frequency   = rawF;
        out.energyWh    = rawE;
        out.status      = LineStatus::LOST;
        out.timestampMs = millis();
        return;
    }

    float pfCal = (apparent > 0.0f) ? (power / apparent) : 0.0f;
    if (pfCal > 1.0f) pfCal = 1.0f;
    if (pfCal < 0.0f) pfCal = 0.0f;

    float reactive = 0.0f;
    float s2 = apparent * apparent;
    float p2 = power * power;
    if (s2 > p2) reactive = sqrtf(s2 - p2);

    out.valid        = true;
    out.voltage      = vCal;
    out.current      = iCal;
    out.activePower  = power;
    out.apparentPower = apparent;
    out.reactivePower = reactive;
    out.powerFactor  = pfCal;
    out.frequency    = rawF;
    out.energyWh     = rawE;
    out.timestampMs  = millis();
}

float Calibration::computeUnbalance(const PhaseReading phases[3]) {
    float vAvg = (phases[0].voltage + phases[1].voltage + phases[2].voltage) / 3.0f;
    if (vAvg <= 0.0f) return 0.0f;
    float maxDev = 0.0f;
    for (int i = 0; i < 3; i++) {
        float dev = fabsf(phases[i].voltage - vAvg);
        if (dev > maxDev) maxDev = dev;
    }
    return (maxDev / vAvg) * 100.0f;
}
