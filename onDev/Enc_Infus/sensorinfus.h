#ifndef sensorinfus_h
#define sensorinfus_h

#include <Arduino.h>
#include <HX711.h>
#include "buzzer.h"

buzzer buzzbutton;

class Tpm
{
private:
    int sensor_pin;
    int tpm_val;
    byte lastReading;
    unsigned long lastDebounceTime;
    unsigned long debounceDelay = 20;
    unsigned long notupdatelim = 60000;

public:
    void init(int sensor_pin)
    {
        this->sensor_pin = sensor_pin;
        pinMode(sensor_pin, INPUT_PULLUP);
    }

    /**
     * @brief Fungsi update dipanggil setiap interupsi.
     *
     */
    void update()
    {
        int val_sample_tpm_previous = get();
        // debounce handler
        byte newReading = digitalRead(this->sensor_pin);

        if (newReading != lastReading)
        {
            lastDebounceTime = millis();
        }
        else if (millis() - lastDebounceTime > debounceDelay)
        {
            // Update tpm
            tpm_val = (int) (60000 / (millis() - lastDebounceTime));
            // tpm_val = (float)(60000 / (millis() - lastDebounceTime));

            // int val_sample_tpm_previous = get();
            int difference = tpm_val - val_sample_tpm_previous;
            if (difference > 25 || difference < -25){
                tpm_val = val_sample_tpm_previous;
                Serial.println("selisihnya besar");
                Serial.println(difference);
            }
            val_sample_tpm_previous = tpm_val;
        }
        lastReading = newReading;
    }

    int get()
    {
        if (millis() - lastDebounceTime > notupdatelim)
        {
            // tpm tidak terupdate setelah notupdatelim ms, return 0
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
    bool callibrated = false;

public:
    void init(int LOADCELL_DOUT_PIN, int LOADCELL_SCK_PIN)
    {
        scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
    }

    /**
     * @brief Proses untuk kalibrasi sensor berat
     *
     */
    void callib()
    {
        callibrated = false;
        while (!callibrated)
        {
            if (scale.is_ready())
            {
                scale.set_scale();
                Serial.println("Tare... remove any weights from the scale.");
                delay(5000);
                scale.tare();
                Serial.println("Tare done...");
                Serial.print("Place a known weight on the scale...");
                bool waitserial = 1;
                int dataIn;
                long reading;
                String str;
                while (waitserial)
                {
                    reading = scale.get_units(10);
                    Serial.printf("Result: %d. Berat asli?\n", reading);
                    delay(1000);
                    while (Serial.available() > 0)
                    {
                        dataIn = Serial.parseInt();
                        str = Serial.readStringUntil('\n');
                        // Do something with the data - like print it
                        Serial.printf("Input Berat %d\n", dataIn);

                        // Tolak dataIn = 0
                        if (dataIn == 0)
                            Serial.print(F("Berat tidak boleh 0\n"));
                        else
                            waitserial = 0;
                    }
                }
                callib_param = (float)reading / dataIn;
                Serial.printf("Callib Param: %f", callib_param);
                scale.set_scale(callib_param);
                // Sensor terkalibrasi
                callibrated = 1;
            }
            else
            {
                Serial.println("HX711 not found.");
            }
            delay(1000);
        }
    }
    void set_callib(float callib_param)
    {
        while (!scale.is_ready())
        {
            Serial.print(F("Wait scale ready\n"));
            delay(100);
        }
        Serial.println("Tare... remove any weights from the scale.");
        delay(5000);
        scale.tare();
        this->callib_param = callib_param;
        scale.set_scale(callib_param);
    }
    float get_scale()
    {
        return this->callib_param;
    }
    float get_unit()
    {
        return scale.get_units(5);
    }

    double get_raw()
    {
        return scale.get_value(5);
    }

    void tare()
    {
        return scale.tare();
    }
};

class Button
{
private:
    int button_pin;
    bool pushed, hold, start_hold;
    unsigned long lastDebounceTime, startHoldTime;
    unsigned long debounceDelay = 50;
    unsigned long holdtime = 5000, holddur = 10000;

public:
    void init(int button_pin)
    {
        this->button_pin = button_pin;
        pinMode(button_pin, INPUT_PULLUP);
    }
    void print(){
        Serial.printf("IsH: %d, IsP:%d, H_pin:%d\n", is_hold(), is_push(), pushed);
    }
    /**
     * @brief Fungsi update dipanggil setiap interupsi.
     *
     */
    void update()
    {
        // debounce handler
        bool newReading;
        static bool lastReading;
        unsigned long now_t = millis();
        if ((now_t - lastDebounceTime) > debounceDelay)
        {
            newReading = digitalRead(this->button_pin); // Pull up
            if (newReading != lastReading)
            {
                // Debouncer 
                //Sebelumnya tidak dipush dan sekarang dipush
                if (!pushed && !newReading){
                    startHoldTime = now_t;
                    start_hold = 1;
                }else if (!pushed){
                    start_hold = 0;
                }
                lastReading = newReading;
            }
            pushed = !newReading;
        }
        lastDebounceTime = now_t;
    }

    bool is_hold()
    {
        return start_hold && (millis() - startHoldTime > holdtime) && (millis() - startHoldTime < holddur);
    }
    bool is_push()
    {
        return !digitalRead(this->button_pin);
    }
};

class Bat
{
    private:
    int bat_pin;
    int led_pin;
    int ledState = LOW;
    unsigned long previousTime = 0;
    const long interval = 500;

    public:
    void init(int bat_pin, int led_pin)
    {
        this->bat_pin = bat_pin;
        this->led_pin = led_pin;
        pinMode(led_pin, OUTPUT);
    }

    void cek()
    {
        digitalWrite(led_pin, LOW);
        float bat_volt = analogRead(bat_pin);
        Serial.print("ADC : ");
        Serial.println(bat_volt);
        unsigned long currentTime = millis();
        if (bat_volt < 2000){
            if(currentTime - previousTime >= interval){
                previousTime = currentTime;
                if (ledState == LOW){
                    ledState = HIGH;
                } else {
                    ledState = LOW;
                }
                digitalWrite(led_pin, ledState);
            }
        }
    }
};

#endif
