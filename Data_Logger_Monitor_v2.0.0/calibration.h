#ifndef CALIBRATION_H
#define CALIBRATION_H

#include "types.h"

// ============================================================
// PZEM-004T Calibration — additive offset model
// ============================================================
// Calibration affects all derived parameters:
//   V_cal = V_raw + V_offset
//   I_cal = I_raw + I_offset
//   VA    = V_cal * I_cal
//   W     = VA * PF (PF recalculated)
//   VAr   = sqrt(VA^2 - W^2)
//   Wh    = computed from W * delta_t
//   PF    = W / VA (re-derived from calibrated values)
//   Hz    = NOT affected by calibration
// ============================================================

class Calibration {
public:
    Calibration();

    void setOffsets(const CalibrationData& data);
    CalibrationData getOffsets() const;

    // Apply calibration to raw readings, fill corrected PhaseData
    void apply(float rawV, float rawI, float rawP, float rawE,
               float rawF, float rawPF, int phaseIndex, PhaseData& out) const;

    // Compute voltage unbalance (NEMA method) from 3 phases
    static float computeUnbalance(const PhaseData phases[3]);

private:
    CalibrationData _offsets;
};

#endif
