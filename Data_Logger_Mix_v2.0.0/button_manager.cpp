#include "button_manager.h"

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

    if (raw != _lastRaw) {
        _lastChangeMs = now;
        _lastRaw = raw;
    }

    if ((now - _lastChangeMs) >= _debounceMs) {
        if (raw != _state) {
            _state = raw;
            if (_state == LOW) {
                _pressStartMs    = now;
                _longPressHandled = false;
            } else {
                unsigned long held = now - _pressStartMs;
                if (held < _longPressMs && !_longPressHandled) {
                    _shortPressDetected = true;
                }
            }
        }
    }

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
