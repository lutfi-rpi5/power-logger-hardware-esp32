#include "led_signal.h"

/**
 * @file led_signal.cpp
 * @brief Built-in LED blink pattern controller (non-blocking, millis()-based).
 */

/**
 * @brief Construct the LED controller.
 * @param pin GPIO number driving the LED (active HIGH).
 */
LEDSignal::LEDSignal(uint8_t pin) : _pin(pin) {}

/**
 * @brief Initialise the GPIO as an output and set initial state to OFF.
 */
void LEDSignal::begin() {
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);  // Start with LED off
}

/**
 * @brief Start a blink pattern, cancelling any pattern in progress.
 *
 * Pattern definitions:
 * - IDLE:        LED off (steady).
 * - SOLID_ON:    LED on (steady).
 * - BOOTING:     Continuous slow blink (500 ms on, 500 ms off), infinite repeats.
 * - WIFI_OK:     Double-blink (2 × 120 ms on, 120 ms off).
 * - MQTT_OK:     Triple-blink (3 × 100 ms on, 100 ms off).
 * - RESET_KWH:   Five-blink (5 × 120 ms on, 120 ms off).
 * - ERROR_BLINK: Fast triple-blink (3 × 80 ms on, 80 ms off).
 *
 * @param p Pattern to play.
 */
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
            _blinkCount = 0;   // 0 = infinite (continuous blink)
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

/**
 * @brief Configure a finite blink sequence.
 * @param blinks Number of on-off cycles (blinks).
 * @param onMs   LED on duration per blink (ms).
 * @param offMs  LED off duration per blink (ms).
 */
void LEDSignal::_start(int blinks, unsigned int onMs, unsigned int offMs) {
    _blinkCount = blinks;
    _blinkDone  = 0;
    _onMs       = onMs;
    _offMs      = offMs;
    _playing    = true;
    _lastToggle = millis();
    digitalWrite(_pin, HIGH);  // First blink: turn on
    _ledOn = true;
}

/**
 * @brief Advance the blink state machine. Call from the main loop.
 *
 * Non-blocking — uses millis() elapsed time to toggle the LED.
 * Finite patterns stop automatically after _blinkCount cycles.
 */
void LEDSignal::tick() {
    if (!_playing) return;
    unsigned long now = millis();
    unsigned long elapsed = now - _lastToggle;

    if (_ledOn) {
        // ── LED is currently ON — wait for on-duration ────────────────
        if (elapsed >= _onMs) {
            digitalWrite(_pin, LOW);
            _ledOn      = false;
            _lastToggle = now;
            if (_blinkCount > 0) _blinkDone++;  // Count completed blink
        }
    } else {
        // ── LED is currently OFF — wait for off-duration ──────────────
        unsigned long waitOff = _offMs;
        if (elapsed >= waitOff) {
            // Check if a finite pattern has completed all blinks
            if (_blinkCount > 0 && _blinkDone >= _blinkCount) {
                _playing = false;
                _current = Pattern::IDLE;
                return;
            }
            // Start next blink
            digitalWrite(_pin, HIGH);
            _ledOn      = true;
            _lastToggle = now;
        }
    }
}

/**
 * @brief Check if a finite blink pattern is currently in progress.
 * @return true for finite patterns that have not yet completed.
 *         Returns false for IDLE, SOLID_ON, and BOOTING (infinite).
 */
bool LEDSignal::isPlaying() const {
    return _playing && (_blinkCount > 0);
}
