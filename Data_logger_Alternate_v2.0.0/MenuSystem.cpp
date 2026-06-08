/******************************************************
 * File      : MenuSystem.cpp
 ******************************************************/

#include "MenuSystem.h"

MenuSystem::MenuSystem(Button& btn, DisplayOLED& display)
    : _btn(btn), _display(display)
{}

void MenuSystem::begin() {
    _mode  = MenuMode::MONITORING;
    _dirty = true;
    Serial.println("[Menu] System ready.");
}

void MenuSystem::setAPInfo(const char* ssid, const char* pass, const char* ip) {
    strncpy(_apSSID, ssid, sizeof(_apSSID) - 1);
    strncpy(_apPass, pass, sizeof(_apPass) - 1);
    strncpy(_apIP,   ip,   sizeof(_apIP)   - 1);
    _dirty = true;
}

// ─── Main tick ────────────────────────────────────────────

void MenuSystem::tick(const SystemState& state) {
    _btn.update();

    if (_btn.isShortPressed()) _handleShortPress();
    if (_btn.isLongPressed())  _handleLongPress();

    _checkAutoTransitions();

    // Monitoring: periodic refresh even without button events
    if (_mode == MenuMode::MONITORING) {
        unsigned long now = millis();
        if (now - _lastDisplayMs >= OLED_REFRESH_INTERVAL_MS) {
            _lastDisplayMs = now;
            _dirty = true;
        }
    }

    if (_dirty) {
        _dirty = false;
        _render(state);
    }
}

// ─── Button Handlers ──────────────────────────────────────

void MenuSystem::_handleShortPress() {
    switch (_mode) {
        // Monitoring: short press cycles pages
        case MenuMode::MONITORING:
            _monPage = (_monPage + 1) % 3;
            break;

        // Main menu: cursor moves down, wraps
        case MenuMode::MENU_MAIN:
            _cursor = (_cursor + 1) % 4;   // 4 items
            break;

        // Config: cursor toggles between 2 options
        case MenuMode::MENU_CONFIG:
            _cursor = (_cursor + 1) % 2;
            break;

        // Reset confirm: cursor toggles
        case MenuMode::MENU_RESET_KWH:
            _cursor = (_cursor + 1) % 2;
            break;

        // Reboot confirm: cursor toggles
        case MenuMode::MENU_REBOOT:
            _cursor = (_cursor + 1) % 2;
            break;

        // Auto-transition screens: ignore input
        case MenuMode::MENU_RESET_RESULT:
        case MenuMode::MENU_REBOOT_CDOWN:
            break;
    }
    _dirty = true;
}

void MenuSystem::_handleLongPress() {
    switch (_mode) {
        // Monitoring → main menu
        case MenuMode::MONITORING:
            _toMenuMain();
            break;

        // Main menu: enter selected item
        case MenuMode::MENU_MAIN:
            switch (_cursor) {
                case 0: _toMenuConfig();    break;
                case 1: _toMenuResetKwh();  break;
                case 2: _toMenuReboot();    break;
                case 3: _toMonitoring();    break;
            }
            break;

        // Config menu
        case MenuMode::MENU_CONFIG:
            if (_cursor == 0) {
                _doWebToggle();
            } else {
                _toMenuMain();
            }
            break;

        // Reset kWh confirm
        case MenuMode::MENU_RESET_KWH:
            if (_cursor == 0) {
                _doReset();
            } else {
                _toMenuMain();
            }
            break;

        // Reboot confirm
        case MenuMode::MENU_REBOOT:
            if (_cursor == 0) {
                // Start reboot countdown
                _mode       = MenuMode::MENU_REBOOT_CDOWN;
                _rebootSecs = REBOOT_COUNTDOWN_SEC;
                _rebootMs   = millis();
            } else {
                _toMenuMain();
            }
            break;

        // Auto-transition screens: long press = skip to main menu / just reboot
        case MenuMode::MENU_RESET_RESULT:
            _toMenuMain();
            break;

        case MenuMode::MENU_REBOOT_CDOWN:
            // Ignore – countdown is running
            break;
    }
    _dirty = true;
}

// ─── Auto Transitions ─────────────────────────────────────

void MenuSystem::_checkAutoTransitions() {
    unsigned long now = millis();

    switch (_mode) {
        // After showing reset result, return to menu after 2 seconds
        case MenuMode::MENU_RESET_RESULT:
            if (now - _autoMs >= MENU_AUTO_RETURN_MS) {
                _toMenuMain();
            }
            break;

        // Reboot countdown: update display each second, restart at 0
        case MenuMode::MENU_REBOOT_CDOWN: {
            int secsElapsed = (now - _rebootMs) / 1000;
            int secsLeft    = REBOOT_COUNTDOWN_SEC - secsElapsed;
            if (secsLeft != _rebootSecs) {
                _rebootSecs = secsLeft;
                _dirty = true;
            }
            if (secsLeft <= 0) {
                Serial.println("[Menu] Rebooting...");
                delay(100);
                ESP.restart();
            }
            break;
        }

        default: break;
    }
}

// ─── State Transitions ────────────────────────────────────

void MenuSystem::_toMonitoring() {
    _mode    = MenuMode::MONITORING;
    _monPage = 0;
    _dirty   = true;
    Serial.println("[Menu] → Monitoring");
}

void MenuSystem::_toMenuMain() {
    _mode   = MenuMode::MENU_MAIN;
    _cursor = 0;
    _dirty  = true;
    Serial.println("[Menu] → Main Menu");
}

void MenuSystem::_toMenuConfig() {
    _mode   = MenuMode::MENU_CONFIG;
    _cursor = 0;
    _dirty  = true;
    Serial.println("[Menu] → Config");
}

void MenuSystem::_toMenuResetKwh() {
    _mode   = MenuMode::MENU_RESET_KWH;
    _cursor = 0;
    _dirty  = true;
    Serial.println("[Menu] → Reset kWh");
}

void MenuSystem::_toMenuReboot() {
    _mode   = MenuMode::MENU_REBOOT;
    _cursor = 0;
    _dirty  = true;
    Serial.println("[Menu] → Reboot");
}

void MenuSystem::_doReset() {
    Serial.println("[Menu] Executing energy reset...");
    bool ok = false;
    if (_resetCb) ok = _resetCb();
    _resetSuccess = ok;
    _mode         = MenuMode::MENU_RESET_RESULT;
    _autoMs       = millis();
    _dirty        = true;
}

void MenuSystem::_doWebToggle() {
    _webServerOn = !_webServerOn;
    Serial.printf("[Menu] Web server: %s\n", _webServerOn ? "ON" : "OFF");
    if (_webCb) _webCb(_webServerOn);
    _dirty = true;
}

// ─── Rendering ────────────────────────────────────────────

void MenuSystem::_render(const SystemState& state) {
    bool wOk = state.wifiConnected;
    bool mOk = state.mqttConnected;

    if (_mode == MenuMode::MONITORING) {
        _renderMonitoring(state);
    } else {
        _renderMenu(state);
    }
}

void MenuSystem::_renderMonitoring(const SystemState& state) {
    switch (_monPage) {
        case 0: _display.drawMonitorPage1(state); break;
        case 1: _display.drawMonitorPage2(state); break;
        case 2: _display.drawMonitorPage3(state); break;
    }
}

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
