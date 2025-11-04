#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>

class Button {
public:
    Button(uint8_t pin, unsigned long longPressTime = 5000, unsigned long debounceTime = 50);

    void update();
    bool isShortPressed();
    bool isLongPressed();

private:
    uint8_t _pin;
    unsigned long _longPressTime;
    unsigned long _debounceTime;

    bool _state;
    bool _lastState;
    unsigned long _pressStart;
    bool _longPressHandled;
    bool _shortPressDetected;
    bool _longPressDetected;
};

#endif
