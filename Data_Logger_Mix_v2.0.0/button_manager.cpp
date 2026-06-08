#include "button_manager.h"

/**
 * @file button_manager.cpp
 * @brief Non-blocking button driver with debounce and short/long press detection.
 *
 * State machine overview:
 * ```
 * HIGH (released) ──(raw reads LOW for > debounceMs)──→ LOW (pressed, start timer)
 * LOW (pressed) ──(held >= longPressMs)──→ Long press fired (once)
 * LOW (pressed) ──(raw reads HIGH for > debounceMs)──→ HIGH (released)
 *   ├── held < longPressMs → Short press fired
 *   └── held >= longPressMs → Long press already fired (suppress short)
 * ```
 */

/**
 * @brief Construct the button driver and configure the GPIO.
 * @param pin          GPIO number (INPUT_PULLUP enabled).
 * @param longPressMs  Minimum hold to register a long press (ms).
 * @param debounceMs   Debounce window (ms) — ignores noise shorter than this.
 */
Button::Button(uint8_t pin, unsigned long longPressMs, unsigned long debounceMs)
    : _pin(pin)
    , _longPressMs(longPressMs)
    , _debounceMs(debounceMs)
    , _state(HIGH)         // Default: not pressed (pull-up)
    , _lastRaw(HIGH)
    , _lastChangeMs(0)
    , _pressStartMs(0)
    , _longPressHandled(false)
    , _shortPressDetected(false)
    , _longPressDetected(false)
{
    pinMode(_pin, INPUT_PULLUP);
}

/**
 * @brief Read the raw GPIO state and update the debounce/press state machine.
 *
 * Must be called at least once every ~10 ms for reliable debounce timing.
 * Designed to be called from the main loop() once per iteration.
 */
void Button::update() {
    bool raw = digitalRead(_pin);        // LOW = pressed (pull-up), HIGH = released
    unsigned long now = millis();

    // ── Raw edge detection ────────────────────────────────────────────
    // Record the timestamp of every raw state change for debouncing
    if (raw != _lastRaw) {
        _lastChangeMs = now;
        _lastRaw = raw;
    }

    // ── Debounce: accept new state only after stable for debounceMs ──
    if ((now - _lastChangeMs) >= _debounceMs) {
        if (raw != _state) {
            _state = raw;
            if (_state == LOW) {
                // ── Press detected ────────────────────────────────────
                _pressStartMs    = now;
                _longPressHandled = false;
            } else {
                // ── Release detected ──────────────────────────────────
                unsigned long held = now - _pressStartMs;
                if (held < _longPressMs && !_longPressHandled) {
                    _shortPressDetected = true;  // Short press: released before threshold
                }
            }
        }
    }

    // ── Long press detection (continuous hold without release) ───────
    // Fires exactly once when held duration crosses longPressMs
    if (_state == LOW && !_longPressHandled) {
        if ((now - _pressStartMs) >= _longPressMs) {
            _longPressHandled   = true;
            _longPressDetected  = true;
        }
    }
}

/**
 * @brief Check-and-clear short press flag.
 * @return true if a short press event has occurred since last call.
 */
bool Button::isShortPressed() {
    if (_shortPressDetected) {
        _shortPressDetected = false;
        return true;
    }
    return false;
}

/**
 * @brief Check-and-clear long press flag.
 * @return true if a long press event has occurred since last call.
 */
bool Button::isLongPressed() {
    if (_longPressDetected) {
        _longPressDetected = false;
        return true;
    }
    return false;
}

/**
 * @brief Get the duration of the current button press.
 * @return Milliseconds since press started, or 0 if not pressed.
 */
unsigned long Button::holdDuration() const {
    if (_state == LOW) return millis() - _pressStartMs;
    return 0;
}
