#include "calibration.h"
#include <math.h>

/**
 * @file calibration.cpp
 * @brief Implementation of additive V/I calibration with full power triangle recomputation.
 *
 * Mathematical model (per phase):
 * ```
 * V_cal = V_raw + V_offset  (clamped to ≥ 0)
 * I_cal = I_raw + I_offset  (clamped to ≥ 0)
 *
 * S_raw = V_raw × I_raw
 * S_cal = V_cal × I_cal
 *
 * P_cal = P_raw × (S_cal / S_raw)   [if S_raw > 0]
 * E_cal = E_raw × (P_cal / P_raw)   [if P_raw > 0]
 *
 * PF_cal = P_cal / S_cal            [if S_cal > 0]
 * Q_cal  = √(S_cal² - P_cal²)       [if S_cal² > P_cal²]
 * ```
 *
 * Reference: IEEE Std 1459-2010 — Definitions for the Measurement of
 * Electric Power Quantities Under Sinusoidal, Nonsinusoidal, or
 * Balanced Conditions.
 */

Calibration::Calibration() {
    // Initialise all offsets to zero (no adjustment)
    for (int i = 0; i < 3; i++) {
        _offsets.voltageOffset[i] = 0.0f;
        _offsets.currentOffset[i] = 0.0f;
    }
}

void Calibration::setOffsets(const CalibrationConfig& data) { _offsets = data; }
CalibrationConfig Calibration::getOffsets() const { return _offsets; }

/**
 * @brief Apply calibration offsets and recompute the full power triangle.
 *
 * Step-by-step processing:
 * 1. Apply additive V/I offsets, clamp to [0, +inf).
 * 2. Compute raw and calibrated apparent power (S = V × I).
 * 3. If V or I is NaN/zero → mark LOST, preserve energy, exit early.
 * 4. Scale active power (P) proportionally to the S_cal / S_raw ratio.
 *    This preserves the power factor (cos φ) characteristic of the load.
 * 5. Scale cumulative energy (E) proportionally to the P_cal / P_raw ratio.
 * 6. Recompute PF = P_cal / S_cal, clamped to [0, 1].
 * 7. Recompute Q = √(S_cal² - P_cal²).
 *
 * @param rawV       Raw voltage from PZEM (V)
 * @param rawI       Raw current from PZEM (A)
 * @param rawP       Raw active power from PZEM (W)
 * @param rawE       Raw cumulative energy from PZEM (Wh)
 * @param rawF       Raw frequency from PZEM (Hz) — passed through unchanged
 * @param rawPF      Raw power factor from PZEM (not used — recomputed)
 * @param phaseIndex Phase index: 0=R, 1=S, 2=T
 * @param out        [out] PhaseReading populated with calibrated values
 */
void Calibration::apply(float rawV, float rawI, float rawP, float rawE,
                         float rawF, float rawPF, int phaseIndex,
                         PhaseReading& out) const
{
    // ── Step 1: Apply additive offsets, clamp to non-negative ────────
    // Negative V/I values are physically impossible for RMS measurements;
    // clamping prevents nonsensical derived values.
    float vCal = rawV + _offsets.voltageOffset[phaseIndex];
    float iCal = rawI + _offsets.currentOffset[phaseIndex];
    if (vCal < 0.0f) vCal = 0.0f;
    if (iCal < 0.0f) iCal = 0.0f;

    // ── Step 2: Compute raw and calibrated apparent power ─────────────
    // S_raw is used as the scaling reference; S_cal is the output value.
    float rawApparent = rawV * rawI;
    float calApparent = vCal * iCal;

    // ── Step 3: Guard — invalid readings → LOST status ───────────────
    // NaN raw values indicate a PZEM communication failure (no response).
    // V ≤ 0 means the phase is effectively dead.
    // In this case, preserve the previous energy value (do NOT reset
    // accumulated counters on transient communication glitches).
    if (isnan(rawV) || rawV <= 0.0f || isnan(rawI)) {
        out.valid       = false;
        out.voltage     = 0.0f;
        out.current     = 0.0f;
        out.activePower = 0.0f;
        out.apparentPower = 0.0f;
        out.reactivePower = 0.0f;
        out.powerFactor = 0.0f;
        out.frequency   = rawF;
        out.energyWh    = rawE;   // Preserve last known energy value
        out.status      = LineStatus::LOST;
        out.timestampMs = millis();
        return;
    }

    // ── Step 4: Scale active power proportionally to S change ────────
    // Rationale: P = S × PF. Since S changed due to V/I calibration,
    // P must scale by the same ratio to keep PF representative of the
    // actual load. This maintains power triangle consistency.
    // Edge case: if S_raw = 0, keep P raw (no scaling possible).
    float calPower = rawP;
    if (rawApparent > 0.0f) {
        calPower = rawP * (calApparent / rawApparent);
    }

    // ── Step 5: Scale cumulative energy proportionally to P change ───
    // Energy accumulation must match the reported P to avoid inconsistent
    // billing-grade totals. Edge case: if P_raw = 0, keep E raw.
    float calEnergy = rawE;
    if (rawP > 0.0f) {
        calEnergy = rawE * (calPower / rawP);
    }

    // ── Step 6: Recompute power factor from calibrated values ────────
    // PF is clamped to [0, 1] to avoid invalid values from floating-point
    // edge cases. A negative PF would indicate regeneration (grid-tie
    // inverter), which is out of scope for this monitoring-only firmware.
    float pfCal = (calApparent > 0.0f) ? (calPower / calApparent) : 0.0f;
    if (pfCal > 1.0f) pfCal = 1.0f;
    if (pfCal < 0.0f) pfCal = 0.0f;

    // ── Step 7: Recompute reactive power from the power triangle ─────
    // Q = √(S² - P²). If P > S (floating-point edge case), Q = 0.
    float reactive = 0.0f;
    float s2 = calApparent * calApparent;
    float p2 = calPower * calPower;
    if (s2 > p2) reactive = sqrtf(s2 - p2);

    // ── Populate output struct ───────────────────────────────────────
    out.valid        = true;
    out.voltage      = vCal;
    out.current      = iCal;
    out.activePower  = calPower;
    out.apparentPower = calApparent;
    out.reactivePower = reactive;
    out.powerFactor  = pfCal;
    out.frequency    = rawF;   // Hz is NOT affected by calibration
    out.energyWh     = calEnergy;
    out.timestampMs  = millis();
}

/**
 * @brief Compute voltage unbalance using the NEMA MG-1 standard method.
 *
 * NEMA unbalance formula (IEEE Std 141 / NEMA MG-1):
 * ```
 * V_avg = (V_R + V_S + V_T) / 3
 * max_dev = max(|V_R - V_avg|, |V_S - V_avg|, |V_T - V_avg|)
 * unbalance = (max_dev / V_avg) × 100
 * ```
 *
 * This is the standard method used in SPLN (Indonesia) for evaluating
 * three-phase supply quality. The result is a percentage of the average.
 *
 * @param phases Array of 3 PhaseReading structs [R, S, T].
 * @return Unbalance percentage (0.0–100.0). Returns 0.0 if V_avg ≤ 0.
 */
float Calibration::computeUnbalance(const PhaseReading phases[3]) {
    float vAvg = (phases[0].voltage + phases[1].voltage + phases[2].voltage) / 3.0f;
    if (vAvg <= 0.0f) return 0.0f;

    // Find the maximum absolute deviation from the average
    float maxDev = 0.0f;
    for (int i = 0; i < 3; i++) {
        float dev = fabsf(phases[i].voltage - vAvg);
        if (dev > maxDev) maxDev = dev;
    }

    return (maxDev / vAvg) * 100.0f;
}
