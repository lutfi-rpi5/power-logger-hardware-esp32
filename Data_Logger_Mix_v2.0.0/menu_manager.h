#pragma once
#include <Arduino.h>
#include <functional>
#include "button_manager.h"
#include "display_oled.h"
#include "types.h"
#include "config.h"

enum class MenuMode : uint8_t {
    MONITORING          = 0,
    MENU_MAIN           = 1,
    MENU_CONFIG         = 2,
    MENU_RESET_KWH      = 3,
    MENU_RESET_RESULT   = 4,
    MENU_REBOOT         = 5,
    MENU_REBOOT_CDOWN   = 6
};

class MenuSystem {
public:
    using ResetCallback  = std::function<bool()>;
    using WebToggleCB    = std::function<void(bool active)>;

    MenuSystem(Button& btn, DisplayOLED& display);
    void begin();
    void tick(const SystemState& state);

    void onResetKwh(ResetCallback cb)  { _resetCb  = cb; }
    void onWebToggle(WebToggleCB cb)   { _webCb    = cb; }

    MenuMode getMode()       const { return _mode; }
    bool     isWebActive()   const { return _webServerOn; }
    void     setAPInfo(const char* ssid, const char* pass, const char* ip);
    void     requestRedraw() { _dirty = true; }

private:
    Button&       _btn;
    DisplayOLED&  _display;

    MenuMode      _mode        = MenuMode::MONITORING;
    uint8_t       _monPage     = 0;
    uint8_t       _cursor      = 0;
    bool          _webServerOn = false;
    bool          _resetSuccess = false;
    bool          _dirty       = true;

    unsigned long _autoMs      = 0;
    unsigned long _rebootMs    = 0;
    int           _rebootSecs  = REBOOT_COUNTDOWN_SEC;

    char _apSSID[32] = DEFAULT_AP_SSID;
    char _apPass[32] = DEFAULT_AP_PASS;
    char _apIP[16]   = "0.0.0.0";

    ResetCallback _resetCb;
    WebToggleCB   _webCb;

    unsigned long _lastDisplayMs = 0;

    void _handleShortPress();
    void _handleLongPress();
    void _checkAutoTransitions();

    void _toMonitoring();
    void _toMenuMain();
    void _toMenuConfig();
    void _toMenuResetKwh();
    void _toMenuReboot();
    void _doReset();
    void _doWebToggle();

    void _render(const SystemState& state);
    void _renderMonitoring(const SystemState& state);
    void _renderMenu(const SystemState& state);
};
