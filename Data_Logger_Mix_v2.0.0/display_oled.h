#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "config.h"
#include "types.h"

/**
 * @file display_oled.h
 * @brief SSD1306 128×64 OLED display driver for monitoring pages and menus.
 *
 * Provides rendering for:
 * - 3 monitoring pages (Voltage/Current, Frequency/Power, PF/Energy).
 * - Boot progress screen.
 * - 4-item menu with config/reset/reboot screens.
 *
 * Layout constants (all in pixels, referenced from top-left):
 * - Title bar: row 0, Wi-Fi/MQTT status icons in top-right corner.
 * - Separator line: row 9.
 * - Phase headers (R / S / T): row 11, centred in three columns.
 * - Data rows: 19, 27, 35, 43, 51.
 * - Column centres: COL0_CX=21, COL1_CX=64, COL2_CX=107.
 *
 * Font: Adafruit 5×7 monospaced (1 char = 6 pixels wide including spacing).
 * Column centres are calculated for 7-character strings: the midpoints
 * of three equal divisions of the 128-pixel width.
 */

/// @name Row Y-positions (pixels from top)
/// @{
#define ROW_TITLE   0   ///< Title bar row
#define ROW_SEP     9   ///< Separator line Y
#define ROW_0      11   ///< Phase headers row
#define ROW_1      19   ///< Data row 1
#define ROW_2      27   ///< Data row 2
#define ROW_3      35   ///< Data row 3
#define ROW_4      43   ///< Data row 4 (unbalance line or menu)
#define ROW_5      51   ///< Data row 5 (reserved)
/// @}

/// @name Column X-centres (pixels from left)
/// Columns are at 21, 64, 107 — the midpoints of three 42-pixel
/// segments: [0–42], [43–85], [86–127]. Each column comfortably
/// fits a 7-character string (7 × 6 = 42 px).
/// @{
#define COL0_CX    21   ///< Phase R column centre
#define COL1_CX    64   ///< Phase S column centre
#define COL2_CX   107   ///< Phase T column centre
/// @}

/**
 * @class DisplayOLED
 * @brief SSD1306 OLED rendering engine for monitoring and menu screens.
 *
 * All draw methods accept the current SystemState for data and connection
 * status. The display is double-buffered — call draw*() methods then
 * commit() to flush the buffer to the display hardware.
 *
 * Thread safety: Not guaranteed. Must be called only from Core 1 (loop
 * task) where the menu manager orchestrates rendering.
 */
class DisplayOLED {
public:
    /**
     * @brief Construct the OLED display driver.
     * @param i2cAddr I2C address of the SSD1306 (default: 0x3C).
     */
    explicit DisplayOLED(uint8_t i2cAddr = OLED_I2C_ADDR);

    /**
     * @brief Initialise I2C and the display controller.
     * Shows the boot screen at 0% progress after init.
     */
    void begin();

    /// @name Monitoring Pages
    /// @{
    /**
     * @brief Monitoring Page 1: Voltage, Current, Status, Unbalance.
     * Shows per-phase V/I readings with auto-scaled units.
     * Status text blinks when LOST/UNDER/OVER.
     * [!] indicator blinks when unbalance exceeds threshold.
     */
    void drawMonitorPage1(const SystemState& s);

    /**
     * @brief Monitoring Page 2: Frequency, Apparent Power, Reactive Power.
     * Auto-scales VA ↔ kVA and VAr ↔ kVAr at 1000 threshold.
     */
    void drawMonitorPage2(const SystemState& s);

    /**
     * @brief Monitoring Page 3: Power Factor, Active Power, Energy.
     * Auto-scales W ↔ kW and Wh ↔ kWh at 1000 threshold.
     */
    void drawMonitorPage3(const SystemState& s);
    /// @}

    /// @name Menu Pages
    /// @{
    /**
     * @brief Main menu with 4 items: Config, Reset kWh, Reboot, Back.
     * @param cursor Currently selected item (0–3).
     */
    void drawMenuMain(int cursor, bool wifiOk, bool mqttOk);

    /**
     * @brief Web config toggle screen with AP info.
     * @param cursor 0 = toggle, 1 = back.
     * @param active Current webserver state (Active/Inactive).
     */
    void drawMenuConfig(int cursor, bool active,
                        const char* ssid, const char* pass, const char* ip,
                        bool wifiOk, bool mqttOk);

    /**
     * @brief Reset kWh confirmation screen.
     * @param cursor 0 = Reset, 1 = Back.
     */
    void drawMenuResetKwh(int cursor, bool wifiOk, bool mqttOk);

    /**
     * @brief Reset kWh result screen (shown for 2 s, auto-return).
     * @param success true if reset succeeded.
     */
    void drawMenuResetResult(bool success, bool wifiOk, bool mqttOk);

    /**
     * @brief Reboot confirmation screen.
     * @param cursor 0 = Reboot, 1 = Back.
     */
    void drawMenuReboot(int cursor, bool wifiOk, bool mqttOk);

    /**
     * @brief Reboot countdown screen (3-2-1-0 then resets).
     * @param secsLeft Seconds remaining before reboot.
     */
    void drawMenuRebootCountdown(int secsLeft, bool wifiOk, bool mqttOk);
    /// @}

    /**
     * @brief Draw the boot progress bar.
     * @param progressPercent 0–100.
     */
    void drawBoot(int progressPercent);

    /**
     * @brief Clear the display buffer (black).
     */
    void clear();

    /**
     * @brief Flush the display buffer to the SSD1306 hardware.
     */
    void commit();

private:
    Adafruit_SSD1306 _disp;  ///< Adafruit display driver instance

    /**
     * @brief Clear the display buffer (internal, no commit).
     */
    void _clear();

    /**
     * @brief Draw the title bar with Wi-Fi (W) and MQTT (Q) icons.
     * @param title  Title string (e.g. "3Ph Data Logger").
     */
    void _header(const char* title, bool wifiOk, bool mqttOk);

    /**
     * @brief Draw a horizontal separator line.
     */
    void _separator();

    /**
     * @brief Draw three phase column headers (R, S, T).
     */
    void _phaseHeaders();

    /**
     * @brief Print a string centred on an X coordinate.
     * @param cx Centre X in pixels.
     * @param y  Y position in pixels.
     * @param str Null-terminated string to print.
     */
    void _printCX(int16_t cx, int16_t y, const char* str);

    /**
     * @brief Print a string in one of the three phase columns.
     * @param col Column index (0, 1, 2).
     * @param y   Y position.
     * @param str Null-terminated string to print.
     */
    void _printCol(uint8_t col, int16_t y, const char* str);

    /// @name Formatting Helpers
    /// Each formats a float into a short string with auto-scaling.
    /// Buffer must be at least 10 bytes.
    /// @{
    static void _fmtVoltage (char* b, float v);  ///< "---V" or "220V"
    static void _fmtCurrent (char* b, float i);  ///< "10.00A" or "12.3A"
    static void _fmtPower   (char* b, float w);  ///< "1000W" or "1.5kW"
    static void _fmtApparent(char* b, float va); ///< "1000VA" or "1.5kVA"
    static void _fmtReactive(char* b, float var);///< "1000VAr" or "1.5kVAr"
    static void _fmtEnergy  (char* b, float wh); ///< "1000Wh" or "1.50kWh"
    static void _fmtFreq    (char* b, float hz); ///< "50.0Hz"
    static void _fmtPF      (char* b, float pf); ///< "0.97PF"
    /// @}

    /**
     * @brief Draw a menu line with optional selection cursor.
     * @param y        Y position.
     * @param selected true prepends "> ".
     * @param text     Menu item text.
     */
    void _menuLine(int16_t y, bool selected, const char* text);

    /**
     * @brief Draw a two-option confirmation row with cursor.
     * @param y      Y position.
     * @param cursor 0 = select left, 1 = select right.
     * @param left   Left option text.
     * @param right  Right option text.
     */
    void _confirmRow(int16_t y, int cursor, const char* left, const char* right);
};
