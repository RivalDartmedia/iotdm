#ifndef koneksi_wifi_h
#define koneksi_wifi_h

#include "mem_set.h"
#include "indikator.h"
#include "koneksi_cred.h"

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
String avail_wifi, port_ssid, port_name, port_pass, port_token;
bool portal_on;

String processor(const String &var)
{
  if (var == "wifi_selection")
  {
    return avail_wifi;
  }
  return String();
}

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

void setupServer(class InfusConfig &config)
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
        if (request->hasParam("infus_name"))
        {
          port_name = request->getParam("infus_name")->value();
        }
        if (request->hasParam("wifi_ssid"))
        {
          port_ssid = request->getParam("wifi_ssid")->value();
        }
        if (request->hasParam("wifi_pass"))
        {
          port_pass = request->getParam("wifi_pass")->value();
        }
        if (request->hasParam("token_id"))
        {
          port_token = request->getParam("token_id")->value();
        }
        // Mulai koneksi
        request->send(200, "text/plain", "Informasi tersimpan. Akhiri Sesi");
        portal_on = 0;
      }
    );
}

bool start_portal(InfusConfig &config)
{
  // Mulai Portal
  WiFi.mode(WIFI_OFF);
  WiFi.mode(WIFI_AP_STA);
  Serial.println(F("SSID : "));
  Serial.println(config.get(infus_name_p).c_str());

  WiFi.softAP(config.get(infus_name_p).c_str());
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
  // Close Server
  config.edit(infus_name_p, port_name);
  config.edit(wifi_pass_p, port_pass);
  config.edit(wifi_ssid_p, port_ssid);
  config.edit(tokenID_p, port_token);
  config.save(LittleFS);
  WiFi.mode(WIFI_OFF);
  return 0;
}

class ConnectionWiFi
{
private:
  // Update server+token+send_p+berat_v+   +tpm_v
  // get Blink Indicator server + get_p + blink_v

  String tokenid;
  String send_message;
  HTTPClient https;

public:
  bool checkwifi()
  {
    int limit_try = 10, cnt = 0;
    while (WiFi.status() != WL_CONNECTED && cnt < limit_try)
    {
      Serial.println("Tidak terkoneksi dengan Wifi");
      // Serial.println("Trying to Reconnect");
      // WiFi.begin(ssid, password);
      delay(250);
    }
    // Check koneksi
    if(cnt < limit_try){
      return 1;
    }
    // Return 0 jika tidak bisa koneksi
    return 0;
  }
  
  void connectWifi(InfusConfig &infusconfig)
  {
    WiFi.begin(infusconfig.get(wifi_ssid_p).c_str(), infusconfig.get(wifi_pass_p).c_str());
    delay(500);
  }

  int update_secure(InfusConfig &infusconfig, double tpm, int weigh, indi_state &indi_command)
  {
    // Mulai koneksi
    // WiFi.begin(infusconfig.get(wifi_ssid_p).c_str(), infusconfig.get(wifi_pass_p).c_str());
    // do
    // {
    //   WiFi.begin(infusconfig.get(wifi_ssid_p).c_str(), infusconfig.get(wifi_pass_p).c_str());
    //   delay(500);
    // } while (!this->checkwifi());

    Serial.println("Terhubung WiFi");

    WiFiClientSecure *client = new WiFiClientSecure;
    int httpCode;
    if (client)
    {
      tokenid = infusconfig.get(tokenID_p);
      client->setCACert(DEFAULT_ROOT_CA);
      {
        // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is
        HTTPClient https;
        send_message = server_dom + send_p + token + tokenid + berat_v + String(weigh) + tpm_v + String(tpm);
        // Serial.printf("Sending %s\n", send_message);
        // Serial.print("[HTTPS] begin...\n");
        if (https.begin(*client, send_message))
        { // HTTPS
          // Serial.print("[HTTPS] GET...\n");
          // start connection and send HTTP header
          httpCode = https.GET();

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

          send_message = server_dom + get_p + token + tokenid + blink_v;
          // New Connect to get blink command
          if (https.begin(*client, send_message))
          { // HTTPS
            // Serial.print("[HTTPS] GET...\n");
            // start connection and send HTTP header
            httpCode = https.GET();

            // httpCode will be negative on error
            if (httpCode > 0)
            {
              // HTTP header has been send and Server response header has been handled
              Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

              // file found at server
              if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
              {
                String payload = https.getString();
                // Serial.println(payload);
                int payload_val = payload.toInt();
                //Atur Indikator disini
                if(payload_val >= 255){
                  indi_command = blink_fast;
                }else{
                  indi_command = blink_slow;
                }
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
    }
    return httpCode;
  }
};

#endif // !1