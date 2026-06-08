#ifndef MENU_MANAGER_H
#define MENU_MANAGER_H

#include <Arduino.h>
#include "types.h"
#include "display_manager.h"

// ============================================================
// OLED menu navigation state machine
// ============================================================
// Two modes:
//   MONITORING — 3 pages, short press = next page, long press = enter menu
//   MENU       — 5 pages, short press = move cursor, long press = enter/confirm/back
// ============================================================

class MenuManager {
public:
    MenuManager();

    void begin(DisplayManager* display);

    // Call from main loop
    void update(bool wifiOn, bool mqttOn,
                const PhaseData phases[3], float unbalance,
                const ThresholdData& thr,
                bool apActive, const char* apIP,
                bool rebootCountdownActive, int rebootCountdownSec);

    // Button handlers (call from button events)
    void onShortPress();
    void onLongPress();

    // State queries
    OLEDMode getCurrentMode() const;
    bool isInMenu() const;
    bool shouldReboot() const;
    bool shouldResetEnergy() const;
    void clearResetEnergyFlag();
    bool shouldFactoryReset() const;
    void clearFactoryResetFlag();
    bool shouldToggleAP() const;
    void clearToggleAPFlag();
    void setAPConfig(const char* ssid, const char* pass, const char* ip);

private:
    DisplayManager* _display;
    OLEDMode _mode;
    int _cursor;
    int _subCursor;  // 0 = left, 1 = right for confirm screens
    bool _rebootFlag;
    bool _resetEnergyFlag;
    bool _factoryResetFlag;
    bool _toggleAPFlag;
    bool _rebootCountdownActive;
    int  _rebootCountdownSec;

    char _apSSID[16];
    char _apPass[16];
    char _apIP[16];

    // Menu navigation
    void _render(bool wifiOn, bool mqttOn,
                 const PhaseData phases[3], float unbalance,
                 const ThresholdData& thr,
                 bool apActive);
    void _renderMonitorPage(bool wifiOn, bool mqttOn,
                            const PhaseData phases[3], float unbalance,
                            const ThresholdData& thr);
    void _renderMenuMain(bool wifiOn, bool mqttOn);
    void _renderMenuConfig(bool apActive, bool wifiOn, bool mqttOn);
    void _renderMenuResetKWH(bool wifiOn, bool mqttOn);
    void _renderMenuReboot(bool wifiOn, bool mqttOn);
    void _renderMenuFactory(bool wifiOn, bool mqttOn);

    // Actions
    void _enterMenu();
    void _exitMenu();
    void _executeMainAction(int idx);
};

#endif
