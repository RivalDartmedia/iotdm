#ifndef display_led_h
#define display_led_h

#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);

class DisplayLed
{
public:
    void init()
    {
        display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32
    }

    void tes()
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("TES");
    }

    void print(String text)
    {
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0,0);
        display.println(text);
        display.display();
    }

    void cekMemoriWiFi()
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Checking WiFi");
        lcd.setCursor(0, 1);
        lcd.print("Memory");
    }

    void wiFiFound(String ssid)
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("WiFi data found:");
        lcd.setCursor(0, 1);
        lcd.print(ssid);
    }

    void connectingWiFi(String wifi)
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Connecting to:");
        lcd.setCursor(0, 1);
        lcd.print(wifi);
    }

    void settingWiFi(int setWiFi)
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Reset WiFi ?");
        lcd.setCursor(0, 1);
        lcd.print(setWiFi);
    }

    void weighCallib(int cnt_callib)
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Jangan gantung");
        lcd.setCursor(0, 1);
        lcd.print("botol infus");
        lcd.setCursor(13, 1);
        lcd.print(cnt_callib);
    }

    void weighCallibrated()
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Gantung botol");
        lcd.setCursor(0, 1);
        lcd.print("infus");
    }

    void sample(int tpm, int weigh)
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("TPM :");
        lcd.setCursor(6, 0);
        lcd.print(tpm);
        lcd.setCursor(0, 1);
        lcd.print("Weigh :");
        lcd.setCursor(8, 1);
        lcd.print(weigh);
        lcd.setCursor(10, 1);
        lcd.print("gr");
    }

    void wiFiNotConnected()
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("WiFi not");
        lcd.setCursor(0, 1);
        lcd.print("connected");
    }

    void sendSim()
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Sending data via");
        lcd.setCursor(0, 1);
        lcd.print("SIM card");
    }

    void regNetWorker()
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Registering");
        lcd.setCursor(0, 1);
        lcd.print("Network...");
    }
};

#endif
