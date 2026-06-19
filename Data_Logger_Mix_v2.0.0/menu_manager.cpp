#include "menu_manager.h"
#include "diagnostics.h"

/**
 * @file menu_manager.cpp
 * @brief OLED menu state machine driven by a single button (short/long press).
 *
 * State machine summary:
 * ```
 *                    ┌─────────────────────────────────────┐
 *                    │         MONITORING (3 pages)        │
 *                    │  short press → cycle pages 0-1-2    │
 *                    │  long press  → enter MENU_MAIN      │
 *                    └──────────┬──────────────────────────┘
 *                               │ long press
 *                               ▼
 *                    ┌─────────────────────┐
 *                    │      MENU_MAIN      │
 *                    │ 4 items (cursor 0-3)│
 *                    └──┬──┬──┬──┬─────────┘
 *           cursor=0 ───┘  │  │  └── long press → back to MONITORING
 *        cursor=1 ─────────┘  └─── cursor=2
 *                  ▼                 ▼
 *        ┌──────────────┐  ┌───────────────────┐
 *        │ MENU_CONFIG  │  │  MENU_REBOOT      │
 *        │ toggle AP    │  │ confirm/countdown │
 *        └──────────────┘  └───────────────────┘
 *                  ▼                 ▼
 *        ┌─────────────────┐  ┌───────────────────────┐
 *        │ MENU_RESET_KWH  │  │ MENU_REBOOT_CDOWN     │
 *        │ confirm → result│  │ 3-2-1-0 → ESP.restart │
 *        └─────────────────┘  └───────────────────────┘
 * ```
 */

MenuSystem::MenuSystem(Button& btn, DisplayOLED& display)
    : _btn(btn), _display(display)
{}

/**
 * @brief Initialise the menu system to MONITORING mode.
 */
void MenuSystem::begin() {
    _mode  = MenuMode::MONITORING;
    _dirty = true;  // Force initial screen render
    diag.info("MENU", "System ready.");
}

/**
 * @brief Update AP connection info displayed in the Config screen.
 * @param ssid AP SSID.
 * @param pass AP password.
 * @param ip   AP IP address string.
 */
void MenuSystem::setAPInfo(const char* ssid, const char* pass, const char* ip) {
    strncpy(_apSSID, ssid, sizeof(_apSSID) - 1);
    strncpy(_apPass, pass, sizeof(_apPass) - 1);
    strncpy(_apIP,   ip,   sizeof(_apIP)   - 1);
    _dirty = true;
}

/**
 * @brief Main menu handler — call from loop() every iteration.
 *
 * 1. Update button state (debounce + press detection).
 * 2. Handle short and long press events.
 * 3. Check automatic transitions (result auto-return, reboot countdown).
 * 4. Periodic OLED refresh in MONITORING mode (at OLED_REFRESH_INTERVAL_MS).
 * 5. Redraw if dirty.
 *
 * @param state Current SystemState (for all render methods).
 */
void MenuSystem::tick(const SystemState& state) {
    _btn.update();  // Read and debounce button state

    // Process press events
    if (_btn.isShortPressed()) _handleShortPress();
    if (_btn.isLongPressed())  _handleLongPress();

    // Check auto-transitions (reset result timeout, reboot countdown)
    _checkAutoTransitions();

    // In MONITORING mode, refresh display periodically
    if (_mode == MenuMode::MONITORING) {
        unsigned long now = millis();
        if (now - _lastDisplayMs >= OLED_REFRESH_INTERVAL_MS) {
            _lastDisplayMs = now;
            _dirty = true;
        }
    }

    // Redraw if flagged
    if (_dirty) {
        _dirty = false;
        _render(state);
    }
}

/**
 * @brief Handle a short button press (release before longPressMs).
 *
 * Behaviour depends on current mode:
 * - MONITORING: advance to next page (0→1→2→0→...).
 * - MENU_MAIN: move cursor down (0→1→2→3→0→...).
 * - MENU_CONFIG / MENU_RESET_KWH / MENU_REBOOT: toggle cursor (0↔1).
 * - Other modes: no action.
 */
void MenuSystem::_handleShortPress() {
    switch (_mode) {
        case MenuMode::MONITORING:
            _monPage = (_monPage + 1) % 3;  // Cycle 0→1→2→0
            break;
        case MenuMode::MENU_MAIN:
            _cursor = (_cursor + 1) % 4;    // Cycle 0→1→2→3→0
            break;
        case MenuMode::MENU_CONFIG:
        case MenuMode::MENU_RESET_KWH:
        case MenuMode::MENU_REBOOT:
            _cursor = (_cursor + 1) % 2;    // Toggle 0↔1
            break;
        // Result and countdown screens ignore short presses
        case MenuMode::MENU_RESET_RESULT:
        case MenuMode::MENU_REBOOT_CDOWN:
            break;
    }
    _dirty = true;
}

/**
 * @brief Handle a long button press (held ≥ BTN_LONG_PRESS_MS).
 *
 * Enters menus, confirms selections, or navigates back.
 * See the state machine diagram at the top of this file.
 */
void MenuSystem::_handleLongPress() {
    switch (_mode) {
        case MenuMode::MONITORING:
            _toMenuMain();
            break;
        case MenuMode::MENU_MAIN:
            switch (_cursor) {
                case 0: _toMenuConfig();    break;
                case 1: _toMenuResetKwh();  break;
                case 2: _toMenuReboot();    break;
                case 3: _toMonitoring();    break;
            }
            break;
        case MenuMode::MENU_CONFIG:
            if (_cursor == 0) _doWebToggle();  // Toggle Active/Inactive
            else _toMenuMain();                // Back
            break;
        case MenuMode::MENU_RESET_KWH:
            if (_cursor == 0) _doReset();  // Execute energy reset
            else _toMenuMain();            // Back
            break;
        case MenuMode::MENU_REBOOT:
            if (_cursor == 0) {
                // Enter countdown mode (3-2-1-0 → ESP.restart)
                _mode       = MenuMode::MENU_REBOOT_CDOWN;
                _rebootSecs = REBOOT_COUNTDOWN_SEC;
                _rebootMs   = millis();
            } else {
                _toMenuMain();  // Back
            }
            break;
        case MenuMode::MENU_RESET_RESULT:
            _toMenuMain();  // Dismiss result -> main menu
            break;
        case MenuMode::MENU_REBOOT_CDOWN:
            // Countdown in progress — ignore button presses
            break;
    }
    _dirty = true;
}

/**
 * @brief Check and process automatic transitions.
 *
 * - MENU_RESET_RESULT: auto-return to main menu after MENU_AUTO_RETURN_MS.
 * - MENU_REBOOT_CDOWN: decrement countdown every second; reboot at 0.
 *
 * Both are non-blocking (millis()-based, no delay()).
 */
void MenuSystem::_checkAutoTransitions() {
    unsigned long now = millis();
    switch (_mode) {
        case MenuMode::MENU_RESET_RESULT:
            if (now - _autoMs >= MENU_AUTO_RETURN_MS) {
                _toMenuMain();
            }
            break;
        case MenuMode::MENU_REBOOT_CDOWN: {
            int secsElapsed = (now - _rebootMs) / 1000;
            int secsLeft    = REBOOT_COUNTDOWN_SEC - secsElapsed;
            if (secsLeft != _rebootSecs) {
                _rebootSecs = secsLeft;
                _dirty = true;  // Update display on every second change
            }
            if (secsLeft <= 0) {
                diag.info("MENU", "Rebooting...");
                delay(100);  // Allow serial buffer to flush
                ESP.restart();
            }
            break;
        }
        default: break;
    }
}

// ─── Mode Transitions ──────────────────────────────────────────────

void MenuSystem::_toMonitoring() { _mode=MenuMode::MONITORING; _monPage=0; _dirty=true; diag.info("MENU","→ Monitoring"); }
void MenuSystem::_toMenuMain()   { _mode=MenuMode::MENU_MAIN; _cursor=0; _dirty=true; diag.info("MENU","→ Main Menu"); }
void MenuSystem::_toMenuConfig() { _mode=MenuMode::MENU_CONFIG; _cursor=0; _dirty=true; diag.info("MENU","→ Config"); }
void MenuSystem::_toMenuResetKwh() { _mode=MenuMode::MENU_RESET_KWH; _cursor=0; _dirty=true; diag.info("MENU","→ Reset kWh"); }
void MenuSystem::_toMenuReboot() { _mode=MenuMode::MENU_REBOOT; _cursor=0; _dirty=true; diag.info("MENU","→ Reboot"); }

/**
 * @brief Execute the energy reset via the registered callback.
 *
 * The callback is registered in Data_Logger_Mix_v2.0.0.ino and
 * calls DataAcquisition::resetAllEnergy(). The result (true/false)
 * is stored and displayed on the subsequent result screen.
 */
void MenuSystem::_doReset() {
    diag.info("MENU", "Executing energy reset...");
    bool ok = false;
    if (_resetCb) ok = _resetCb();
    _resetSuccess = ok;
    _mode  = MenuMode::MENU_RESET_RESULT;
    _autoMs = millis();  // Start auto-return timer
    _dirty = true;
}

/**
 * @brief Toggle the web server state via the registered callback.
 *
 * The callback is registered in Data_Logger_Mix_v2.0.0.ino and
 * calls WebServerManager::activate() or deactivate().
 * The new state is toggled before calling the callback so the
 * menu display updates immediately.
 */
void MenuSystem::_doWebToggle() {
    _webServerOn = !_webServerOn;
    diag.info("MENU", "Web server: %s", _webServerOn ? "ON" : "OFF");
    if (_webCb) _webCb(_webServerOn);
    _dirty = true;
}

/**
 * @brief Route rendering to the correct handler based on current mode.
 */
void MenuSystem::_render(const SystemState& state) {
    if (_mode == MenuMode::MONITORING) _renderMonitoring(state);
    else _renderMenu(state);
}

/**
 * @brief Render the active monitoring page (0/1/2).
 */
void MenuSystem::_renderMonitoring(const SystemState& state) {
    switch (_monPage) {
        case 0: _display.drawMonitorPage1(state); break;
        case 1: _display.drawMonitorPage2(state); break;
        case 2: _display.drawMonitorPage3(state); break;
    }
}

/**
 * @brief Render the active menu screen based on current mode.
 */
void MenuSystem::_renderMenu(const SystemState& state) {
    bool wOk = state.wifiConnected;
    bool mOk = state.mqttConnected;
    switch (_mode) {
        case MenuMode::MENU_MAIN:
            _display.drawMenuMain(_cursor, wOk, mOk);
            break;
        case MenuMode::MENU_CONFIG:
            _display.drawMenuConfig(_cursor, _webServerOn,
                _apSSID, _apPass, _apIP, wOk, mOk);
            break;
        case MenuMode::MENU_RESET_KWH:
            _display.drawMenuResetKwh(_cursor, wOk, mOk);
            break;
        case MenuMode::MENU_RESET_RESULT:
            _display.drawMenuResetResult(_resetSuccess, wOk, mOk);
            break;
        case MenuMode::MENU_REBOOT:
            _display.drawMenuReboot(_cursor, wOk, mOk);
            break;
        case MenuMode::MENU_REBOOT_CDOWN:
            _display.drawMenuRebootCountdown(_rebootSecs, wOk, mOk);
            break;
        default: break;
    }
}
