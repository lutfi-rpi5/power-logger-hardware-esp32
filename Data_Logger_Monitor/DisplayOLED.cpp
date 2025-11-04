#include "DisplayOLED.h"

DisplayOLED::DisplayOLED() : display(OLED_WIDTH, OLED_HEIGHT, &Wire, -1) {}

void DisplayOLED::begin() {
    display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Power Monitor Init...");
    display.display();
    delay(700);
}

const char* DisplayOLED::statusToText(int st) {
    switch(st){
        case 0: return "OK";
        case 1: return "LOW V";
        default: return "ERROR";
    }
}

void DisplayOLED::update(float v[3], float i[3], float p[3], float pf[3], float f[3], float e[3], int status[3]) {
    if (millis() - lastUpdate < interval) return;
    lastUpdate = millis();

    display.clearDisplay();
    display.setCursor(0,0);

    switch(page) {
        case 0: drawPage1(v, i); break;
        case 1: drawPage2(p, pf, f); break;
        case 2: drawPage3(e, status); break;
    }

    display.display();
    page = (page + 1) % 3;
}

void DisplayOLED::drawPage1(float v[3], float i[3]) {
    display.println("V / I");
    for(int l=0; l<3; l++){
        display.printf("L%d %3.0fV %2.0fA\n", l+1, v[l], i[l]);
    }
}

void DisplayOLED::drawPage2(float p[3], float pf[3], float f[3]) {
    display.println("P / PF / F");
    for(int l=0; l<3; l++){
        display.printf("L%d %4.0fW %.2f %.1fHz\n", l+1, p[l], pf[l], f[l]);
    }
}

void DisplayOLED::drawPage3(float e[3], int status[3]) {
    display.println("Energy / Status");
    for(int l=0; l<3; l++){
        display.printf("L%d %.2fkWh %s\n", l+1, e[l]/1000.0, statusToText(status[l]));
    }
}
