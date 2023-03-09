#ifndef Sensing_h
#define Sensing_h

#include "Arduino.h"
class Tpm
{
private:
    int sensor_pin;
    float tpm_val;
    byte lastReading;
    unsigned long lastDebounceTime;
    unsigned long debounceDelay = 20;

public:
    // Constructor Declaration
    Tpm()
    {
    }

    void init(int sensor_pin)
    {
        pinMode(sensor_pin, INPUT_PULLUP);
    }

    void update()
    {
        // debounce handler
        // byte newReading = digitalRead(sensor_pin);

        // if (newReading != lastReading)
        // {
        //     lastDebounceTime = millis();
        // }
        // else if (millis() - lastDebounceTime > debounceDelay)
        // {
        //     // Update tpm
        //     tpm_val = 60000/(millis() - lastDebounceTime);
        // }
        // lastReading = newReading;
        tpm_val += 10;
    }

    float get()
    {
        // Untuk test
        update();
        return tpm_val;
        // tpm_val = 0; // reset setelah didapat
    }
};

class Weigh
{
private:
    int sensor_pin;
    float weigh_val;
    int callib_param;

public:
    // Constructor Declaration
    Weigh()
    {
    }

    void init(int param)
    {
        // Start I2C dkk
        callib(param);

    }

    void callib(int param)
    {
        // debounce handler
        callib_param = param;
    }
    void update()
    {
        // debounce handler
        weigh_val += callib_param;
    }

    float get()
    {
        update();
        return weigh_val;
    }
};

#endif