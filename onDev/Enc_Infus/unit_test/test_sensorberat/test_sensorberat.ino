//ESP32
//Sensor Lib Test

#include "mem_set.h"
#include "sensorinfus.h"
// #include "koneksi.h"
// #include "indikator.h"
// #include "soc/rtc_wdt.h"

#define tpm_pin 21
#define LOADCELL_DOUT_PIN 4
#define LOADCELL_SCK_PIN 2

Tpm tpm;
Weigh weigh;
LoadCellConfig loadconfig;

void updatetpm()
{
  tpm.update();
}

void beginsens(){
    tpm.init(tpm_pin);
    attachInterrupt(tpm_pin, updatetpm, FALLING);
    weigh.init(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
}

void setup(){
    Serial.begin(9600);
    // rtc_wdt_protect_off();
    // rtc_wdt_disable();
    //STEP1: Init Memory
    init_fs();
    //STEP2: Load Config

    //STEP2.1: Config if needed

    //STEP4: Init Connection

    //STEP5: Init Sensor
    beginsens();
    
    //Callib and Save
    weigh.callib(); // Lakukan proses callib, atau Load
    Serial.print(weigh.get_scale());
    loadconfig.edit(weigh.get_scale());

    //Save to Config
    loadconfig.save(LittleFS);

    //Load and Callibr
    // loadconfig.load(LittleFS);
    // weigh.set_callib(loadconfig.get());
    // Serial.printf("Load Param: %f", loadconfig.get());
}

void loop() {
    // rtc_wdt_protect_off();
    // rtc_wdt_disable();
    //State Monitoring
    //STEP-M1: Connection Management

    //STEP-M2: Check Command

    //STEP-M3: Get Sensor Data
    Serial.printf("TPM: %.2f\n", tpm.get());
    Serial.printf("Weigh: %3.f\n", weigh.get_unit());

    //STEP-M4: Send Data

    //State Lost Connection
    //STEP-LC1: Connection Management

    //STEP-LC2: Show Error
//    delay(2000);
    delay(50);
}
