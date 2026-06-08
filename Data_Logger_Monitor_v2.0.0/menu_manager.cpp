#include "menu_manager.h"
#include "config.h"
#include "diagnostics.h"

static const char* MAIN_MENU_ITEMS[] = {
    "Config",
    "Reset kWh",
    "Reboot Device",
    "Reset to Factory",
    "Back to Monitoring"
};
static const int MAIN_MENU_COUNT = 5;

MenuManager::MenuManager()
    : _display(nullptr)
    , _mode(OLEDMode::MONITOR_PAGE_1)
    , _cursor(0)
    , _subCursor(0)
    , _rebootFlag(false)
    , _resetEnergyFlag(false)
    , _factoryResetFlag(false)
    , _toggleAPFlag(false)
    , _rebootCountdownActive(false)
    , _rebootCountdownSec(3)
{
    strlcpy(_apSSID, AP_SSID, sizeof(_apSSID));
    strlcpy(_apPass, AP_PASS, sizeof(_apPass));
    strlcpy(_apIP, "192.168.4.1", sizeof(_apIP));
}

void MenuManager::begin(DisplayManager* display) {
    _display = display;
}

void MenuManager::setAPConfig(const char* ssid, const char* pass, const char* ip) {
    strlcpy(_apSSID, ssid, sizeof(_apSSID));
    strlcpy(_apPass, pass, sizeof(_apPass));
    strlcpy(_apIP, ip, sizeof(_apIP));
}

// ─── Main update (called from loop) ─────────────────────────

void MenuManager::update(bool wifiOn, bool mqttOn,
                          const PhaseData phases[3], float unbalance,
                          const ThresholdData& thr,
                          bool apActive, const char* apIP,
                          bool rebootCountdownActive, int rebootCountdownSec)
{
    _rebootCountdownActive = rebootCountdownActive;
    _rebootCountdownSec = rebootCountdownSec;

    if (apIP && strlen(apIP) > 0) {
        strlcpy(_apIP, apIP, sizeof(_apIP));
    }

    _render(wifiOn, mqttOn, phases, unbalance, thr, apActive);
}

// ─── Rendering ──────────────────────────────────────────────

void MenuManager::_render(bool wifiOn, bool mqttOn,
                           const PhaseData phases[3], float unbalance,
                           const ThresholdData& thr, bool apActive)
{
    _display->clear();

    uint8_t m = (uint8_t)_mode;

    if (m <= 2) {
        // Monitoring mode (pages 0-2)
        _renderMonitorPage(wifiOn, mqttOn, phases, unbalance, thr);
    } else {
        // Menu mode
        switch (_mode) {
            case OLEDMode::MENU_MAIN:
                _renderMenuMain(wifiOn, mqttOn);
                break;
            case OLEDMode::MENU_CONFIG:
                _renderMenuConfig(apActive, wifiOn, mqttOn);
                break;
            case OLEDMode::MENU_RESET_KWH:
                _renderMenuResetKWH(wifiOn, mqttOn);
                break;
            case OLEDMode::MENU_REBOOT:
                _renderMenuReboot(wifiOn, mqttOn);
                break;
            case OLEDMode::MENU_FACTORY:
                _renderMenuFactory(wifiOn, mqttOn);
                break;
            default:
                _renderMenuMain(wifiOn, mqttOn);
                break;
        }
    }

    _display->display();
}

void MenuManager::_renderMonitorPage(bool wifiOn, bool mqttOn,
                                      const PhaseData phases[3], float unbalance,
                                      const ThresholdData& thr)
{
    _display->drawTitleBar("3-Phase Data Logger", wifiOn, mqttOn);

    switch (_mode) {
        case OLEDMode::MONITOR_PAGE_1:
            _display->drawMonitorPage1(phases, unbalance, thr);
            break;
        case OLEDMode::MONITOR_PAGE_2:
            _display->drawMonitorPage2(phases);
            break;
        case OLEDMode::MONITOR_PAGE_3:
            _display->drawMonitorPage3(phases);
            break;
        default:
            _display->drawMonitorPage1(phases, unbalance, thr);
            break;
    }
}

void MenuManager::_renderMenuMain(bool wifiOn, bool mqttOn) {
    _display->drawMenuTitle("3-Phase Data Logger", wifiOn, mqttOn);
    _display->drawMenuItems(MAIN_MENU_ITEMS, MAIN_MENU_COUNT, _cursor, 1);
}

void MenuManager::_renderMenuConfig(bool apActive, bool wifiOn, bool mqttOn) {
    _display->drawMenuTitle("CONFIGURATION", wifiOn, mqttOn);
    _display->drawConfigPage(_apSSID, _apPass, _apIP, apActive, _subCursor == 0);
}

void MenuManager::_renderMenuResetKWH(bool wifiOn, bool mqttOn) {
    _display->drawMenuTitle("CONFIGURATION", wifiOn, mqttOn);
    static bool showResult = false;
    // We render confirmation by default; result is shown after action
    if (_resetEnergyFlag) {
        _display->drawStatusMessage("    Reset kWh", "    Success");
    } else {
        _display->drawConfirmPrompt("Are you sure to", "  Reset kWh?",
                                     "Reset", "Back", _subCursor == 0);
    }
}

void MenuManager::_renderMenuReboot(bool wifiOn, bool mqttOn) {
    _display->drawMenuTitle("CONFIGURATION", wifiOn, mqttOn);
    if (_rebootCountdownActive) {
        char buf[32];
        snprintf(buf, sizeof(buf), "  in %d seconds", _rebootCountdownSec);
        _display->drawStatusMessage(" Device Rebooting", buf);
    } else {
        _display->drawConfirmPrompt("Are you sure to", "  Reboot Device?",
                                     "Reboot", "Back", _subCursor == 0);
    }
}

void MenuManager::_renderMenuFactory(bool wifiOn, bool mqttOn) {
    _display->drawMenuTitle("CONFIGURATION", wifiOn, mqttOn);
    if (_factoryResetFlag) {
        _display->drawStatusMessage(" Reset to Factory", "    Success");
    } else {
        _display->drawConfirmPrompt("Are you sure to", "  Reset Device?",
                                     "Reset", "Back", _subCursor == 0);
    }
}

// ─── Button Handlers ────────────────────────────────────────

void MenuManager::onShortPress() {
    uint8_t m = (uint8_t)_mode;

    if (m <= 2) {
        // Monitoring: cycle pages
        _mode = (OLEDMode)(((int)_mode + 1) % 3);
        diag.debug("MENU", "Monitor page -> %d", (int)_mode);
        return;
    }

    // Menu mode: move cursor
    switch (_mode) {
        case OLEDMode::MENU_MAIN:
            _cursor = (_cursor + 1) % MAIN_MENU_COUNT;
            break;
        case OLEDMode::MENU_CONFIG:
            _subCursor = (_subCursor + 1) % 2;  // Active <-> Back
            break;
        case OLEDMode::MENU_RESET_KWH:
            _subCursor = (_subCursor + 1) % 2;  // Reset <-> Back
            break;
        case OLEDMode::MENU_REBOOT:
            _subCursor = (_subCursor + 1) % 2;  // Reboot <-> Back
            break;
        case OLEDMode::MENU_FACTORY:
            _subCursor = (_subCursor + 1) % 2;  // Reset <-> Back
            break;
        default:
            break;
    }
}

void MenuManager::onLongPress() {
    uint8_t m = (uint8_t)_mode;

    if (m <= 2) {
        // Monitoring: enter menu
        _enterMenu();
        return;
    }

    // Menu mode actions
    switch (_mode) {
        case OLEDMode::MENU_MAIN:
            _executeMainAction(_cursor);
            break;
        case OLEDMode::MENU_CONFIG:
            if (_subCursor == 0) {
                // Toggle AP
                _toggleAPFlag = true;
            } else {
                _exitMenu();
            }
            break;
        case OLEDMode::MENU_RESET_KWH:
            if (_subCursor == 0) {
                _resetEnergyFlag = true;
            } else {
                _exitMenu();
            }
            break;
        case OLEDMode::MENU_REBOOT:
            if (_subCursor == 0) {
                _rebootFlag = true;
            } else {
                _exitMenu();
            }
            break;
        case OLEDMode::MENU_FACTORY:
            if (_subCursor == 0) {
                _factoryResetFlag = true;
            } else {
                _exitMenu();
            }
            break;
        default:
            break;
    }
}

// ─── Internal Navigation ────────────────────────────────────

void MenuManager::_enterMenu() {
    _mode = OLEDMode::MENU_MAIN;
    _cursor = 0;
    _subCursor = 0;
    diag.info("MENU", "Entered menu mode");
}

void MenuManager::_exitMenu() {
    _mode = OLEDMode::MONITOR_PAGE_1;
    _cursor = 0;
    _subCursor = 0;
    diag.info("MENU", "Returned to monitoring");
}

void MenuManager::_executeMainAction(int idx) {
    switch (idx) {
        case 0: _mode = OLEDMode::MENU_CONFIG;  _subCursor = 0; break;
        case 1: _mode = OLEDMode::MENU_RESET_KWH; _subCursor = 0; break;
        case 2: _mode = OLEDMode::MENU_REBOOT;  _subCursor = 0; break;
        case 3: _mode = OLEDMode::MENU_FACTORY;  _subCursor = 0; break;
        case 4: _exitMenu(); break;
    }
}

// ─── State Queries ──────────────────────────────────────────

OLEDMode MenuManager::getCurrentMode() const {
    return _mode;
}

bool MenuManager::isInMenu() const {
    return (uint8_t)_mode > 2;
}

bool MenuManager::shouldReboot() const {
    return _rebootFlag;
}

bool MenuManager::shouldResetEnergy() const {
    return _resetEnergyFlag;
}

void MenuManager::clearResetEnergyFlag() {
    _resetEnergyFlag = false;
}

bool MenuManager::shouldFactoryReset() const {
    return _factoryResetFlag;
}

void MenuManager::clearFactoryResetFlag() {
    _factoryResetFlag = false;
}

bool MenuManager::shouldToggleAP() const {
    return _toggleAPFlag;
}

void MenuManager::clearToggleAPFlag() {
    _toggleAPFlag = false;
}
