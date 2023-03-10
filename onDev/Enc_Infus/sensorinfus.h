#ifndef sensorinfus_h
#define sensorinfus_h

#include <Arduino.h>
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

    /**
     * @brief Fungsi update dipanggil setiap interupsi.
     * 
     */
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
            tpm_val = (float) (60000 / (millis() - lastDebounceTime));
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
                delay(5000);
                long reading = scale.get_units(10);
                Serial.print("Result: ");
                Serial.println(reading);

                bool waitserial = 1;
                int dataIn;
                while(waitserial){
                    Serial.print("Input Berat:");
                    delay(2000);
                    while (Serial.available() > 0)
                    {
                        dataIn = Serial.parseInt();
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
                callibrated = 1;
            }
            else
            {
                Serial.println("HX711 not found.");
            }
            delay(1000);
        }
    }
    void set_callib(float callib_param){
        this->callib_param = callib_param;
    }
    float get_scale()
    {
        return scale.get_scale();
    }
    float get_unit()
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

class settingButton
{
private:
    int button_pin;
    byte lastReading;
    bool isHold, startHold;
    unsigned long lastDebounceTime, finishHoldTime;
    unsigned long debounceDelay = 20;
    unsigned long holdtime = 5000;

public:
    void init(int button_pin)
    {
        pinMode(button_pin, INPUT_PULLUP);
    }

    /**
     * @brief Fungsi update dipanggil setiap interupsi.
     * 
     */
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
            //Debouncer
            if(newReading == 0){
                //Tombol ditahan
                if(!startHold){
                    //Mulai ditekan
                    startHold = 1;
                    finishHoldTime = millis();
                }else {
                    if(millis() - holdtime > finishHoldTime){
                        //Setelah sekian lama, tombol terus ditekan
                        isHold = 1;
                    }
                }
            }
            else {
                //Reset Hold
                startHold = 0;
                isHold = 0;
            }
        }
        lastReading = newReading;
    }

    bool is_hold()
    {
        return isHold;
    }
    bool is_push()
    {
        return startHold;
    }
};

#endif