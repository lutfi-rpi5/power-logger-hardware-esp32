#include "button_manager.h"
#include "config.h"

ButtonManager::ButtonManager(uint8_t pin, unsigned long longPressMs)
    : _pin(pin)
    , _longPressMs(longPressMs)
    , _debounceMs(BUTTON_DEBOUNCE_MS)
    , _lastState(HIGH)
    , _state(HIGH)
    , _pressStartMs(0)
    , _longPressHandled(false)
    , _shortPressFlag(false)
    , _longPressFlag(false)
{}

void ButtonManager::begin() {
    pinMode(_pin, INPUT_PULLUP);
}

void ButtonManager::update() {
    int reading = digitalRead(_pin);

    // Debounce
    if (reading != _lastState) {
        delay(_debounceMs);
        reading = digitalRead(_pin);
    }

    // State change
    if (reading != _state) {
        _state = reading;

        if (_state == LOW) {
            // Press started
            _pressStartMs = millis();
            _longPressHandled = false;
        } else {
            // Released — check if it was a short press
            unsigned long duration = millis() - _pressStartMs;
            if (duration < _longPressMs && !_longPressHandled) {
                _shortPressFlag = true;
            }
        }
    }

    // Long press detection (while still held)
    if (_state == LOW && !_longPressHandled) {
        if (millis() - _pressStartMs >= _longPressMs) {
            _longPressHandled = true;
            _longPressFlag = true;
        }
    }

    _lastState = reading;
}

bool ButtonManager::isShortPressed() {
    if (_shortPressFlag) {
        _shortPressFlag = false;
        return true;
    }
    return false;
}

bool ButtonManager::isLongPressed() {
    if (_longPressFlag) {
        _longPressFlag = false;
        return true;
    }
    return false;
}

void ButtonManager::setLongPressTime(unsigned long ms) {
    _longPressMs = ms;
}

unsigned long ButtonManager::getLongPressTime() const {
    return _longPressMs;
}
