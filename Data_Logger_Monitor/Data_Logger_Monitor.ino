#include <WiFi.h>
// #include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <PZEM004Tv30.h>
#include "credentials.h"
#include "ResetEnergy.h"
#include "WifiSelector.h"
// #include "credentials_example.h"

// ===================== WiFi ======================
WiFiClient espClient;

// ============ Instance WiFiSelector ==============
WiFiSelector wifiManager(10000);  // timeout 10 detik

// ===================== MQTT ======================
PubSubClient client(espClient);

// ==================== Millis =====================
unsigned long lastMsg = 0;

// ================== PZEM CONFIG ==================
#define PZEM_RX1_PIN 4
#define PZEM_TX1_PIN 15
#define PZEM_RX2_PIN 17
#define PZEM_TX2_PIN 16
// #define PZEM_SERIAL Serial2
HardwareSerial PZEMSerial1(1);
HardwareSerial PZEMSerial2(2);
#define NUM_PZEMS 3   // Ubah sesuai jumlah PZEM

// Inisialisasi PZEM dengan alamat unik
PZEM004Tv30 pzems[NUM_PZEMS] = {
  PZEM004Tv30(PZEMSerial1, PZEM_RX1_PIN, PZEM_TX1_PIN, 0x10), // Line R / PZEM 1
  PZEM004Tv30(PZEMSerial1, PZEM_RX1_PIN, PZEM_TX1_PIN, 0x11), // Line S / PZEM 2
  PZEM004Tv30(PZEMSerial2, PZEM_RX2_PIN, PZEM_TX2_PIN, 0x12)  // Line T / PZEM 3
};

// Reset Energy Initial
#define RESET_ENERGY_BUTTON_PIN  5
#define RESET_ENERGY_LED_PIN     2   // LED internal ESP32 (GPIO 2)/18
ResetEnergy resetEnergy(RESET_ENERGY_BUTTON_PIN, RESET_ENERGY_LED_PIN);

// ================== MQTT TOPICS ==================
// Voltage (V)
const char* workshop_v[]        = {"lutpiii/workshop/voltage/l1",        "lutpiii/workshop/voltage/l2",        "lutpiii/workshop/voltage/l3"};
// Current (A)
const char* workshop_i[]        = {"lutpiii/workshop/current/l1",        "lutpiii/workshop/current/l2",        "lutpiii/workshop/current/l3"};
// Frequency (Hz)
const char* workshop_f[]        = {"lutpiii/workshop/frequency/l1",      "lutpiii/workshop/frequency/l2",      "lutpiii/workshop/frequency/l3"};
// Apparent Power (VA)
const char* workshop_va[]       = {"lutpiii/workshop/apparent_power/l1", "lutpiii/workshop/apparent_power/l2", "lutpiii/workshop/apparent_power/l3"};
// Reactive Power (VAr)
const char* workshop_var[]      = {"lutpiii/workshop/reactive_power/l1", "lutpiii/workshop/reactive_power/l2", "lutpiii/workshop/reactive_power/l3"};
// Active Power (W)
const char* workshop_p[]        = {"lutpiii/workshop/active_power/l1",   "lutpiii/workshop/active_power/l2",   "lutpiii/workshop/active_power/l3"};
// Power Factor
const char* workshop_pf[]       = {"lutpiii/workshop/power_factor/l1",   "lutpiii/workshop/power_factor/l2",   "lutpiii/workshop/power_factor/l3"};
// Energy (kWh)
const char* workshop_e[]        = {"lutpiii/workshop/active_energy/l1",  "lutpiii/workshop/active_energy/l2",  "lutpiii/workshop/active_energy/l3"};
// Status Main Power
const char* line_status[]       = {"lutpiii/workshop/line_status/l1",    "lutpiii/workshop/line_status/l2",    "lutpiii/workshop/line_status/l3"};

// ================== ENUM LINE STATUS ==================
enum LineStatus { LINE_GOOD, LINE_UNDERVOLTAGE, LINE_ISSUED };
LineStatus lineStatus[NUM_PZEMS];  // status tiap line 

// ================== SETUP WIFI ==================
void setup_wifi() {
  if (!wifiManager.connectBestNetwork()) {
    Serial.println("Tidak bisa terhubung ke jaringan mana pun!");
  } else {
    wifiManager.printStatus();
  }
}

// ================== MQTT CONNECT ==================
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str(), MQTT_USER, MQTT_PASS)) {
      Serial.println("✅MQTT Server Connected!");
    } else {
      Serial.print("❌failed, rc=");
      Serial.print(client.state());
      Serial.println(" retry in 5s");
      delay(5000);
    }
  }
}

// ================== SETUP ==================
void setup() {
  Serial.begin(115200);
  setup_wifi();

  resetEnergy.begin();

  client.setServer(MQTT_SERVER, MQTT_PORT);
  // espClient.setInsecure(); // WARNING: tidak aman
  // espClient.setCACert(CA_CERT);

  // Init Serial2 untuk PZEM
  // PZEM_SERIAL.begin(9600, SERIAL_8N1, PZEM_RX_PIN, PZEM_TX_PIN);
}

// ================== LOOP ==================
void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  // WiFi auto check tiap 30 detik
  static unsigned long lastWiFiCheck = 0;
  if (millis() - lastWiFiCheck > 30000) {
    lastWiFiCheck = millis();
    if (!wifiManager.isConnected()) {
      Serial.println("WiFi terputus, mencoba koneksi ulang...");
      wifiManager.connectBestNetwork();
    }
  }

  resetEnergy.handle(pzems, NUM_PZEMS);

  unsigned long now = millis();
  if (now - lastMsg > 500) {  // interval 1 detik
    lastMsg = now;

    for (int i = 0; i < NUM_PZEMS; i++) {
      float voltage = pzems[i].voltage();
      float current = pzems[i].current();
      float power   = pzems[i].power();
      float energy  = pzems[i].energy()*1000;
      float freq    = pzems[i].frequency();
      float pf      = pzems[i].pf();

      // Apparent Power & Reactive Power
      float va = voltage * current;
      float var = sqrt((va * va) - (power * power));

      // if(!isnan(voltage)){
      //   lineStatus[i] = LINE_GOOD;
      //   // Debug Serial
      //   // Serial.print("PZEM "); Serial.print(i+1);
      //   // Serial.print(" | V: "); Serial.print(voltage);
      //   // Serial.print("V | I: "); Serial.print(current);
      //   // Serial.print("A | f: "); Serial.print(freq);
      //   // Serial.print("Hz | VA: "); Serial.print(va);
      //   // Serial.print("VA | VAR: "); Serial.print(var);
      //   // Serial.print("VAR | P: "); Serial.print(power);
      //   // Serial.print("W | PF: "); Serial.print(pf);
      //   // Serial.print(" | E: "); Serial.print(energy, 2);
      //   // Serial.println("Wh");
      // }else if(voltage < 199 && voltage > 1){
      //   lineStatus[i] = LINE_UNDERVOLTAGE;
      //   // Serial.println("⚡WARNING: 3-Phase is Running UNDERVOLTAGE!!!");
      // }else{
      //   lineStatus[i] = LINE_ISSUED;
      //   voltage = current = power = freq = va = var = pf = 0;
      //   // Serial.println("❌MAIN POWER ISSUED | PLEASE CHECK THE PROBE!!");
      // }

      if (isnan(voltage) || voltage <= 0) {
        lineStatus[i] = LINE_ISSUED; // Tidak ada data / probe error
        voltage = current = power = freq = va = var = pf = 0;
      }
      else if (voltage < 199) {
        lineStatus[i] = LINE_UNDERVOLTAGE; // Tegangan rendah
      }
      else {
        lineStatus[i] = LINE_GOOD; // Normal
      }

      // ============= SERIAL MONITOR =============
      switch (lineStatus[i]) {
        case LINE_GOOD:
          Serial.printf("✅ Line %d: GOOD (V: %.1fV", i + 1, voltage);
          // Serial.print("PZEM "); Serial.print(i+1);
          Serial.print(" | I: "); Serial.print(current);
          Serial.print("A | f: "); Serial.print(freq);
          Serial.print("Hz | VA: "); Serial.print(va);
          Serial.print("VA | VAR: "); Serial.print(var);
          Serial.print("VAR | P: "); Serial.print(power);
          Serial.print("W | PF: "); Serial.print(pf);
          Serial.print(" | E: "); Serial.print(energy, 2);
          Serial.println("Wh)");
          break;
        case LINE_UNDERVOLTAGE:
          Serial.printf("⚡ Line %d: UNDERVOLTAGE (V: %.1fV", i + 1, voltage);
          Serial.print(" | I: "); Serial.print(current);
          Serial.print("A | f: "); Serial.print(freq);
          Serial.print("Hz | VA: "); Serial.print(va);
          Serial.print("VA | VAR: "); Serial.print(var);
          Serial.print("VAR | P: "); Serial.print(power);
          Serial.print("W | PF: "); Serial.print(pf);
          Serial.print(" | E: "); Serial.print(energy, 2);
          Serial.println("Wh)");
          break;
        case LINE_ISSUED:
          Serial.printf("❌ Line %d: ISSUED / PROBE ERROR\n", i + 1);
          break;
      }


      // Publish ke MQTT
      client.publish(workshop_v[i], String(voltage).c_str());
      client.publish(workshop_i[i], String(current).c_str());
      client.publish(workshop_p[i], String(power).c_str());
      client.publish(workshop_e[i], String(energy).c_str());
      client.publish(workshop_f[i], String(freq).c_str());
      client.publish(workshop_pf[i], String(pf).c_str());
      client.publish(workshop_va[i], String(va).c_str());
      client.publish(workshop_var[i], String(var).c_str());

      // Publish status line
      const char* statusStr = (lineStatus[i] == LINE_GOOD) ? "good"
                             : (lineStatus[i] == LINE_UNDERVOLTAGE) ? "undervoltage"
                             : "issued";
      client.publish(line_status[i], statusStr);
    }

    Serial.println("=== Data Published ===");
  }
}
