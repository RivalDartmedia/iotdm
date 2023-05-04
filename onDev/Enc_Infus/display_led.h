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

    void print(String text)
    {
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0,0);
        display.println(text);
        // display.setCursor(0,10);
        // display.println("Juragan");
        display.display();
    }

    void connectingWiFi(String text)
    {
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0,0);
        display.println("SSID :");
        display.setCursor(0,16);
        display.println(text);
        display.display();
    }

    void settingWiFi(int setWiFi)
    {
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0,0);
        display.println("Set ulang WiFi ?");
        display.setCursor(80,16);
        display.println(setWiFi);
        display.display();
    }

    void weighCallib(int cnt_callib)
    {
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0,0);
        display.println("Angkat    infus !");
        display.setCursor(90,16);
        display.println(cnt_callib);
        display.display();
    }

    void print_sample(int text)
    {
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0,0);
        display.println(text);
        display.display();
    }
};

#endif
