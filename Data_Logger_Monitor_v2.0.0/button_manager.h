#ifndef BUTTON_MANAGER_H
#define BUTTON_MANAGER_H

#include <Arduino.h>

// ============================================================
// Single button manager — short press, long press (2s)
// ============================================================
// Short press  → navigate / cycle / cursor
// Long press   → enter / confirm / back (configurable duration)
// ============================================================

class ButtonManager {
public:
    ButtonManager(uint8_t pin, unsigned long longPressMs = 2000);

    void begin();
    void update();  // call every loop iteration

    bool isShortPressed();
    bool isLongPressed();

    void setLongPressTime(unsigned long ms);
    unsigned long getLongPressTime() const;

private:
    uint8_t _pin;
    unsigned long _longPressMs;
    unsigned long _debounceMs;

    int _lastState;
    int _state;
    unsigned long _pressStartMs;
    bool _longPressHandled;
    bool _shortPressFlag;
    bool _longPressFlag;
};

#endif
