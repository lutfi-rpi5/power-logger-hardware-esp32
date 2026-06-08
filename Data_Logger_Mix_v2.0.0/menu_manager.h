#pragma once
#include <Arduino.h>
#include <functional>
#include "button_manager.h"
#include "display_oled.h"
#include "types.h"
#include "config.h"

/**
 * @file menu_manager.h
 * @brief OLED menu state machine with button navigation.
 *
 * Implements a hierarchical menu system for the 128×64 OLED:
 * - Monitoring mode: 3-page rotating display (short press to cycle).
 * - Menu mode: Long press enters menu; short press moves cursor;
 *   long press confirms selection.
 *
 * Menu tree:
 * ```
 * Monitoring (Page 1/2/3)
 *     │ long press
 *     ▼
 * Main Menu
 *     ├── Config ────────▶ Toggle web server AP [Active / Inactive]
 *     ├── Reset kWh ─────▶ Confirmation → Result screen (auto-return 2 s)
 *     ├── Reboot Device ─▶ Confirmation → Countdown 3-2-1-0 → ESP.restart()
 *     └── Back to Monitoring
 * ```
 */

/**
 * @enum MenuMode
 * @brief All possible states of the menu finite state machine.
 */
enum class MenuMode : uint8_t {
    MONITORING          = 0,  ///< Default: cycling through 3 monitoring pages
    MENU_MAIN           = 1,  ///< Main menu with 4 options
    MENU_CONFIG         = 2,  ///< Web server AP toggle screen
    MENU_RESET_KWH      = 3,  ///< Reset kWh confirmation screen
    MENU_RESET_RESULT   = 4,  ///< Reset result (success/fail), auto-returns
    MENU_REBOOT         = 5,  ///< Reboot confirmation screen
    MENU_REBOOT_CDOWN   = 6   ///< Reboot countdown (3-2-1-0), non-blocking
};

/**
 * @class MenuSystem
 * @brief State machine orchestrating button input and OLED rendering.
 *
 * Call tick(state) from the main loop. The menu system reads the button
 * state, updates the internal state machine, and triggers OLED redraws
 * via the DisplayOLED instance.
 *
 * Callbacks (set via onResetKwh, onWebToggle):
 * - Executed on long-press confirm for Reset kWh / Config toggle.
 * - Run synchronously within the tick() call (should be fast).
 */
class MenuSystem {
public:
    /// Callback signature for the energy reset action. Returns true on success.
    using ResetCallback  = std::function<bool()>;

    /// Callback signature for web server toggle. Receives the new active state.
    using WebToggleCB    = std::function<void(bool active)>;

    /**
     * @brief Construct the menu system.
     * @param btn     Reference to the Button driver.
     * @param display Reference to the OLED display driver.
     */
    MenuSystem(Button& btn, DisplayOLED& display);

    /**
     * @brief Initialise to MONITORING mode with dirty flag set.
     */
    void begin();

    /**
     * @brief Main handler — call this from loop() every iteration.
     *
     * Reads button events, advances the state machine, and redraws the
     * OLED when dirty. Monitoring pages refresh at OLED_REFRESH_INTERVAL_MS.
     *
     * @param state Current SystemState for all render methods.
     */
    void tick(const SystemState& state);

    /**
     * @brief Register the energy reset callback.
     * @param cb Function to call when user confirms Reset kWh.
     */
    void onResetKwh(ResetCallback cb)  { _resetCb  = cb; }

    /**
     * @brief Register the web server toggle callback.
     * @param cb Function to call when user toggles Config → Active/Inactive.
     */
    void onWebToggle(WebToggleCB cb)   { _webCb    = cb; }

    /**
     * @brief Get the current menu mode.
     * @return Current MenuMode enum value.
     */
    MenuMode getMode()       const { return _mode; }

    /**
     * @brief Check if the web server is currently marked active in the menu.
     * @return true if webserver is toggled ON.
     */
    bool     isWebActive()   const { return _webServerOn; }

    /**
     * @brief Update the AP connection info displayed in the Config screen.
     * @param ssid AP SSID.
     * @param pass AP password.
     * @param ip   AP IP address string.
     */
    void     setAPInfo(const char* ssid, const char* pass, const char* ip);

    /**
     * @brief Force a full redraw on the next tick().
     */
    void     requestRedraw() { _dirty = true; }

private:
    Button&       _btn;       ///< Button driver (short/long press)
    DisplayOLED&  _display;   ///< OLED rendering engine

    MenuMode      _mode        = MenuMode::MONITORING;  ///< Current FSM state
    uint8_t       _monPage     = 0;    ///< Active monitoring page (0, 1, 2)
    uint8_t       _cursor      = 0;    ///< Cursor position in current menu
    bool          _webServerOn = false; ///< Toggle state for web server
    bool          _resetSuccess = false; ///< Result of last reset attempt
    bool          _dirty       = true;  ///< Requires OLED redraw

    unsigned long _autoMs      = 0;    ///< millis() for auto-return timers
    unsigned long _rebootMs    = 0;    ///< millis() when reboot countdown started
    int           _rebootSecs  = REBOOT_COUNTDOWN_SEC;  ///< Remaining seconds until reboot

    char _apSSID[32] = DEFAULT_AP_SSID;  ///< Cached AP SSID for Config screen
    char _apPass[32] = DEFAULT_AP_PASS;  ///< Cached AP password
    char _apIP[16]   = "0.0.0.0";        ///< Cached AP IP address

    ResetCallback _resetCb;   ///< Registered energy reset callback
    WebToggleCB   _webCb;     ///< Registered web toggle callback

    unsigned long _lastDisplayMs = 0;  ///< millis() of last OLED refresh

    /**
     * @brief Handle a short button press based on current mode.
     * Typically moves cursor or cycles monitoring page.
     */
    void _handleShortPress();

    /**
     * @brief Handle a long button press based on current mode.
     * Typically confirms selection or enters/leaves a submenu.
     */
    void _handleLongPress();

    /**
     * @brief Check and handle automatic transitions (result timeout, countdown).
     */
    void _checkAutoTransitions();

    /// @name Mode Transitions
    /// @{
    void _toMonitoring();
    void _toMenuMain();
    void _toMenuConfig();
    void _toMenuResetKwh();
    void _toMenuReboot();
    /// @}

    /**
     * @brief Execute the energy reset via callback.
     */
    void _doReset();

    /**
     * @brief Toggle the web server state via callback.
     */
    void _doWebToggle();

    /**
     * @brief Route rendering to the correct draw method based on current mode.
     */
    void _render(const SystemState& state);

    /**
     * @brief Render one of the three monitoring pages.
     */
    void _renderMonitoring(const SystemState& state);

    /**
     * @brief Render one of the menu screens (main/config/reset/reboot).
     */
    void _renderMenu(const SystemState& state);
};
