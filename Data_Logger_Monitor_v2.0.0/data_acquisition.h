#ifndef DATA_ACQUISITION_H
#define DATA_ACQUISITION_H

#include <Arduino.h>
#include <PZEM004Tv30.h>
#include "types.h"

// ============================================================
// 3-Phase Data Acquisition from PZEM-004T sensors
// ============================================================
//  PZEM  Phase  Serial port  Address
//   1     R      UART1       0x10
//   2     S      UART1       0x11
//   3     T      UART2       0x12
// ============================================================

class DataAcquisition {
public:
    DataAcquisition();
    void begin();

    // Read all 3 phases, returns raw values (pre-calibration)
    void readAll();

    // Access raw readings
    float getRawVoltage(int phase) const;
    float getRawCurrent(int phase) const;
    float getRawPower(int phase) const;
    float getRawEnergy(int phase) const;
    float getRawFrequency(int phase) const;
    float getRawPF(int phase) const;
    bool  isDataValid(int phase) const;

    // Determine line status from calibrated voltage
    LineStatus determineLineStatus(float voltage, const ThresholdData& thr) const;

    // Generate fake data for testing
    void generateFakeData();

    // Reset all PZEM energy counters
    void resetEnergy();

private:
    HardwareSerial _serial1;
    HardwareSerial _serial2;
    PZEM004Tv30 _pzem[3];

    float _rawV[3];
    float _rawI[3];
    float _rawP[3];
    float _rawE[3];
    float _rawF[3];
    float _rawPF[3];
};

#endif
