#pragma once
#include <Arduino.h>

/**
 * @file button_manager.h
 * @brief Single button driver with hardware debounce, short press, and long press detection.
 *
 * Implements a state machine using millis()-based timing (no delay() blocking).
 * The button GPIO is configured as INPUT_PULLUP, meaning the pin reads LOW when
 * pressed and HIGH when released.
 *
 * Timing:
 * - Short press: release occurs before @ref BTN_LONG_PRESS_MS (600 ms).
 * - Long press: continuous hold beyond @ref BTN_LONG_PRESS_MS.
 * - Debounce: ignores transitions shorter than @ref BTN_DEBOUNCE_MS (50 ms).
 *
 * @note Designed for a single NO (normally open) push button connected to ground.
 */

/**
 * @class Button
 * @brief Non-blocking button state machine with short/long press detection.
 *
 * Call update() once per loop iteration, then check isShortPressed() or
 * isLongPressed(). Both return true only once per press event (auto-clearing).
 */
class Button {
public:
    /**
     * @brief Construct and initialise the button pin.
     * @param pin          GPIO number connected to the button (INPUT_PULLUP).
     * @param longPressMs  Minimum hold duration for a long press (ms).
     * @param debounceMs   Debounce window (ms).
     */
    Button(uint8_t pin,
           unsigned long longPressMs  = 2000,
           unsigned long debounceMs   = 50);

    /**
     * @brief Read the button state and update internal state machine.
     * Must be called frequently (every loop iteration) for accurate timing.
     */
    void update();

    /**
     * @brief Check if a short press was detected since last call.
     * Auto-clearing: returns true once, then resets.
     * @return true if a short press occurred.
     */
    bool isShortPressed();

    /**
     * @brief Check if a long press was detected since last call.
     * Auto-clearing: returns true once, then resets.
     * @return true if a long press occurred.
     */
    bool isLongPressed();

    /**
     * @brief Check if the button is currently held down.
     * @return true if the debounced state is LOW (pressed).
     */
    bool isHeld() const { return _state == LOW; }

    /**
     * @brief Get how long the button has been continuously held.
     * @return Duration in milliseconds (0 if not pressed).
     */
    unsigned long holdDuration() const;

private:
    uint8_t       _pin;            ///< GPIO number
    unsigned long _longPressMs;    ///< Long press threshold (ms)
    unsigned long _debounceMs;     ///< Debounce window (ms)

    bool          _state;          ///< Current debounced state (HIGH = released, LOW = pressed)
    bool          _lastRaw;        ///< Last raw reading from digitalRead()
    unsigned long _lastChangeMs;   ///< millis() at last raw state change
    unsigned long _pressStartMs;   ///< millis() when press was first detected

    bool          _longPressHandled;    ///< true after long press is fired once
    bool          _shortPressDetected;  ///< Pending short press flag (auto-clearing)
    bool          _longPressDetected;   ///< Pending long press flag (auto-clearing)
};
