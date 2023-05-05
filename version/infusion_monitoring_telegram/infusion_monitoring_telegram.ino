#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include "SIM800L.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);

const int swPin = 16; //D0

// const char* ssid = "vivo 1612";
// const char* password = "dudungmaman";
const char* ssid = "Galaxy A014494";
const char* password = "tbxx2911";
// const char* ssid = "DM4";
// const char* password = "seninjumat2";

#define SIM800_TX_PIN 13  //Rx ESP D7
#define SIM800_RX_PIN 15  //Tx ESP D8
#define SIM800_RST_PIN 12 //D6
const char APN[] = "internet"; // ==> your APN
SIM800L* sim800l;

#define BOTtoken "6025777872:AAGOiaT-UVZCXnW7Ur9KS2xmuLIXgirmRE4"
#define CHAT_ID "-894765173"
X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

void setup() {
  pinMode(swPin, INPUT);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32
  Serial.begin(9600);
  while(!Serial);
  // setup_sim();
  configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
  client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
}

void loop() {
  int swState = digitalRead(swPin);
  if (swState == HIGH){
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.println("SIM MODE");
    display.setCursor(0,0);
    display.display();

    setup_sim();
    sim_mode();
  }else if (swState == LOW){
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.println("WIFI MODE");
    display.setCursor(0,0);
    display.display();

    setup_wifi();
    bot.sendMessage(CHAT_ID, "(WIFI MODE) ALERT!, OKSIGEN HABIS", "");
    Serial.println("Alert terkirim via Wifi");
    delay(3000);
  }
}