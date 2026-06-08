#include "data_acquisition.h"
#include "config.h"
#include "diagnostics.h"
#include <math.h>

DataAcquisition::DataAcquisition()
    : _serial1(PZEM_SERIAL_1)
    , _serial2(PZEM_SERIAL_2)
    , _pzem{
        PZEM004Tv30(_serial1, PZEM_RX1_PIN, PZEM_TX1_PIN, PZEM_ADDR_R),
        PZEM004Tv30(_serial1, PZEM_RX1_PIN, PZEM_TX1_PIN, PZEM_ADDR_S),
        PZEM004Tv30(_serial2, PZEM_RX2_PIN, PZEM_TX2_PIN, PZEM_ADDR_T)
      }
{
    for (int i = 0; i < NUM_PZEMS; i++) {
        _rawV[i] = 0.0f;
        _rawI[i] = 0.0f;
        _rawP[i] = 0.0f;
        _rawE[i] = 0.0f;
        _rawF[i] = 0.0f;
        _rawPF[i] = 0.0f;
    }
}

void DataAcquisition::begin() {
    _serial1.begin(PZEM_BAUD, SERIAL_8N1, PZEM_RX1_PIN, PZEM_TX1_PIN);
    _serial2.begin(PZEM_BAUD, SERIAL_8N1, PZEM_RX2_PIN, PZEM_TX2_PIN);
    diag.info("DAQ", "PZEM serial ports initialized");
}

void DataAcquisition::readAll() {
    for (int i = 0; i < NUM_PZEMS; i++) {
        _rawV[i]  = _pzem[i].voltage();
        _rawI[i]  = _pzem[i].current();
        _rawP[i]  = _pzem[i].power();
        _rawE[i]  = _pzem[i].energy() * 1000.0f;  // Convert kWh to Wh
        _rawF[i]  = _pzem[i].frequency();
        _rawPF[i] = _pzem[i].pf();

        // Fix NaN from failed reads
        if (isnan(_rawV[i]))  _rawV[i] = 0.0f;
        if (isnan(_rawI[i]))  _rawI[i] = 0.0f;
        if (isnan(_rawP[i]))  _rawP[i] = 0.0f;
        if (isnan(_rawE[i]))  _rawE[i] = 0.0f;
        if (isnan(_rawF[i]))  _rawF[i] = 0.0f;
        if (isnan(_rawPF[i])) _rawPF[i] = 0.0f;

        diag.debug("DAQ", "Phase %d raw: V=%.1f I=%.2f P=%.1f E=%.1f F=%.1f PF=%.2f",
                   i, _rawV[i], _rawI[i], _rawP[i], _rawE[i], _rawF[i], _rawPF[i]);
    }
}

float DataAcquisition::getRawVoltage(int phase) const {
    return (phase >= 0 && phase < NUM_PZEMS) ? _rawV[phase] : 0.0f;
}

float DataAcquisition::getRawCurrent(int phase) const {
    return (phase >= 0 && phase < NUM_PZEMS) ? _rawI[phase] : 0.0f;
}

float DataAcquisition::getRawPower(int phase) const {
    return (phase >= 0 && phase < NUM_PZEMS) ? _rawP[phase] : 0.0f;
}

float DataAcquisition::getRawEnergy(int phase) const {
    return (phase >= 0 && phase < NUM_PZEMS) ? _rawE[phase] : 0.0f;
}

float DataAcquisition::getRawFrequency(int phase) const {
    return (phase >= 0 && phase < NUM_PZEMS) ? _rawF[phase] : 0.0f;
}

float DataAcquisition::getRawPF(int phase) const {
    return (phase >= 0 && phase < NUM_PZEMS) ? _rawPF[phase] : 0.0f;
}

bool DataAcquisition::isDataValid(int phase) const {
    return (phase >= 0 && phase < NUM_PZEMS) && (_rawV[phase] > 0.0f);
}

LineStatus DataAcquisition::determineLineStatus(float voltage, const ThresholdData& thr) const {
    if (voltage <= 0.0f || voltage < thr.voltageLost) {
        return LineStatus::LOST;
    }
    if (voltage < thr.voltageUnder) {
        return LineStatus::UNDER;
    }
    if (voltage > thr.voltageOver) {
        return LineStatus::OVER;
    }
    return LineStatus::OK;
}

void DataAcquisition::generateFakeData() {
    for (int i = 0; i < NUM_PZEMS; i++) {
        _rawV[i]  = 200.0f + (float)random(0, 400) / 100.0f;   // 200-204 V
        _rawI[i]  = 10.0f + (float)random(0, 500) / 100.0f;     // 10-15 A
        _rawP[i]  = _rawV[i] * _rawI[i] * 0.85f;
        _rawE[i]  = 10000.0f + (float)random(0, 1000);
        _rawF[i]  = 49.9f + (float)random(0, 20) / 100.0f;      // 49.9-50.1 Hz
        _rawPF[i] = 0.80f + (float)random(0, 200) / 1000.0f;    // 0.80-0.99 PF
    }
    diag.info("DAQ", "Fake data generated for all phases");
}

void DataAcquisition::resetEnergy() {
    for (int i = 0; i < NUM_PZEMS; i++) {
        diag.info("DAQ", "Resetting energy on PZEM %d...", i);
        _pzem[i].resetEnergy();
        delay(200);  // Small delay between resets
    }
}
