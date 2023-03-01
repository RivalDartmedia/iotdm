#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <WiFiManager.h>

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);

int sensorPin = 13; // This is our input pin D7
unsigned long lastflash;
int RPM;

const int swPin = 16; //D0
int swState;

const char* ssid = "vivo 1612";
const char* password = "dudungmaman";
// const char* ssid = "DM4";
// const char* password = "seninjumat2";

#define BOTtoken "6025777872:AAGOiaT-UVZCXnW7Ur9KS2xmuLIXgirmRE4"
#define CHAT_ID "-894765173"
X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

void ICACHE_RAM_ATTR sens() {
  RPM++;
}

void setup() {
  pinMode(sensorPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(sensorPin), sens, RISING);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32
  Serial.begin(115200);
  setup_connection();
  configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
  client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
  Serial.println("SETUP BERHASIL");
}

void loop() {
  sensor_read();
}