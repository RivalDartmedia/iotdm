// #include <ESP8266WiFi.h>
#include <WiFi.h>
// #include <ESP8266HTTPClient.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "ZTE_2.4G_5yzkX4";
const char* password = "123456789";

WiFiClient client;
HTTPClient http;


void setupWifi(){
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.print(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WIFI connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  setupWifi();
}

void loop() {
  Serial.print("[HTTP] begin...\n");
  int number = random(0, 100);
  String numberstr;
  numberstr = String(number);
  Serial.println(numberstr);
  // if (http.begin(client, "http://date.jsontest.com/")) {
  // if (http.begin(client, "http://tytomulyono.com/test.php")){
  if (http.begin(client, "http://sgp1.blynk.cloud/external/api/update?token=ZIjaYVCHA9Vota0HFas5xh49JGXrM3Zy&V4="+numberstr)){

    Serial.print("[HTTP] GET...\n");
    // start connection and send HTTP header
    int httpCode = http.GET();
    Serial.println(httpCode);

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        String payload = http.getString();
        Serial.println(payload);
      }
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
      http.end();
  } else {
    Serial.printf("[HTTP} Unable to connect\n");
  }

  delay(3000);
}