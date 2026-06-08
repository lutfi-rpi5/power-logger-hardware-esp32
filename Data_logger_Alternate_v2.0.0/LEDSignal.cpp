/******************************************************
 * File      : LEDSignal.cpp
 ******************************************************/

#include "LEDSignal.h"

LEDSignal::LEDSignal(uint8_t pin) : _pin(pin) {}

void LEDSignal::begin() {
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);
}

void LEDSignal::play(Pattern p) {
    _current = p;
    _playing = false;
    _ledOn   = false;

    switch (p) {
        case Pattern::IDLE:
            digitalWrite(_pin, LOW);
            return;

        case Pattern::SOLID_ON:
            digitalWrite(_pin, HIGH);
            _ledOn = true;
            return;

        case Pattern::BOOTING:
            // Continuous slow blink, handled by tick()
            _blinkCount = 0;   // 0 = infinite
            _onMs  = 500;
            _offMs = 500;
            _playing = true;
            _lastToggle = millis();
            digitalWrite(_pin, HIGH);
            _ledOn = true;
            return;

        case Pattern::WIFI_OK:     _start(2, 120, 120); return;
        case Pattern::MQTT_OK:     _start(3, 100, 100); return;
        case Pattern::RESET_KWH:   _start(5, 120, 120); return;
        case Pattern::ERROR_BLINK: _start(3,  80,  80); return;

        default: return;
    }
}

void LEDSignal::_start(int blinks, unsigned int onMs, unsigned int offMs) {
    _blinkCount = blinks;
    _blinkDone  = 0;
    _onMs       = onMs;
    _offMs      = offMs;
    _playing    = true;
    _lastToggle = millis();
    digitalWrite(_pin, HIGH);
    _ledOn = true;
}

void LEDSignal::tick() {
    if (!_playing) return;

    unsigned long now = millis();
    unsigned long elapsed = now - _lastToggle;

    if (_ledOn) {
        if (elapsed >= _onMs) {
            digitalWrite(_pin, LOW);
            _ledOn      = false;
            _lastToggle = now;
            if (_blinkCount > 0) _blinkDone++;  // finite mode
        }
    } else {
        // LED is off
        unsigned long waitOff = _offMs;
        if (elapsed >= waitOff) {
            // Check if finite pattern is done
            if (_blinkCount > 0 && _blinkDone >= _blinkCount) {
                _playing = false;
                _current = Pattern::IDLE;
                return;
            }
            // Next blink
            digitalWrite(_pin, HIGH);
            _ledOn      = true;
            _lastToggle = now;
        }
    }
}

bool LEDSignal::isPlaying() const {
    return _playing && (_blinkCount > 0);  // infinite (BOOTING) returns false
}
