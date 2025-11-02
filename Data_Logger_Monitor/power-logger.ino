// #include <WiFi.h>
// #include <PubSubClient.h>
// #include <PZEM004Tv30.h>

// // PZEM-400T
// HardwareSerial pzemSerial(2);
// PZEM004Tv30 pzem(pzemSerial, 16, 17);  // PIN RX=16, TX=17

// // Update these with values suitable for your network.
// const char* ssid = "FLA 2.4G";
// const char* password = "20092009";
 
// // const char* ssid = "PTBA-LOGGER";
// // const char* password = "katekpasswordnyo";
// const char* mqtt_server = "192.168.1.50"; // IP Broker Mosquitto

// WiFiClient espClient;
// PubSubClient client(espClient);
// unsigned long lastMsg = 0;
// #define MSG_BUFFER_SIZE (50)
// char msg[MSG_BUFFER_SIZE];

// // Topic Voltage / V
// #define workshop_v_l1 "workshop/voltage/l1"
// #define workshop_v_l2 "workshop/voltage/l2"
// #define workshop_v_l3 "workshop/voltage/l3"

// // Topic Current / I
// #define workshop_i_l1 "workshop/current/l1"
// #define workshop_i_l2 "workshop/current/l2"
// #define workshop_i_l3 "workshop/current/l3"

// // Topic Frequency / f
// #define workshop_f_l1 "workshop/frequency/l1"
// #define workshop_f_l2 "workshop/frequency/l2"
// #define workshop_f_l3 "workshop/frequency/l3"

// // Topic Apperent Power / VA
// #define workshop_va_l1 "workshop/apparent_power/l1"
// #define workshop_va_l2 "workshop/apparent_power/l2"
// #define workshop_va_l3 "workshop/apparent_power/l3"

// // Topic Reactive Power / VAr
// #define workshop_var_l1 "workshop/reactive_power/l1"
// #define workshop_var_l2 "workshop/reactive_power/l2"
// #define workshop_var_l3 "workshop/reactive_power/l3"

// // Topic Active Power / Watt
// #define workshop_p_l1 "workshop/active_power/l1"
// #define workshop_p_l2 "workshop/active_power/l2"
// #define workshop_p_l3 "workshop/active_power/l3"

// // Topic Power Factor
// #define workshop_pf_l1 "workshop/power_factor/l1"
// #define workshop_pf_l2 "workshop/power_factor/l2"
// #define workshop_pf_l3 "workshop/power_factor/l3"

// // Topic Active Energy / kWh
// #define workshop_e_l1 "workshop/active_energy/l1"
// #define workshop_e_l2 "workshop/active_energy/l2"
// #define workshop_e_l3 "workshop/active_energy/l3"

// void setup_wifi() {
//   // pzem.resetEnergy();
//   // delay(10);
//   Serial.println();
//   Serial.print("Connecting to ");
//   Serial.println(ssid);

//   WiFi.mode(WIFI_STA);
//   WiFi.begin(ssid, password);

//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }
//   randomSeed(micros());
//   Serial.println("");
//   Serial.println("WiFi connected");
//   Serial.println("IP address: ");
//   Serial.println(WiFi.localIP());
// }

// void reconnect() {
//   while (!client.connected()) {
//     Serial.print("Attempting MQTT connection...");
//     String clientId = "ESP32Client-";
//     clientId += String(random(0xffff), HEX);
//     if (client.connect(clientId.c_str())) {
//       Serial.println("connected");
//     } else {
//       Serial.print("failed, rc=");
//       Serial.print(client.state());
//       Serial.println(" try again in 5 seconds");
//       delay(5000);
//     }
//   }
// }

// void setup() {
//   Serial.begin(115200);
//   setup_wifi();
//   // client.setServer(mqtt_server, 1883);
//   pzem.resetEnergy();
// }

// void loop() {
//   // if (!client.connected()) {
//   //   reconnect();
//   // }
//   // client.loop();

//   unsigned long now = millis();
//   if (now - lastMsg > 1000) {   // interval 1 detik
//     lastMsg = now;

//     // Simulasi data sensor dengan range realistis
//     // Voltage
//     float v1 = pzem.voltage();        // random(2190, 2260) / 10.0;
//     float v2 = random(2190, 2260) / 10.0;
//     float v3 = random(2190, 2260) / 10.0;

//     // Current
//     float i1 = pzem.current();       // random(400, 600) / 10.0;
//     float i2 = random(400, 600) / 10.0;
//     float i3 = random(400, 600) / 10.0;

//     // Power Factor
//     float pf1 = pzem.pf();           // random(85, 100) / 100.0;
//     float pf2 = random(85, 100) / 100.0;
//     float pf3 = random(85, 100) / 100.0;

//     // Frequency
//     float f1 = pzem.frequency();     // random(495, 505) / 10.0;
//     float f2 = random(495, 505) / 10.0;
//     float f3 = random(495, 505) / 10.0;

//     // Apparent Power (VA)
//     float va1 = v1 * i1;
//     float va2 = v2 * i2;
//     float va3 = v3 * i3;

//     // Active Power (W)
//     float p1 = pzem.power();  // va1 * pf1;
//     float p2 = va2 * pf2;
//     float p3 = va3 * pf3;

//     // Reactive Power (VAr) = sqrt(S² - P²)
//     float var1 = sqrt((va1 * va1) - (p1 * p1));
//     float var2 = sqrt((va2 * va2) - (p2 * p2));
//     float var3 = sqrt((va3 * va3) - (p3 * p3));

//     // Akumulasi energi (kWh), interval 1 detik
//     static float e1 = 0, e2 = 0, e3 = 0;
//     e1 = pzem.energy()*1000;  //+= p1 / 3600.0; // (dari W ke kWh) // (mWh dari module)
//     e2 += p2 / 3600.0;
//     e3 += p3 / 3600.0;

//     // Cetak ke serial
//   Serial.print("Voltage: ");
//   Serial.print(v1);
//   Serial.print(" V | Current: ");
//   Serial.print(i1);
//   Serial.print(" A | Power: ");
//   Serial.print(p1);
//   Serial.print(" W | Energy: ");
//   Serial.print(e1);
//   Serial.println(" Wh");

//     // Publish ke masing-masing topic
//     // client.publish(workshop_v_l1, String(v1).c_str());
//     // client.publish(workshop_v_l2, String(v2).c_str());
//     // client.publish(workshop_v_l3, String(v3).c_str());

//     // client.publish(workshop_i_l1, String(i1).c_str());
//     // client.publish(workshop_i_l2, String(i2).c_str());
//     // client.publish(workshop_i_l3, String(i3).c_str());

//     // client.publish(workshop_f_l1, String(f1).c_str());
//     // client.publish(workshop_f_l2, String(f2).c_str());
//     // client.publish(workshop_f_l3, String(f3).c_str());

//     // client.publish(workshop_va_l1, String(va1).c_str());
//     // client.publish(workshop_va_l2, String(va2).c_str());
//     // client.publish(workshop_va_l3, String(va3).c_str());

//     // client.publish(workshop_p_l1, String(p1).c_str());
//     // client.publish(workshop_p_l2, String(p2).c_str());
//     // client.publish(workshop_p_l3, String(p3).c_str());

//     // client.publish(workshop_var_l1, String(var1).c_str());
//     // client.publish(workshop_var_l2, String(var2).c_str());
//     // client.publish(workshop_var_l3, String(var3).c_str());

//     // client.publish(workshop_e_l1, String(e1).c_str());
//     // client.publish(workshop_e_l2, String(e2).c_str());
//     // client.publish(workshop_e_l3, String(e3).c_str());

//     // client.publish(workshop_pf_l1, String(pf1).c_str());
//     // client.publish(workshop_pf_l2, String(pf2).c_str());
//     // client.publish(workshop_pf_l3, String(pf3).c_str());

//     // Serial.println("Data published to MQTT topics");
//   }
// }
