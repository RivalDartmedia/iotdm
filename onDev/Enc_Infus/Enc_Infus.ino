//ESP32
#include "mem_set.h"
// #include "sensor.h"
#include "koneksi.h"
// #include "indikator.h"

InfusConfig config1;
ConnectionManager connect1;

void setup(){
    //STEP0:
    Serial.begin(115200);

    //STEP1: Init Memorya
    init_fs();
    
    //STEP2: Load Config
    config1.load(LittleFS);

    //STEP2.1: Config if needed

    //STEP4: Init Connection
    connect1.start_portal(config1);

    //STEP5: Init Sensor

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
    delay(50);
}