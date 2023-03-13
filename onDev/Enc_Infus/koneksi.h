#ifndef koneksi_h
#define koneksi_h

#include "mem_set.h"

#include <DNSServer.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include "ESPAsyncWebServer.h"
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Arduino.h>
#include <SoftwareSerial.h>
#include "SIM800L.h"
#include <ArduinoJson.h>

#define SIM800_TX_PIN 16
#define SIM800_RX_PIN 17
#define SIM800_RST_PIN 22

static const char DEFAULT_ROOT_CA[] =
#include "certs/certloc_pem.h"

DNSServer dnsServer;
AsyncWebServer server(80);

class ConnectionManager
{
private:
    const char *server = "http://sgp1.blynk.cloud/external/api/update?token=ZIjaYVCHA9Vota0HFas5xh49JGXrM3Zy&V4=60"; // Server URL
    HTTPClient https;
    const char* APN = "3gprs";
    SIM800L* sim800l;

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
    void init(){
      SoftwareSerial* serial = new SoftwareSerial(SIM800_TX_PIN, SIM800_RX_PIN);
      serial->begin(9600); 
      delay(1000);

      sim800l = new SIM800L((Stream *)serial, SIM800_RST_PIN, 200, 512);
      // sim800l = new SIM800L((Stream *)serial, SIM800_RST_PIN, 200, 512, (Stream *)&Serial);
      Serial.println("Waiting for module settings...");
  
      // Setup module for GPRS communication
      int PowerMode= sim800l->getPowerMode(); 
      if(PowerMode==1) {
        Serial.println(F("Module in normal power mode"));
        Serial.println(F("Ready for next setup..."));
      } else {
        Serial.println(F("Switch to normal power mode ..."));
        sim800l->setPowerMode(NORMAL);
        delay(200);
      }
      setupModule();
    }

    void setupModule(){
      // Wait until the module is ready to accept AT commands
      while(!sim800l->isReady()) {
      Serial.println(F("Problem to initialize AT command, retry in 1 sec"));
      delay(1000);
      }
      Serial.println(F("Setup Complete!"));
 
      // Wait for operator network registration (national or roaming network)
      NetworkRegistration network = sim800l->getRegistrationStatus();
      while(network != REGISTERED_HOME && network != REGISTERED_ROAMING) {
        delay(1000);
        network = sim800l->getRegistrationStatus();
      }
      Serial.println(F("Network registration OK"));
      delay(1000);

      // Setup APN for GPRS configuration
      bool success = sim800l->setupGPRS(APN);
      while(!success) {
        success = sim800l->setupGPRS(APN);
        delay(5000);
      }
      Serial.println(F("GPRS config OK"));
    }

    void connect_gprs(){
      Serial.println("Mulai...");
      // Establish GPRS connectivity (5 trials)
      bool connected = false;
      for(uint8_t i = 0; i < 5 && !connected; i++) {
        delay(1000);
        connected = sim800l->connectGPRS();
      }

      // Check if connected, if not reset the module and setup the config again
      if(connected) {
        Serial.println(F("GPRS connected !"));
      } else {
        Serial.println(F("GPRS not connected !"));
        Serial.println(F("Reset the module."));
        sim800l->reset();
        setupModule();
        return;
      }
  
      Serial.println(F("Start HTTP GET..."));

      //Write TS Channel
      WriteTS();
  
      // Close GPRS connectivity (5 trials)
      bool disconnected = sim800l->disconnectGPRS();
      for(uint8_t i = 0; i < 5 && !connected; i++) {
        delay(1000);
        disconnected = sim800l->disconnectGPRS();
      }
  
      if(disconnected) {
        Serial.println(F("GPRS disconnected !"));
      } else {
        Serial.println(F("GPRS still connected !"));
      }
      sim800l->reset();
      delay(1000);
    }

    void WriteTS(){
      // String address="http://date.jsontest.com/";
      // String address="http://api.telegram.org/bot6025777872:AAGOiaT-UVZCXnW7Ur9KS2xmuLIXgirmRE4/sendMessage?chat_id=-894765173&text=tes";
      int number = random(0, 100);
      String numberstr;
      numberstr = String(number);
      Serial.println(numberstr);
      String address="http://sgp1.blynk.cloud/external/api/update?token=ZIjaYVCHA9Vota0HFas5xh49JGXrM3Zy&V4="+numberstr;
      char URL1[100];
 
      // Do HTTP GET communication with 10s for the timeout (read)
      address.toCharArray(URL1, 100) ;
  
      uint16_t rc = sim800l->doGet(URL1, 10000);
      if(rc == 200) {
        // Success, output the data received on the serial
        Serial.print(F("HTTP GET successful ("));
        Serial.print(sim800l->getDataSizeReceived());
        Serial.println(F(" bytes)"));
        Serial.print(F("Received : "));
        String TS_data=sim800l->getDataReceived();
        Serial.println(TS_data);
      } else {
        // Failed...
        Serial.print(F("HTTP GET error "));
        Serial.println(rc);
      }
    }
};

#endif // !1