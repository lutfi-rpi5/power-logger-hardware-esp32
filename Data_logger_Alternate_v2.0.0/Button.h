#pragma once

/******************************************************
 * File      : Button.h
 * Description:
 *   Non-blocking single-button handler.
 *   Detects short press and long press events using
 *   millis()-based timing (no blocking delay calls).
 *
 *   Usage:
 *     Button btn(PIN_BUTTON, BTN_LONG_PRESS_MS, BTN_DEBOUNCE_MS);
 *     // In loop():
 *     btn.update();
 *     if (btn.isShortPressed()) { ... }
 *     if (btn.isLongPressed())  { ... }
 ******************************************************/

#include <Arduino.h>

class Button {
public:
    /**
     * @param pin           GPIO pin (INPUT_PULLUP assumed, active LOW)
     * @param longPressMs   Duration threshold for long press (ms)
     * @param debounceMs    Debounce window (ms)
     */
    Button(uint8_t pin,
           unsigned long longPressMs  = 2000,
           unsigned long debounceMs   = 50);

    /** Call every loop iteration – reads pin and updates state */
    void update();

    /** Returns true once when a short press is complete (on release) */
    bool isShortPressed();

    /** Returns true once when a long press threshold is reached */
    bool isLongPressed();

    /** True while button is physically held down */
    bool isHeld() const { return _state == LOW; }

    /** How long the button has been held so far (0 if not pressed) */
    unsigned long holdDuration() const;

private:
    uint8_t       _pin;
    unsigned long _longPressMs;
    unsigned long _debounceMs;

    bool          _state;               // Current debounced state
    bool          _lastRaw;             // Last raw reading
    unsigned long _lastChangeMs;        // Time of last raw change (for debounce)
    unsigned long _pressStartMs;        // Time button went LOW

    bool          _longPressHandled;    // Prevent repeat long-press fires
    bool          _shortPressDetected;  // Queued short press event
    bool          _longPressDetected;   // Queued long press event
};
