//ESP32
#include "mem_set.h"
// #include "sensor.h"
#include "koneksi_wifi.h"
#include "indikator.h"

InfusConfig config1;
ConnectionWiFi connect1;
  int val_sample_berat = 600;
  double val_sample_tpm = 18.45;
indi_state main_indicator;

void setup(){
    //STEP0:
    Serial.begin(115200);

    //STEP1: Init Memorya
    init_fs();
    
    //STEP2: Load Config
    config1.load(LittleFS);
    config1.print();

    //STEP2.1: Config if needed

    //STEP4: Init Connection
    // start_portal(config1);
    // config1.print();
    //STEP5: Init Sensor

}

void loop() {
    //State Monitoring
    //STEP-M1: Connection Management
    connect1.update_secure(config1, val_sample_tpm, val_sample_berat, main_indicator);
    //STEP-M2: Check Command

    //STEP-M3: Get Sensor Data

    //STEP-M4: Send Data

    //State Lost Connection
    //STEP-LC1: Connection Management

    //STEP-LC2: Show Error
    delay(5000);
}