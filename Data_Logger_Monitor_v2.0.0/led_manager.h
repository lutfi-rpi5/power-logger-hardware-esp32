#ifndef LED_MANAGER_H
#define LED_MANAGER_H

#include <Arduino.h>

// ============================================================
// Built-in LED signal manager
// ============================================================
// Patterns:
// - Boot:        fast blink 3x
// - Reset kWh:   blink 5x
// - AP active:   slow pulse
// - Error:       continuous fast blink
// ============================================================

class LEDManager {
public:
    LEDManager(uint8_t pin);

    void begin();

    void on();
    void off();
    void blink(int times, unsigned long onMs = 150, unsigned long offMs = 150);
    void pulse(unsigned long intervalMs = 2000);  // non-blocking
    void update();  // call in loop for pulse mode

private:
    uint8_t _pin;
    bool _pulsing;
    unsigned long _pulseInterval;
    unsigned long _lastToggle;
    bool _pulseState;
};

#endif
