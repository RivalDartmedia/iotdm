//Ref : https://github.com/yash-sanghvi/ESP32/blob/master/Captive_Portal/Captive_Portal.ino

#include <DNSServer.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include "ESPAsyncWebServer.h"

DNSServer dnsServer;
AsyncWebServer server(80);

String nama_pasien, kode_kamar, kode_bed, wifi_ssid, wifi_pass;
bool isWiFi;
bool patient_info_received = false;

const char start_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>Captive Portal Demo</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body>
  <h3>Input Informasi Pasien dan Konektifitas</h3>
  <form action="/get">
    Nama Pasien: <br><input type="text" name="nama_pasien">
    <br>
    Kode Kamar: <br><input type="text" name="kode_kamar">
    <br>
    Kode Bed: <br><input type="text" name="kode_bed">
    <br>
    Konektifitas: 
    <select name = "mode_koneksi">
      <option value=0>SIM</option>
      <option value=1>WiFi</option>
    </select>
    <br>
    <input type="submit" value="Submit">
  </form>
</body></html>)rawliteral";

const char setWiFi_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>Input Informasi WiFi</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body>
  <h3>Input WiFi</h3>
  <form action="/get2">
    SSID: <br><input type="text" name="wifi_ssid">
    <br>
    Pass: <br><input type="text" name="wifi_pass">
    <br>
    <input type="submit" value="Submit">
  </form>
</body></html>)rawliteral";

class CaptiveRequestHandler : public AsyncWebHandler {
  public:
    CaptiveRequestHandler() {}
    virtual ~CaptiveRequestHandler() {}

    bool canHandle(AsyncWebServerRequest *request) {
      //request->addInterestingHeader("ANY");
      return true;
    }

    void handleRequest(AsyncWebServerRequest *request) {
      request->send_P(200, "text/html", start_html);
    }
};

void setupServer() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/html", start_html);
    Serial.println("Client Connected");
  });

  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest * request) {
    String inputMessage;
    String inputParam;
    char message[100];

    if (request->hasParam("nama_pasien")) {
      inputMessage = request->getParam("nama_pasien")->value();
      inputParam = "nama_pasien";
      nama_pasien = inputMessage;
      Serial.println(inputMessage);
      patient_info_received = true;
    }

    if (request->hasParam("kode_kamar")) {
      inputMessage = request->getParam("kode_kamar")->value();
      inputParam = "kode_kamar";
      kode_kamar = inputMessage;
      Serial.println(inputMessage);
      patient_info_received = true;
    }

    if (request->hasParam("kode_bed")) {
      inputMessage = request->getParam("kode_bed")->value();
      inputParam = "kode_bed";
      kode_bed = inputMessage;
      Serial.println(inputMessage);
      patient_info_received = true;
    }

    if (request->hasParam("mode_koneksi")) {
      inputMessage = request->getParam("mode_koneksi")->value();
      inputParam = "mode_koneksi";
      isWiFi = inputMessage == "1";
      Serial.println(inputMessage);
      patient_info_received = true;
    }

    //Dari pilihan WiFi
    if (isWiFi) {
      request->send_P(200, "text/html", setWiFi_html);
    }
    else {
      sprintf(message, "Pasien %s. Kamar %s. Tempat tidur %s telah terdaftar. <br><a href=\"/\">Return to Home Page</a>", nama_pasien, kode_kamar, kode_bed);
      request->send(200, "text/html", message);
    }
  });

  server.on("/get2", HTTP_GET, [] (AsyncWebServerRequest * request) {
    String inputMessage;
    String inputParam;
    char message[100];

    if (request->hasParam("wifi_ssid")) {
      inputMessage = request->getParam("wifi_ssid")->value();
      inputParam = "wifi_ssid";
      wifi_ssid = inputMessage;
      Serial.println(inputMessage);
      patient_info_received = true;
    }

    if (request->hasParam("wifi_pass")) {
      inputMessage = request->getParam("wifi_pass")->value();
      inputParam = "wifi_pass";
      wifi_pass = inputMessage;
      Serial.println(inputMessage);
      patient_info_received = true;
    }
    sprintf(message, "Pasien %s. Kamar %s. Tempat tidur %s telah terdaftar. <br><a href=\"/\">Return to Home Page</a>", nama_pasien, kode_kamar, kode_bed);
    request->send(200, "text/html", message);
  });
}

void setup() {
  //your other setup stuff...
  Serial.begin(115200);
  Serial.println();
  Serial.println("Setting up AP Mode");
  WiFi.mode(WIFI_AP);
  WiFi.softAP("esp-captive");
  Serial.print("AP IP address: "); Serial.println(WiFi.softAPIP());
  Serial.println("Setting up Async WebServer");
  setupServer();
  Serial.println("Starting DNS Server");
  dnsServer.start(53, "*", WiFi.softAPIP());
  server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);//only when requested from AP
  //more handlers...
  server.begin();
  Serial.println("All Done!");
}

void loop() {
  dnsServer.processNextRequest();
  if (patient_info_received) {
    Serial.print("Hello "); Serial.println(nama_pasien);
    Serial.println(kode_kamar); Serial.println(kode_bed);
    Serial.println(isWiFi);
    patient_info_received = false;
    Serial.println("We'll wait for the next client now");
  }
}
