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
    unsigned long debounceDelay = 40;

public:
    // Constructor Declaration
    Tpm(int sensor_pin)
    {
        this->sensor_pin = sensor_pin;
        init();
    }

    void init()
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
        tpm += 10;
    }

    float get()
    {
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
        init();
    }

    void init()
    {
        // pinMode(sensor_pin, INPUT_PULLUP);
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