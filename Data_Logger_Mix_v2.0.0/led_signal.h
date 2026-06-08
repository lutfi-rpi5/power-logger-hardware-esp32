#pragma once
#include <Arduino.h>

class LEDSignal {
public:
    enum class Pattern : uint8_t {
        IDLE = 0,
        BOOTING,
        WIFI_OK,
        MQTT_OK,
        RESET_KWH,
        ERROR_BLINK,
        SOLID_ON
    };

    explicit LEDSignal(uint8_t pin);
    void begin();
    void play(Pattern p);
    void tick();
    bool isPlaying() const;

private:
    uint8_t  _pin;
    Pattern  _current = Pattern::IDLE;
    int      _blinkCount = 0;
    int      _blinkDone  = 0;
    unsigned long _lastToggle = 0;
    unsigned int  _onMs   = 0;
    unsigned int  _offMs  = 0;
    bool          _ledOn  = false;
    bool          _playing = false;

    void _start(int blinks, unsigned int onMs, unsigned int offMs);
};
