#ifndef koneksi_h
#define koneksi_h

#include "mem_set.h"
#include "indikator.h"

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


  class CaptiveRequestHandler : public AsyncWebHandler
  {
  public:
    CaptiveRequestHandler() {}
    virtual ~CaptiveRequestHandler() {}

    bool canHandle(AsyncWebServerRequest *request)
    {
      request->addInterestingHeader("Setting Infus");
      return true;
    }

    void handleRequest(AsyncWebServerRequest *request)
    {
      request->send(LittleFS, "/edit_data.htm", String(), false, processor);
    }
  };

class ConnectionWiFi
{
private:
  String server = "https://sgp1.blynk.cloud/external/api/"; // Server URL
  String send_p = "batch/update?";
  String get_p = "get?";
  String token = "token=2nrtIgwDCHP5SF3CToAWWdWZFPGtz6oX";
  String berat_v = "&v0=";
  String tpm_v = "&v1=";
  String blink_v = "&v2"
      // Update server+token+send_p+berat_v+   +tpm_v
      // get Blink Indicator server + get_p + blink_v

  HTTPClient https;
  String avail_wifi;
  bool available, portal_on;

  String processor(const String &var)
  {
    if (var == "wifi_selection")
    {
      return avail_wifi;
    }
    return String();
  }

  void checkavailnetwork()
  {
    avail_wifi = "SSID: <select name = wifi_ssid>";

    int n = WiFi.scanNetworks();
    // Serial.println("scan done");
    if (n == 0)
    {
      avail_wifi += "<option value=null>No Network Available</option><br>";
    }
    else
    {
      for (int i = 0; i < n; ++i)
      {
        avail_wifi += "<option value=" + WiFi.SSID(i) + ">" + WiFi.SSID(i) + "</option><br>";
      }
    }
    avail_wifi += "</select><br>";
  }

void setupServer(InfusConfig &config)
  {
    checkavailnetwork();
    server.on(
        "/",
        HTTP_GET,
        [](AsyncWebServerRequest *request)
        {
          request->send(LittleFS, "/edit_data.htm", String(), false, processor);
          Serial.println("Client Connected");
        });

    server.on(
        "/save_data",
        HTTP_GET,
        [](AsyncWebServerRequest *request)
        {
          if (request->hasParam("wifi_ssid"))
          {
            config.edit(wifi_ssid_p, request->getParam("wifi_ssid")->value());
          }
          if (request->hasParam("wifi_pass"))
          {
            config.edit(wifi_pass_p, request->getParam("wifi_pass")->value());
          }
          if (request->hasParam("token_id"))
          {
            config.edit(tokenID_p, request->getParam("token_id")->value());
          }
          // Mulai koneksi
          request->send(200, "text/plain", "Informasi tersimpan. Akhiri Sesi");
          portal_on = 0;
        });
  }
public:
  bool start_portal(InfusConfig &config)
  {
    // Mulai Portal
    WiFi.mode(WIFI_OFF);
    WiFi.mode(WIFI_AP_STA);
    Serial.println(F("SSID : "));
    Serial.println(config.get(infus_name_p));

    WiFi.softAP(config.get(infus_name_p));
    setupServer(config);
    dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
    dnsServer.start(53, "*", WiFi.softAPIP());

    server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER); // only when requested from AP
    // more handlers...
    server.begin();
    portal_on = 1;
    while (portal_on)
    {
      dnsServer.processNextRequest();
      delay(100);
    }
    server.end();
    //Close Server

    return 0;
  }

  int update_secure(InfusConfig &infusconfig, double tpm, int weigh, int &indi_command)
  {
    // Mulai koneksi
    // WiFi.begin(infusconfig.get(wifi_ssid_p), infusconfig.get(wifi_pass_p));
    do
    {
      WiFi.begin(SSID, Pass);
      delay(500);
    } while (!this->checkwifi());

    Serial.println("Terhubung WiFi");

    WiFiClientSecure *client = new WiFiClientSecure;
    if (client)
    {
      client->setCACert(DEFAULT_ROOT_CA);
      {
        // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is
        HTTPClient https;
        Serial.printf("Sending %s\n", berat_v + String(val_sample_berat) + tpm_v + String(val_sample_tpm));
        Serial.print("[HTTPS] begin...\n");
        if (https.begin(*client, server + token + berat_v + String(val_sample_berat) + tpm_v + String(val_sample_tpm)))
        { // HTTPS
          Serial.print("[HTTPS] GET...\n");
          // start connection and send HTTP header
          int httpCode = https.GET();

          // httpCode will be negative on error
          if (httpCode > 0)
          {
            // HTTP header has been send and Server response header has been handled
            Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

            // file found at server
            if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
            {
              String payload = https.getString();
              Serial.println(payload);
            }
          }
          else
          {
            Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
          }

          https.end();
          //New Connect to get blink command
          if (https.begin(*client, server + token + berat_v + String(val_sample_berat) + tpm_v + String(val_sample_tpm)))
          { // HTTPS
            Serial.print("[HTTPS] GET...\n");
            // start connection and send HTTP header
            int httpCode = https.GET();

            // httpCode will be negative on error
            if (httpCode > 0)
            {
              // HTTP header has been send and Server response header has been handled
              Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

              // file found at server
              if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
              {
                String payload = https.getString();
                Serial.println(payload);
              }
            }
            else
            {
              Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
            }

          https.end();
        }
        else
        {
          Serial.printf("[HTTPS] Unable to connect\n");
        }
        // End extra scoping block
      }
      delete client;
    }
    return httpCode;
  }

  int get_indicator()
  {
    // cek apa perlu memberikan output indikasi
    return 0;
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
};

#endif // !1