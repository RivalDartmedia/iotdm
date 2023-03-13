#ifndef koneksi_h
#define koneksi_h

#include "mem_set.h"

#include <DNSServer.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include "ESPAsyncWebServer.h"
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

static const char DEFAULT_ROOT_CA[] =
#include "certs/certloc_pem.h"

DNSServer dnsServer;
AsyncWebServer server(80);

class ConnectionManager
{
private:
    const char *server = "http://sgp1.blynk.cloud/external/api/update?token=ZIjaYVCHA9Vota0HFas5xh49JGXrM3Zy&V4=60"; // Server URL
    HTTPClient https;

public:
    bool start_portal(InfusConfig &infusconfig)
    {
        Serial.println(infusconfig.get(infus_name_p));
        return 0;
    }

    bool connect_client_wifi_secure(InfusConfig &infusconfig)
    {
        // Mulai koneksi
        // WiFi.begin(infusconfig.get(wifi_ssid_p), infusconfig.get(wifi_pass_p));
        WiFi.begin("ZTE_2.4G_5yzkX4","123456789");
        while(!this->checkwifi()){
            delay(200);
        }
        // Set CA / get fingerprint
        WiFiClientSecure *client = new WiFiClientSecure;
        if (client)
        {
            client->setCACert(DEFAULT_ROOT_CA);

            int httpCode = https.GET();
            // httpCode will be negative on error
            if (httpCode > 0)
            {
                // HTTP header has been send and Server response header has been handled
                Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
                // file found at server
                if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
                {
                    // print server response payload
                    String payload = https.getString();
                    Serial.println(payload);
                }
            }
            https.end();
            delete client;
        }
        return 1;
    }

    bool send_sens(float tpm, float weigh)
    {
        // Prioritas koneksi wifi
        if (checkwifi())
        {
            // Connect lewat wifi
            // Kirim nilai tpm dan weigh
            return 1;
        }
        else if (checksim())
        {
            // Connect lewat SIM jika tersedia
            // Kirim nilai tpm dan weigh
            return 1;
        }

        // Tidak bisa koneksi, show error
        return 0;
    }

    int get_indicator()
    {
        // cek apa perlu memberikan output indikasi
    }
    bool checkwifi()
    {
        if (WiFi.status() != WL_CONNECTED)
        {
            Serial.println("Tidak bisa connect Wifi");
            // Serial.println("Trying to Reconnect");
            // WiFi.begin(ssid, password);
            return 0;
        }
        // Check koneksi
        // Return 0 jika tidak bisa koneksi
        return 1;
    }

    bool checksim()
    {
        // Fungsi untuk cek apa bisa koneksi disini
        return 0;
    }
};

#endif // !1