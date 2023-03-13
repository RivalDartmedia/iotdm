//ESP32
//Memorr Lib Test

#include "mem_set.h"
// #include "sensor.h"
// #include "koneksi.h"
// #include "indikator.h"

InfusConfig config1;
LoadCellConfig load1;

void setup(){
    Serial.begin(115200);
    delay(5000);
    //STEP1: Init Memory
    init_fs();
    config1.load(LittleFS);
    config1.print();
    config1.edit(tokenID_p, "oekdoakos");
    Serial.printf("Template: %s\n",config1.get(temp_p));
    config1.print();
    config1.save(LittleFS);
    config1.load(LittleFS);
    config1.print();

    load1.load(LittleFS);
    Serial.println(load1.get());
    load1.edit(load1.get()+0.123);
    load1.save(LittleFS);
    Serial.println(load1.get());
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