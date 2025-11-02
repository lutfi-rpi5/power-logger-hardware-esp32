#ifndef WIFI_SELECTOR_H
#define WIFI_SELECTOR_H

#include <WiFi.h>
#include "credentials.h"

class WiFiSelector {
public:
    WiFiSelector(unsigned long timeout = 10000);  // default 10 detik
    bool connectBestNetwork();                     // koneksi ke WiFi terbaik
    bool isConnected();                            // cek koneksi
    void printStatus();                            // tampilkan info koneksi

private:
    unsigned long _timeout;
    bool connectToNetwork(const WiFiCredential& cred);
};

#endif
