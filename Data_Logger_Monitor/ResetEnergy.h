#ifndef RESET_ENERGY_H
#define RESET_ENERGY_H

#include <Arduino.h>
#include <PZEM004Tv30.h>

class ResetEnergy {
public:
    ResetEnergy(uint8_t btnPin, uint8_t ledPin);
    void begin();
    void handle(PZEM004Tv30* pzems, int numPzems);

private:
    uint8_t buttonPin, ledPin;
    unsigned long pressStart = 0;
    unsigned long lastDebounceTime = 0;
    unsigned long blinkStart = 0;
    bool buttonStableState = HIGH;
    bool lastButtonReading = HIGH;
    bool isPressed = false;
    bool resetTriggered = false;
    bool isBlinking = false;
    bool ledState = false;
    int blinkCount = 0;

    const unsigned long debounceDelay = 20;   // 50 ms debounce
    const unsigned long holdTime = 5000;      // 5 detik untuk reset
};

#endif
