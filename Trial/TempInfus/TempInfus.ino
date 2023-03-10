//ESP32
//Sensor Lib Test

// #include "memory.h"
#include "sensor.h"
// #include "koneksi.h"
// #include "indikator.h"

InfusConfig config1;

void setup(){
    Serial.begin(115200);
    //STEP1: Init Memory
    config1.init();
    config1.load();
    config1.print();
}

void loop() {
    //State Monitoring
    //STEP-M1: Connection Management

    //STEP-M2: Check Command

    //STEP-M3: Get Sensor Data

    //STEP-M4: Send Data

    //State Lost Connection
    //STEP-LC1: Connection Management

    //STEP-LC2: Show Error
}