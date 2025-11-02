#include "WiFiSelector.h"

WiFiSelector::WiFiSelector(unsigned long timeout) {
    _timeout = timeout;
}

bool WiFiSelector::connectBestNetwork() {
    Serial.println("\n[WiFiSelector] Memindai jaringan...");
    int n = WiFi.scanNetworks();

    if (n == 0) {
        Serial.println("Tidak ada jaringan ditemukan!");
        return false;
    }

    for (int i = 0; i < WIFI_COUNT; i++) {
        for (int j = 0; j < n; j++) {
            String ssid = WiFi.SSID(j);
            if (ssid == wifiList[i].ssid) {
                Serial.printf("Mencoba koneksi ke: %s (prioritas %d)\n", wifiList[i].ssid, wifiList[i].priority);
                if (connectToNetwork(wifiList[i])) {
                    Serial.printf("Terhubung ke %s\n", wifiList[i].ssid);
                    return true;
                }
            }
        }
    }

    Serial.println("Gagal terhubung ke semua jaringan.");
    return false;
}

bool WiFiSelector::connectToNetwork(const WiFiCredential& cred) {
    WiFi.begin(cred.ssid, cred.password);
    unsigned long startAttemptTime = millis();

    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < _timeout) {
        delay(500);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.printf("\nTerhubung ke %s, IP: %s\n", cred.ssid, WiFi.localIP().toString().c_str());
        return true;
    }

    Serial.printf("\nGagal konek ke %s\n", cred.ssid);
    WiFi.disconnect();
    return false;
}

bool WiFiSelector::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}

void WiFiSelector::printStatus() {
    if (isConnected()) {
        Serial.printf("WiFi Aktif: %s | IP: %s\n",
                      WiFi.SSID().c_str(),
                      WiFi.localIP().toString().c_str());
    } else {
        Serial.println("WiFi tidak terhubung.");
    }
}
