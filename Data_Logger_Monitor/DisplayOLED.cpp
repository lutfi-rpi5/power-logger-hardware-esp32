#include "DisplayOLED.h"

DisplayOLED::DisplayOLED(uint8_t address)
  : display(OLED_WIDTH, OLED_HEIGHT, &Wire, -1), page(0), lastUpdate(0), autoScroll(true)
{}

void DisplayOLED::begin() {
  Wire.begin(); // default SDA=21 SCL=22, override di main kalau perlu
  if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("[OLED] SSD1306 allocation failed");
    return;
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("Power Logger OLED init");
  display.display();
  delay(500);
  display.clearDisplay();
}

void DisplayOLED::setPage(uint8_t p) {
  page = p % 3;
  // paksa refresh berikutnya
  lastUpdate = 0;
}

void DisplayOLED::enableAutoScroll(bool en) {
  autoScroll = en;
}

const char* DisplayOLED::statusToText(int st) {
  switch(st) {
    case 0: return "OK";
    case 1: return "LOW";
    default: return "ERR";
  }
}

void DisplayOLED::update(float v[3], float i[3], float p[3], float pf[3], float f[3], float e[3], int status[3]) {
  unsigned long now = millis();
  if (now - lastUpdate < interval) return;
  lastUpdate = now;

  display.clearDisplay();
  display.setCursor(0,0);

  switch(page) {
    case 0: drawPage1(v, i); break;
    case 1: drawPage2(p, pf, f); break;
    case 2: drawPage3(e, status); break;
    default: drawPage1(v,i); break;
  }

  display.display();

  // hanya auto-advance jika diijinkan
  if (autoScroll) {
    page = (page + 1) % 3;
  }
}

void DisplayOLED::drawPage1(float v[3], float i[3]) {
  display.setTextSize(1);
  display.println("    V     I");
  for (int l = 0; l < 3; ++l) {
    // rapiin agar muat 128x64
    char buf[32];
    snprintf(buf, sizeof(buf), "L%d %3.0fV %4.1fA", l+1, v[l], i[l]);
    display.println(buf);
  }
}

void DisplayOLED::drawPage2(float p[3], float pf[3], float f[3]) {
  display.setTextSize(1);
  display.println("P (W)    PF    F(Hz)");
  for (int l = 0; l < 3; ++l) {
    char buf[40];
    snprintf(buf, sizeof(buf), "L%d %5.0fW  %.2f  %.1f", l+1, p[l], pf[l], f[l]);
    display.println(buf);
  }
}

void DisplayOLED::drawPage3(float e[3], int status[3]) {
  display.setTextSize(1);
  display.println("E(kWh)    STATUS");
  for (int l = 0; l < 3; ++l) {
    char buf[40];
    // tampilkan energy dalam kWh (assume e[] dalam Wh, jika beda sesuaikan)
    snprintf(buf, sizeof(buf), "L%d %.2fkWh  %s", l+1, e[l]/1000.0, statusToText(status[l]));
    display.println(buf);
  }
}
