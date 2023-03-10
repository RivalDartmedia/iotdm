//ESP32
//Sensor Lib Test

// #include "mem_set.h"
#include "sensorinfus.h"
// #include "koneksi.h"
// #include "indikator.h"

#define tpm_pin 23
#define LOADCELL_DOUT_PIN 17
#define LOADCELL_SCK_PIN 18

Tpm tpm;
Weigh weigh;

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
    Serial.begin(115200);
    //STEP1: Init Memory

    //STEP2: Load Config

    //STEP2.1: Config if needed

    //STEP4: Init Connection

    //STEP5: Init Sensor
    beginsens();
    weigh.callib(); // Lakukan proses callib, atau Load
    // LoadCellConfig loadconfig;
    // weigh.set_callib(loadconfig.get(););

}

void loop() {
    //State Monitoring
    //STEP-M1: Connection Management

    //STEP-M2: Check Command

    //STEP-M3: Get Sensor Data
    Serial.printf("TPM: %.2f", tpm.get());
    Serial.printf("Weigh: %.4f", weigh.get_unit());

    //STEP-M4: Send Data

    //State Lost Connection
    //STEP-LC1: Connection Management

    //STEP-LC2: Show Error
    delay(2000);
}