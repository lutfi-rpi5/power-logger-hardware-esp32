#include "menu_manager.h"
#include "diagnostics.h"

MenuSystem::MenuSystem(Button& btn, DisplayOLED& display)
    : _btn(btn), _display(display)
{}

void MenuSystem::begin() {
    _mode  = MenuMode::MONITORING;
    _dirty = true;
    diag.info("MENU", "System ready.");
}

void MenuSystem::setAPInfo(const char* ssid, const char* pass, const char* ip) {
    strncpy(_apSSID, ssid, sizeof(_apSSID) - 1);
    strncpy(_apPass, pass, sizeof(_apPass) - 1);
    strncpy(_apIP,   ip,   sizeof(_apIP)   - 1);
    _dirty = true;
}

void MenuSystem::tick(const SystemState& state) {
    _btn.update();

    if (_btn.isShortPressed()) _handleShortPress();
    if (_btn.isLongPressed())  _handleLongPress();

    _checkAutoTransitions();

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

void MenuSystem::_handleShortPress() {
    switch (_mode) {
        case MenuMode::MONITORING:
            _monPage = (_monPage + 1) % 3;
            break;
        case MenuMode::MENU_MAIN:
            _cursor = (_cursor + 1) % 4;
            break;
        case MenuMode::MENU_CONFIG:
        case MenuMode::MENU_RESET_KWH:
        case MenuMode::MENU_REBOOT:
            _cursor = (_cursor + 1) % 2;
            break;
        case MenuMode::MENU_RESET_RESULT:
        case MenuMode::MENU_REBOOT_CDOWN:
            break;
    }
    _dirty = true;
}

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
            if (_cursor == 0) _doWebToggle();
            else _toMenuMain();
            break;
        case MenuMode::MENU_RESET_KWH:
            if (_cursor == 0) _doReset();
            else _toMenuMain();
            break;
        case MenuMode::MENU_REBOOT:
            if (_cursor == 0) {
                _mode       = MenuMode::MENU_REBOOT_CDOWN;
                _rebootSecs = REBOOT_COUNTDOWN_SEC;
                _rebootMs   = millis();
            } else {
                _toMenuMain();
            }
            break;
        case MenuMode::MENU_RESET_RESULT:
            _toMenuMain();
            break;
        case MenuMode::MENU_REBOOT_CDOWN:
            break;
    }
    _dirty = true;
}

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
                _dirty = true;
            }
            if (secsLeft <= 0) {
                diag.info("MENU", "Rebooting...");
                delay(100);
                ESP.restart();
            }
            break;
        }
        default: break;
    }
}

void MenuSystem::_toMonitoring() { _mode=MenuMode::MONITORING; _monPage=0; _dirty=true; diag.info("MENU","→ Monitoring"); }
void MenuSystem::_toMenuMain()   { _mode=MenuMode::MENU_MAIN; _cursor=0; _dirty=true; diag.info("MENU","→ Main Menu"); }
void MenuSystem::_toMenuConfig() { _mode=MenuMode::MENU_CONFIG; _cursor=0; _dirty=true; diag.info("MENU","→ Config"); }
void MenuSystem::_toMenuResetKwh() { _mode=MenuMode::MENU_RESET_KWH; _cursor=0; _dirty=true; diag.info("MENU","→ Reset kWh"); }
void MenuSystem::_toMenuReboot() { _mode=MenuMode::MENU_REBOOT; _cursor=0; _dirty=true; diag.info("MENU","→ Reboot"); }

void MenuSystem::_doReset() {
    diag.info("MENU", "Executing energy reset...");
    bool ok = false;
    if (_resetCb) ok = _resetCb();
    _resetSuccess = ok;
    _mode  = MenuMode::MENU_RESET_RESULT;
    _autoMs = millis();
    _dirty = true;
}

void MenuSystem::_doWebToggle() {
    _webServerOn = !_webServerOn;
    diag.info("MENU", "Web server: %s", _webServerOn ? "ON" : "OFF");
    if (_webCb) _webCb(_webServerOn);
    _dirty = true;
}

void MenuSystem::_render(const SystemState& state) {
    if (_mode == MenuMode::MONITORING) _renderMonitoring(state);
    else _renderMenu(state);
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
