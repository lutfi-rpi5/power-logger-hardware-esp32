#pragma once
#include <Arduino.h>
#include <PZEM004Tv30.h>

class ResetEnergy {
public:
    ResetEnergy(uint8_t buttonPin, uint8_t ledPin);
    void begin();
    // dipanggil rutin di loop untuk handle long press flow (sudah kamu punya sebelumnya)
    void handle(PZEM004Tv30 pzems[], int count);

    // fungsi publik baru yang bisa dipanggil kapanpun untuk reset semua PZEM
    // fungsi ini blocking ringan: mengirim perintah satu-per-satu dengan delay kecil
    void resetAll(PZEM004Tv30 pzems[], int count);

private:
    uint8_t _buttonPin;
    uint8_t _ledPin;

    // internal helper
    void blinkLed(int times, unsigned int onMs = 150, unsigned int offMs = 150);
};
