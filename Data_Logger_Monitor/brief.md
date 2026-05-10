project yang sudah jadi dan sudah rilis
ini penjelasan project yang sekarang dan yang akan di update firmware nya
Nama Project  : Data Logger 3 Phase

Overview Alat (Hardware)      : konteksnya alat ini hanya berfungsi untuk akuisisi data dan mengirimkan ke mqtt broker (tidak melakukan data logger), data logger sdh ada unit website yang khusus menangani data logging yang datanya di ambil dari mqtt broker
                                1. Akuisisi Data dari 3 buah PZEM-004T (Masing2 PZEM handle 1 Line RST to Netral (WYE/220V))
                                2. Pengolahan Data
                                3. Publish Data ke MQTT Broker (per topic)
                                4. Handle Display OLED 0.96" (display tampilan sangat alakadarnya) dan push button untuk ganti slide page parameter 3 phase
                                5. bisa management signal wi-fi dan prioritasnya (masih manual (hard-code) atur priotitas)
                                6. ada fitur debug berupa kirim data random ke broker namun masih harus buka tutup komentar di baris program (dan masih cuma punya 1 jenis debug aja)
                                7. masih ada beberapa kekurangan/bug sistem seperti:
                                  a. [asinkron data] data antar parameter yang sampai ke broker atau website tidak serentak/tidak bersamaan (ada yang terlalu cepat/terlalu lambat sepersekian detik) karena masih publish mentah per topic dan belum pakai json
                                  b. [blocking] saat booting esp32 terus melakukan connecting ke wifi dan tidak akan masuk display utama sampai esp32 terkoneksi ke wifi
                                  c. belum ada fitur kalibrasi
                                  d. belum ada webserver & webpage (untuk config dan kalibrasi)
                                  e. masih ada sistem blocking dan non blocking
                                  f. belum ada state machine & self-healing kalau error di lapangan
                                  g. display OLED masih polosan raw text dan susunan parameter masih berantakan
                                  h. Wifi Selector belum seperti pintar network management windows maupun smartphone yang mempunyai known network dan pintar mencari prioritas serta otomatis tersambung 

Spesifikasi Alat (Electrical) : 1. ESP32 Devkit V1 30pin
                                2. PZEM-004T 100A (3 buah, masing2 handle 1 fasa dari RST ke Netral (WYE/220V))
                                3. OLED 0.96" I2C
                                4. Push Button (Normally Open) untuk Menu OLED
                                5. Built-in LED ESP32 untuk signal indikator seperti reset kWh

Konfigurasi Pinout Electrical : 
//************* PZEM ****************//
- PZEM Fasa R dan S dalam satu hardware serial 1
- PZEM Fasa T berada dalam hardware serial 2 

#define PZEM_RX1_PIN 4
#define PZEM_TX1_PIN 15
#define PZEM_RX2_PIN 17
#define PZEM_TX2_PIN 16
// #define PZEM_SERIAL Serial2
HardwareSerial PZEMSerial1(1);
HardwareSerial PZEMSerial2(2);
#define NUM_PZEMS 3   // Ubah sesuai jumlah PZEM

PZEM004Tv30 pzems[NUM_PZEMS] = {
  PZEM004Tv30(PZEMSerial1, PZEM_RX1_PIN, PZEM_TX1_PIN, 0x10), // Line R / PZEM 1
  PZEM004Tv30(PZEMSerial1, PZEM_RX1_PIN, PZEM_TX1_PIN, 0x11), // Line S / PZEM 2
  PZEM004Tv30(PZEMSerial2, PZEM_RX2_PIN, PZEM_TX2_PIN, 0x12)  // Line T / PZEM 3
};
//************* PZEM ****************//

//************* OLED 0.96" I2C ****************//
SDA= GPIO 21
SCL= GPIO 22
//************* OLED 0.96" I2C ****************//

//*************** Button & LED ****************//
BUTTON  5
LED     2
//*************** Button & LED ****************//


Yang Harus diperbaiki/diupdate adalah:
1. Semua bagian sistem dan fungsi harus asynchronous task/non blocking system kalau perlu gunakan dua core esp32
2. sentralisasi semua config dalam 1 file config.h dan pisahkan section config yang hardcoded dan config yang bisa diakses/modifikasi lewat webpage/webserver esp32
3. [Publish Data] Ubah Metode Publish data dari yang Publish data langsung per topic ke broker menjadi kirim data json ke broker (desainlah struktur file json yang proper dan rapi dengan parameter yang lengkap dan mudah di parsing di backend node.js)
4. [Bug] perbaiki bug blocking saat booting yang mengharuskan sistem terkoneksi ke access point wi-fi dulu baru bisa masuk ke sistem (tetap buat opening screen OLED)
5. [Display] upgrade tampilan OLED 0.96" (128x64 pixel) SSD1306 dengan tampilan dan menu yang lebih flexible
 *    Monitoring Mode:
 *    Page 1 [Monitoring Mode]:
 *   ┌────────────────────────────┐
 *   │     3Phase Data Logger     │  ← Title & list separator under the title
 *   │   R         S         T    │  ← Phase Name         
 *   │  220V      220V      220V  │  ← Voltage Line to Netral   
 *   │  100A      100A      100A  │  ← Current per phase   (0-100)
 *   │   OK        OK        OK   │  ← Line_Status   (LOST/UNDER/OK)
 *   └────────────────────────────┘
 *
 *    Page 2 [Monitoring Mode]:
 *   ┌────────────────────────────┐
 *   │     3Phase Data Logger     │  ← Title & list separator under the title
 *   │   R         S         T    │  ← Phase Name         
 *   │  60Hz      60Hz      60Hz  │  ← Frequency     
 *   │ 100VA     100VA     100VA  │  ← Apparent Power (L to N)   
 *   │ 100VAr    100VAr    100VAr │  ← Reactive Power (L to N)          
 *   └────────────────────────────┘
 *

5. [Web-server] buat webpage config yang bisa diakses lewat smartphone atau laptop yang terhubung melalui IP Address yang ESP32 keluarkan 


