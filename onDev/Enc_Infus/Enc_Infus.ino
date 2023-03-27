//ESP32
#include "mem_set.h"
// #include "sensor.h"
#include "koneksi_wifi.h"
#include "indikator.h"

InfusConfig config1;
ConnectionWiFi connect1;
int val_sample_berat = 600;
double val_sample_tpm = 15.55;
indi_state main_indicator;

void setup(){
    //STEP0:
    Serial.begin(9600);

    //STEP1: Init Memorya
    init_fs();
    
    //STEP2: Load Config
    config1.load(LittleFS);
    config1.print();

    //STEP2.1: Config if needed

    //STEP4: Init Connection
    // start_portal(config1);
    config1.print();
    // config1.edit(tokenID_p, "ZIjaYVCHA9Vota0HFas5xh49JGXrM3Zy");
    config1.edit(tokenID_p, "2nrtIgwDCHP5SF3CToAWWdWZFPGtz6oX");
    config1.save(LittleFS);
    //STEP5: Init Sensor
}

void loop() {
    //State Monitoring
    
    //STEP-M1: Get Sensor Data
    connect1.update_secure(config1, val_sample_tpm, val_sample_berat, main_indicator);

    //STEP-M2: Connection Management
    //STEP-M3: Send Data n Update Indicator
    // if(connect1.checkwifi()){
        
    // }else if (false) { //Cek bisa sim atau tidak
    //     //Kirim lewat SIM
    // }
    // else {
        //State Lost Connection
        //STEP-LC1: Show Error
        delay(5000);
    // }
}
