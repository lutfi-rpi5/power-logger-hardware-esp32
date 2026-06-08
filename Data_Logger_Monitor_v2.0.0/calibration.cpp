#include "calibration.h"
#include <math.h>

Calibration::Calibration() {
    for (int i = 0; i < 3; i++) {
        _offsets.voltageOffset[i] = 0.0f;
        _offsets.currentOffset[i] = 0.0f;
    }
}

void Calibration::setOffsets(const CalibrationData& data) {
    _offsets = data;
}

CalibrationData Calibration::getOffsets() const {
    return _offsets;
}

void Calibration::apply(float rawV, float rawI, float rawP, float rawE,
                        float rawF, float rawPF, int phaseIndex,
                        PhaseData& out) const
{
    // Apply offset calibration
    float vCal = rawV + _offsets.voltageOffset[phaseIndex];
    float iCal = rawI + _offsets.currentOffset[phaseIndex];

    // Protect against negative values from aggressive negative offsets
    if (vCal < 0.0f) vCal = 0.0f;
    if (iCal < 0.0f) iCal = 0.0f;

    // Recompute derived parameters from calibrated V and I
    float apparent  = vCal * iCal;
    float power     = rawP;  // PZEM reports active power directly

    // If raw readings are invalid, zero everything
    if (isnan(rawV) || rawV <= 0.0f || isnan(rawI)) {
        out.valid     = false;
        out.voltage   = 0.0f;
        out.current   = 0.0f;
        out.power     = 0.0f;
        out.apparent  = 0.0f;
        out.reactive  = 0.0f;
        out.pf        = 0.0f;
        out.frequency = rawF;
        out.energy    = rawE;
        out.status    = LineStatus::LOST;
        return;
    }

    // Recompute PF from calibrated V and I
    float pfCal = (apparent > 0.0f) ? (power / apparent) : 0.0f;
    if (pfCal > 1.0f) pfCal = 1.0f;
    if (pfCal < 0.0f) pfCal = 0.0f;

    // Reactive power: sqrt(S^2 - P^2)
    float reactive = 0.0f;
    float s2 = apparent * apparent;
    float p2 = power * power;
    if (s2 > p2) {
        reactive = sqrtf(s2 - p2);
    }

    // Energy remains as reported by PZEM (we don't recompute it)
    float energy = rawE;

    out.valid     = true;
    out.voltage   = vCal;
    out.current   = iCal;
    out.power     = power;
    out.apparent  = apparent;
    out.reactive  = reactive;
    out.pf        = pfCal;
    out.frequency = rawF;   // Hz NOT affected by calibration
    out.energy    = energy;
}

float Calibration::computeUnbalance(const PhaseData phases[3]) {
    // NEMA method: voltage unbalance from line-to-neutral readings
    float vAvg = (phases[0].voltage + phases[1].voltage + phases[2].voltage) / 3.0f;
    if (vAvg <= 0.0f) return 0.0f;

    float maxDev = 0.0f;
    for (int i = 0; i < 3; i++) {
        float dev = fabsf(phases[i].voltage - vAvg);
        if (dev > maxDev) maxDev = dev;
    }

    return (maxDev / vAvg) * 100.0f;
}
