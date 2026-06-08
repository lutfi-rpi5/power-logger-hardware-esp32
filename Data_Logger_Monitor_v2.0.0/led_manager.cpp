#include "led_manager.h"

LEDManager::LEDManager(uint8_t pin)
    : _pin(pin)
    , _pulsing(false)
    , _pulseInterval(2000)
    , _lastToggle(0)
    , _pulseState(false)
{}

void LEDManager::begin() {
    pinMode(_pin, OUTPUT);
    off();
}

void LEDManager::on() {
    _pulsing = false;
    digitalWrite(_pin, HIGH);
}

void LEDManager::off() {
    _pulsing = false;
    digitalWrite(_pin, LOW);
}

void LEDManager::blink(int times, unsigned long onMs, unsigned long offMs) {
    _pulsing = false;
    for (int i = 0; i < times; i++) {
        digitalWrite(_pin, HIGH);
        delay(onMs);
        digitalWrite(_pin, LOW);
        if (i < times - 1) delay(offMs);
    }
}

void LEDManager::pulse(unsigned long intervalMs) {
    _pulsing = true;
    _pulseInterval = intervalMs;
    _lastToggle = millis();
    _pulseState = true;
    digitalWrite(_pin, HIGH);
}

void LEDManager::update() {
    if (!_pulsing) return;

    unsigned long now = millis();
    if (now - _lastToggle >= _pulseInterval / 2) {
        _lastToggle = now;
        _pulseState = !_pulseState;
        digitalWrite(_pin, _pulseState ? HIGH : LOW);
    }
}
