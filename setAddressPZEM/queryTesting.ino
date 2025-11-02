// #include <PZEM004Tv30.h>
// #include <SoftwareSerial.h>

// // UART1
// HardwareSerial PZEMSerial1(1);
// // UART2
// HardwareSerial PZEMSerial2(2);
// // SoftwareSerial untuk PZEM3
// SoftwareSerial PZEMSoft(18, 19); // RX=18, TX=19

// // Instance PZEM
// PZEM004Tv30 pzem1(&PZEMSerial1, 4, 5);    // RX=4, TX=5
// PZEM004Tv30 pzem2(&PZEMSerial2, 16, 17);  // RX=16, TX=17
// PZEM004Tv30 pzem3(&PZEMSoft);             // pakai SoftwareSerial RX=18, TX=19

// void setup() {
//   // Debug Serial
//   Serial.begin(115200);

//   // Init UART1
//   PZEMSerial1.begin(9600, SERIAL_8N1, 4, 5);
//   // Init UART2
//   PZEMSerial2.begin(9600, SERIAL_8N1, 16, 17);
//   // Init SoftwareSerial
//   PZEMSoft.begin(9600);

//   delay(1000);
//   Serial.println("=== Monitoring 3 PZEM ===");
// }

// void loop() {
//   // Baca PZEM1
//   float v1 = pzem1.voltage();
//   float i1 = pzem1.current();
//   float p1 = pzem1.power();
//   float e1 = pzem1.energy();

//   // Baca PZEM2
//   float v2 = pzem2.voltage();
//   float i2 = pzem2.current();
//   float p2 = pzem2.power();
//   float e2 = pzem2.energy();

//   // Baca PZEM3
//   float v3 = pzem3.voltage();
//   float i3 = pzem3.current();
//   float p3 = pzem3.power();
//   float e3 = pzem3.energy();

//   // Print hasil
//   Serial.println("---- PZEM 1 ----");
//   Serial.printf("V=%.2fV I=%.2fA P=%.2fW E=%.2fkWh\n", v1, i1, p1, e1);

//   Serial.println("---- PZEM 2 ----");
//   Serial.printf("V=%.2fV I=%.2fA P=%.2fW E=%.2fkWh\n", v2, i2, p2, e2);

//   Serial.println("---- PZEM 3 ----");
//   Serial.printf("V=%.2fV I=%.2fA P=%.2fW E=%.2fkWh\n", v3, i3, p3, e3);

//   Serial.println("==========================\n");

//   delay(2000);
// }
