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
