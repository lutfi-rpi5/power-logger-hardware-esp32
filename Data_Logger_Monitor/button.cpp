#include "Button.h"

Button::Button(uint8_t pin, unsigned long longPressTime, unsigned long debounceTime)
    : _pin(pin), _longPressTime(longPressTime), _debounceTime(debounceTime) {
    pinMode(_pin, INPUT_PULLUP);
    _state = HIGH;
    _lastState = HIGH;
    _pressStart = 0;
    _longPressHandled = false;
    _shortPressDetected = false;
    _longPressDetected = false;
}

void Button::update() {
    bool reading = digitalRead(_pin);

    if (reading != _lastState) {
        delay(_debounceTime);
    }

    if (reading != _state) {
        _state = reading;

        if (_state == LOW) {
            _pressStart = millis();
            _longPressHandled = false;
        } else {
            unsigned long pressDuration = millis() - _pressStart;
            if (pressDuration < _longPressTime && !_longPressHandled) {
                _shortPressDetected = true;
            }
        }
    }

    if (_state == LOW && !_longPressHandled && (millis() - _pressStart >= _longPressTime)) {
        _longPressHandled = true;
        _longPressDetected = true;
    }

    _lastState = reading;
}

bool Button::isShortPressed() {
    if (_shortPressDetected) {
        _shortPressDetected = false;
        return true;
    }
    return false;
}

bool Button::isLongPressed() {
    if (_longPressDetected) {
        _longPressDetected = false;
        return true;
    }
    return false;
}
