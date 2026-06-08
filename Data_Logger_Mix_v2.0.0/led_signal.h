#pragma once
#include <Arduino.h>

/**
 * @file led_signal.h
 * @brief Built-in LED controller with non-blocking blink patterns.
 *
 * Drives the ESP32 built-in LED (GPIO2, active HIGH) to indicate
 * device status through configurable blink sequences. All timing is
 * millis()-based — no delay() calls.
 *
 * Pattern definitions:
 * - IDLE: LED off.
 * - BOOTING: Continuous slow blink (500 ms on/off).
 * - WIFI_OK: Double-burst (2 blinks) on WiFi connect.
 * - MQTT_OK: Triple-burst (3 blinks) on MQTT connect.
 * - RESET_KWH: Five-burst on energy reset command.
 * - ERROR_BLINK: Fast triple-burst on error conditions.
 * - SOLID_ON: LED continuously on.
 *
 * @note The built-in LED on ESP32 DevKit V1 is active LOW on some
 *       board variants, but this firmware assumes active HIGH (GPIO2).
 */

/**
 * @class LEDSignal
 * @brief Non-blocking LED pattern player.
 *
 * Patterns are fire-and-forget: call play(Pattern) to start a sequence,
 * then call tick() from the main loop. The LED state machine advances
 * autonomously. isPlaying() returns true while a finite blink sequence
 * is in progress.
 */
class LEDSignal {
public:
    /**
     * @enum Pattern
     * @brief Available LED indication patterns.
     */
    enum class Pattern : uint8_t {
        IDLE = 0,        ///< LED off (default state)
        BOOTING,         ///< Slow continuous blink during boot
        WIFI_OK,         ///< Double-burst on WiFi connection established
        MQTT_OK,         ///< Triple-burst on MQTT connection established
        RESET_KWH,       ///< Five-burst on energy reset command
        ERROR_BLINK,     ///< Fast triple-burst on error
        SOLID_ON         ///< LED continuously on
    };

    /**
     * @brief Construct the LED controller.
     * @param pin GPIO number for the LED (active HIGH).
     */
    explicit LEDSignal(uint8_t pin);

    /**
     * @brief Initialise the GPIO pin and set initial state (LOW/off).
     */
    void begin();

    /**
     * @brief Start a blink pattern. Interrupts any currently playing pattern.
     * @param p Pattern to play.
     */
    void play(Pattern p);

    /**
     * @brief Advance the LED state machine. Call this from the main loop.
     */
    void tick();

    /**
     * @brief Check if a finite blink pattern is still in progress.
     * @return true if actively blinking (BOOTING counts as infinite, returns false).
     */
    bool isPlaying() const;

private:
    uint8_t  _pin;              ///< LED GPIO pin
    Pattern  _current = Pattern::IDLE;  ///< Currently active pattern
    int      _blinkCount = 0;   ///< Total blinks requested (-1 = infinite)
    int      _blinkDone  = 0;   ///< Completed blink cycles
    unsigned long _lastToggle = 0; ///< millis() of last LED state transition
    unsigned int  _onMs   = 0;  ///< On-duration per blink (ms)
    unsigned int  _offMs  = 0;  ///< Off-duration per blink (ms)
    bool          _ledOn  = false;  ///< Physical LED state
    bool          _playing = false; ///< Pattern sequence active

    /**
     * @brief Configure parameters for a finite blink sequence.
     * @param blinks Number of blink cycles
     * @param onMs   On time per blink (ms)
     * @param offMs  Off time per blink (ms)
     */
    void _start(int blinks, unsigned int onMs, unsigned int offMs);
};
