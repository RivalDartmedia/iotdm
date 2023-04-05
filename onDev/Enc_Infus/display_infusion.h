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

    void hello()
    {
        lcd.clear();
        lcd.print("     Hello");
        lcd.setCursor(5, 1);
        lcd.print("Dunia!");
    }
};

#endif