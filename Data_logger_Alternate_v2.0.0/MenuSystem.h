#pragma once

/******************************************************
 * File      : MenuSystem.h
 * Description:
 *   State machine for the OLED menu.
 *   Driven by a single push button:
 *     - Short press → navigate (next page / move cursor)
 *     - Long press  → confirm / enter / back
 *
 *   States:
 *     MONITORING      → 3 cycling pages of live data
 *     MENU_MAIN       → 4-item main menu
 *     MENU_CONFIG     → webserver on/off
 *     MENU_RESET_KWH  → confirm energy reset
 *     MENU_RESET_RESULT → result screen (auto-return 2s)
 *     MENU_REBOOT     → confirm reboot
 *     MENU_REBOOT_COUNTDOWN → 3-2-1 countdown then restart
 *
 *   Callbacks are used to decouple actions:
 *     onResetKwh   → return true on success
 *     onWebToggle  → called with new desired state
 ******************************************************/

#include <Arduino.h>
#include <functional>
#include "Button.h"
#include "DisplayOLED.h"
#include "PhaseData.h"
#include "config.h"

// ─── Menu State Enum ──────────────────────────────────────
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

    /** Call once in setup() */
    void begin();

    /**
     * Call every loop().
     * Reads button, transitions state, redraws display.
     */
    void tick(const SystemState& state);

    // ── Callbacks ────────────────────────────────────────
    void onResetKwh(ResetCallback cb)  { _resetCb  = cb; }
    void onWebToggle(WebToggleCB cb)   { _webCb    = cb; }

    // ── Accessors ────────────────────────────────────────
    MenuMode getMode()       const { return _mode; }
    bool     isWebActive()   const { return _webServerOn; }
    void     setAPInfo(const char* ssid, const char* pass, const char* ip);

    /** Force a display redraw on next tick (call after EEPROM save etc). */
    void requestRedraw() { _dirty = true; }

private:
    Button&       _btn;
    DisplayOLED&  _display;

    MenuMode      _mode        = MenuMode::MONITORING;
    uint8_t       _monPage     = 0;   // 0-2
    uint8_t       _cursor      = 0;   // generic cursor
    bool          _webServerOn = false;
    bool          _resetSuccess = false;
    bool          _dirty       = true;

    // Auto-transition timer
    unsigned long _autoMs      = 0;
    // Reboot countdown
    unsigned long _rebootMs    = 0;
    int           _rebootSecs  = REBOOT_COUNTDOWN_SEC;

    // AP info for config screen
    char _apSSID[32] = DEFAULT_AP_SSID;
    char _apPass[32] = DEFAULT_AP_PASS;
    char _apIP[16]   = "0.0.0.0";

    // Callbacks
    ResetCallback _resetCb;
    WebToggleCB   _webCb;

    // Display refresh rate (monitoring pages only)
    unsigned long _lastDisplayMs = 0;

    // ── State handlers ──────────────────────────────────
    void _handleShortPress();
    void _handleLongPress();
    void _checkAutoTransitions();

    // ── Transitions ─────────────────────────────────────
    void _toMonitoring();
    void _toMenuMain();
    void _toMenuConfig();
    void _toMenuResetKwh();
    void _toMenuReboot();
    void _doReset();
    void _doWebToggle();

    // ── Rendering ────────────────────────────────────────
    void _render(const SystemState& state);
    void _renderMonitoring(const SystemState& state);
    void _renderMenu(const SystemState& state);
};
