#ifndef display_infusion_h
#define display_infusion_h

#include <Arduino.h>
#include <LCD_I2C.h>

LCD_I2C lcd(0x27, 16, 2);

class Display
{
public:
    void init()
    {
        lcd.begin();
        lcd.backlight();
    }

    void tes()
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("TES");
    }

    void print(String text)
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(text);
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
};

#endif
