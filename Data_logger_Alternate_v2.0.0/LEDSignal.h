#pragma once

/******************************************************
 * File      : LEDSignal.h
 * Description:
 *   Non-blocking LED indicator for system events.
 *   Uses millis()-based timing so it never blocks.
 *
 *   Predefined patterns:
 *     - BOOTING    : slow pulse
 *     - WIFI_OK    : 2 quick blinks
 *     - MQTT_OK    : 3 quick blinks
 *     - RESET_KWH  : 5 blinks (performed in resetAll)
 *     - ERROR      : fast triple blink
 *     - IDLE       : LED off
 *
 *   Usage:
 *     LEDSignal led(PIN_LED);
 *     led.play(LEDSignal::Pattern::WIFI_OK);  // non-blocking
 *     // In loop():
 *     led.tick();
 ******************************************************/

#include <Arduino.h>

class LEDSignal {
public:
    enum class Pattern : uint8_t {
        IDLE = 0,
        BOOTING,        // 1 Hz slow blink while boot pending
        WIFI_OK,        // 2 quick blinks
        MQTT_OK,        // 3 quick blinks
        RESET_KWH,      // 5 blinks (confirm energy reset)
        ERROR_BLINK,    // 3 fast blinks
        SOLID_ON        // LED constantly on
    };

    explicit LEDSignal(uint8_t pin);

    void begin();

    /** Start playing a pattern (non-blocking). */
    void play(Pattern p);

    /** Drive an ongoing BOOTING blink – call in loop() continuously. */
    void tick();

    /** True while a finite pattern is still playing. */
    bool isPlaying() const;

private:
    uint8_t  _pin;
    Pattern  _current = Pattern::IDLE;

    // Pattern playback
    int           _blinkCount  = 0;  // total blinks in pattern
    int           _blinkDone   = 0;  // blinks completed
    unsigned long _lastToggle  = 0;
    unsigned int  _onMs        = 0;
    unsigned int  _offMs       = 0;
    bool          _ledOn       = false;
    bool          _playing     = false;

    void _start(int blinks, unsigned int onMs, unsigned int offMs);
};
