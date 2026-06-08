/******************************************************
 * File      : Button.cpp
 * Note      : Uses millis()-based debounce – NO delay() calls.
 ******************************************************/

#include "Button.h"

Button::Button(uint8_t pin, unsigned long longPressMs, unsigned long debounceMs)
    : _pin(pin)
    , _longPressMs(longPressMs)
    , _debounceMs(debounceMs)
    , _state(HIGH)
    , _lastRaw(HIGH)
    , _lastChangeMs(0)
    , _pressStartMs(0)
    , _longPressHandled(false)
    , _shortPressDetected(false)
    , _longPressDetected(false)
{
    pinMode(_pin, INPUT_PULLUP);
}

void Button::update() {
    bool raw = digitalRead(_pin);
    unsigned long now = millis();

    // Detect raw edge for debounce timer reset
    if (raw != _lastRaw) {
        _lastChangeMs = now;
        _lastRaw = raw;
    }

    // Only accept new state after debounce window has passed
    if ((now - _lastChangeMs) >= _debounceMs) {
        if (raw != _state) {
            _state = raw;

            if (_state == LOW) {
                // Button pressed down
                _pressStartMs    = now;
                _longPressHandled = false;
            } else {
                // Button released
                unsigned long held = now - _pressStartMs;
                if (held < _longPressMs && !_longPressHandled) {
                    _shortPressDetected = true;
                }
            }
        }
    }

    // Long press: fire once while still held
    if (_state == LOW && !_longPressHandled) {
        if ((now - _pressStartMs) >= _longPressMs) {
            _longPressHandled   = true;
            _longPressDetected  = true;
        }
    }
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

unsigned long Button::holdDuration() const {
    if (_state == LOW) return millis() - _pressStartMs;
    return 0;
}
