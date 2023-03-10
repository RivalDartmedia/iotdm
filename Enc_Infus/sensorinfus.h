#ifndef sensorinfus_h
#define sensorinfus_h

#include <Arduino.h>
#include "soc/rtc.h"
#include <HX711.h>

class Tpm
{
private:
    int sensor_pin;
    float tpm_val;
    byte lastReading;
    unsigned long lastDebounceTime;
    unsigned long debounceDelay = 20;
    unsigned long notupdatelim = 6000;

public:
    void init(int sensor_pin)
    {
        pinMode(sensor_pin, INPUT_PULLUP);
    }

    void update()
    {
        // debounce handler
        byte newReading = digitalRead(sensor_pin);

        if (newReading != lastReading)
        {
            lastDebounceTime = millis();
        }
        else if (millis() - lastDebounceTime > debounceDelay)
        {
            // Update tpm
            tpm_val = 60000 / (millis() - lastDebounceTime);
        }
        lastReading = newReading;
    }

    float get()
    {
        if (millis() - lastDebounceTime > notupdatelim){
            //tpm tidak terupdate setelah notupdatelim ms, return 0
            tpm_val = 0;
        }
        return tpm_val;
    }
};

class Weigh
{
private:
    // HX711 circuit wiring
    HX711 scale;
    float callib_param;

public:
    void init(int LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN)
    {
        // Start I2C dkk
        rtc_clk_cpu_freq_set(RTC_CPU_FREQ_80M);
        scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
    }

/**
 * @brief Proses untuk kalibrasi sensor berat
 * 
 */
    void callib()
    {
        bool isdone = false;
        while (!isdone)
        {
            if (scale.is_ready())
            {
                scale.set_scale();
                Serial.println("Tare... remove any weights from the scale.");
                delay(5000);
                scale.tare();
                Serial.println("Tare done...");
                Serial.print("Place a known weight on the scale...");
                delay(5000);
                long reading = scale.get_units(10);
                Serial.print("Result: ");
                Serial.println(reading);
                bool waitserial = 1;
                while(waitserial){
                    Serial.print("Input Berat:");
                    delay(2000);
                    while (Serial.available() > 0)
                    {
                        int dataIn = Serial.parseInt();
                        //Do something with the data - like print it
                        Serial.printf("Input Berat %d\n",dataIn);
                        
                        //Tolak dataIn = 0
                        if (dataIn == 0) Serial.print(F("Berat tidak boleh 0\n"));
                        else waitserial = 0;
                    }
                }
                callib_param = reading/dataIn;
                scale.set_scale(callib_param);
                //Sensor terkalibrasi
                isdone = 1;
            }
            else
            {
                Serial.println("HX711 not found.");
            }
            delay(1000);
        }
    }
    float get_callib()
    {
        return scale.get_units(20);
    }

    float get_raw()
    {
        return scale.get_value(20);
    }
    
    void tare()
    {
        return scale.tare();
    }
};

#endif