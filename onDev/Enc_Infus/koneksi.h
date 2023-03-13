#ifndef koneksi_h
#define koneksi_h

#include "mem_set.h"

#include <DNSServer.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include "ESPAsyncWebServer.h"
#include <WiFiClientSecure.h>

static const char DEFAULT_ROOT_CA[] =
#include <certs/certloc_pem.h>

DNSServer dnsServer;
AsyncWebServer server(80);

WiFiClientSecure client;

class ConnectionManager{
    private:
    const char* server = "blynk.cloud";  // Server URL
    const 
    public:
    bool start_portal(InfusConfig &infusconfig){
        Serial.println(infusconfig.get(infus_name_p));
        return 0;
    }

    bool connect_client_wifi_secure(InfusConfig &infusconfig){
        //Mulai koneksi
        WiFi.begin(infusconfig.get(wifi_ssid_p),infusconfig.get(wifi_pass_p));
        //Set CA / get fingerprint
        client.setCACert(DEFAULT_ROOT_CA);
        //Connect
        if (!client.connect(server, 443)){
            Serial.println("Connection failed!");
            return 0;
        }
        return 1;
    }

    bool send_sens(float tpm, float weigh){
        //Prioritas koneksi wifi
        if(checkwifi()){
            //Connect lewat wifi
            //Kirim nilai tpm dan weigh
            return 1;
        }
        else if (checksim()) 
        {
            //Connect lewat SIM jika tersedia
            //Kirim nilai tpm dan weigh
            return 1;
        }
        
        //Tidak bisa koneksi, show error
        return 0;
    }
    
    int get_indicator(){
        //cek apa perlu memberikan output indikasi

    }
    bool checkwifi(){
        if(WiFi.status() != WL_CONNECTED){
            Serial.println("Tidak bisa connect Wifi");
            // Serial.println("Trying to Reconnect");
            // WiFi.begin(ssid, password);
            return 0;
        }
        // Check koneksi
        // Return 0 jika tidak bisa koneksi
        return 1;
    }

    bool checksim(){
        //Fungsi untuk cek apa bisa koneksi disini
        return 0;
    }
};

#endif // !1