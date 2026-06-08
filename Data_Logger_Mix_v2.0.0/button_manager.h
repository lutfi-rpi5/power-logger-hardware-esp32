#pragma once
#include <Arduino.h>

class Button {
public:
    Button(uint8_t pin,
           unsigned long longPressMs  = 2000,
           unsigned long debounceMs   = 50);

    void update();
    bool isShortPressed();
    bool isLongPressed();
    bool isHeld() const { return _state == LOW; }
    unsigned long holdDuration() const;

private:
    uint8_t       _pin;
    unsigned long _longPressMs;
    unsigned long _debounceMs;

    bool          _state;
    bool          _lastRaw;
    unsigned long _lastChangeMs;
    unsigned long _pressStartMs;

    bool          _longPressHandled;
    bool          _shortPressDetected;
    bool          _longPressDetected;
};
