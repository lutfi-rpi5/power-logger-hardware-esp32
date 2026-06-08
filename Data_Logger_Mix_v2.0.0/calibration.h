#pragma once
#include <Arduino.h>
#include "types.h"

/**
 * @file calibration.h
 * @brief Additive calibration model for PZEM-004T sensors with full power
 *        triangle recomputation.
 *
 * Implements an additive offset model where voltage and current readings
 * are adjusted by per-phase offsets. All derived power parameters are
 * proportionally recomputed to maintain a consistent power triangle:
 *
 * ```
 * V_cal = V_raw + V_offset[phase]
 * I_cal = I_raw + I_offset[phase]
 * S_cal = V_cal × I_cal
 * P_cal = P_raw × (S_cal / S_raw)
 * E_cal = E_raw × (P_cal / P_raw)
 * PF_cal = P_cal / S_cal
 * Q_cal = √(S_cal² - P_cal²)
 * ```
 *
 * Only frequency (Hz) is passed through unchanged, as it is determined
 * by the grid, not by sensor accuracy.
 *
 * Reference: IEEE Std 1459-2010 — Definitions for the Measurement of
 * Electric Power Quantities Under Sinusoidal Conditions.
 *
 * Thread safety: Stateless — all mutable state is set via setOffsets().
 * Const methods (apply, computeUnbalance) are safe to call from any task.
 */

/**
 * @class Calibration
 * @brief Applies V/I offsets and recomputes the full power triangle.
 *
 * The Calibration object holds three per-phase offset pairs (V, I).
 * When apply() is called, it:
 *  1. Applies additive offsets to V and I.
 *  2. Recomputes apparent power S = V_cal × I_cal.
 *  3. Scales active power P proportionally to the S_cal/S_raw ratio.
 *  4. Scales cumulative energy E proportionally to the P_cal/P_raw ratio.
 *  5. Recomputes PF and Q from calibrated values.
 *
 * If the raw V or I is NaN or ≤ 0, the phase is classified as LOST and
 * all outputs are zeroed. The previous energy value is preserved to
 * avoid resetting accumulated counters on transient noise.
 */
class Calibration {
public:
    /**
     * @brief Construct with zero offsets for all three phases.
     */
    Calibration();

    /**
     * @brief Load calibration offsets from a CalibrationConfig struct.
     * @param data Config containing voltageOffset[3] and currentOffset[3].
     */
    void setOffsets(const CalibrationConfig& data);

    /**
     * @brief Get a copy of the currently active offsets.
     * @return CalibrationConfig with current per-phase V/I offsets.
     */
    CalibrationConfig getOffsets() const;

    /**
     * @brief Apply calibration offsets and recompute all power parameters.
     *
     * @param rawV       Raw voltage from PZEM (V)
     * @param rawI       Raw current from PZEM (A)
     * @param rawP       Raw active power from PZEM (W)
     * @param rawE       Raw cumulative energy from PZEM (Wh)
     * @param rawF       Raw frequency from PZEM (Hz) — passed through unchanged
     * @param rawPF      Raw power factor from PZEM (unitless) — NOT used;
     *                   PF is recomputed from P_cal / S_cal
     * @param phaseIndex Phase index: 0 = R, 1 = S, 2 = T
     * @param out        [out] PhaseReading populated with calibrated values
     */
    void apply(float rawV, float rawI, float rawP, float rawE,
               float rawF, float rawPF, int phaseIndex,
               PhaseReading& out) const;

    /**
     * @brief Compute voltage unbalance percentage using the NEMA method.
     *
     * NEMA MG-1 standard formula:
     * ```
     * V_avg = (V_R + V_S + V_T) / 3
     * max_dev = max(|V_R - V_avg|, |V_S - V_avg|, |V_T - V_avg|)
     * unbalance = (max_dev / V_avg) × 100
     * ```
     *
     * @param phases Array of 3 PhaseReading structs [R, S, T].
     * @return Unbalance as a percentage (0.0–100.0). Returns 0.0 when
     *         average voltage is ≤ 0.
     */
    static float computeUnbalance(const PhaseReading phases[3]);

private:
    CalibrationConfig _offsets;  ///< Per-phase V/I offsets loaded from NVS
};
