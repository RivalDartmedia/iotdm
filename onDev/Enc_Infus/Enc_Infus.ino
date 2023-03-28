//ESP32
#include "mem_set.h"
#include "sensorinfus.h"
#include "koneksi_wifi.h"
// #include "koneksi_sim.h"
#include "indikator.h"

InfusConfig config1;
ConnectionWiFi connect1;
// ConnectionSIM sim;
indi_state main_indicator;
Tpm tpm;
Weigh weigh;
LoadCellConfig loadconfig;

#define tpm_pin 21
#define LOADCELL_DOUT_PIN 4
#define LOADCELL_SCK_PIN 2

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
    //STEP0:
    Serial.begin(9600);

    //STEP1: Init Memorya
    init_fs();
    
    //STEP2: Load Config
    // sim.init();
    config1.load(LittleFS);
    config1.print();

    //STEP2.1: Config if needed

    //STEP4: Init Connection
    // start_portal(config1);
    config1.print();
    config1.edit(tokenID_p, "2nrtIgwDCHP5SF3CToAWWdWZFPGtz6oX");
    config1.save(LittleFS);

    //STEP5: Init Sensor
    beginsens();
    
    //Callib and Save
    // weigh.callib(); // Lakukan proses callib, atau Load
    // Serial.print(weigh.get_scale());
    // loadconfig.edit(weigh.get_scale());

    // //Save to Config
    // loadconfig.save(LittleFS);

    //Load and Callibr
    loadconfig.load(LittleFS);
    weigh.set_callib(loadconfig.get());
    Serial.printf("Load Param: %f", loadconfig.get());
}

void loop() {
    //State Monitoring
    
    //STEP-M1: Get Sensor Data
    float val_sample_berat = weigh.get_unit();
    int val_sample_tpm = tpm.get();
    Serial.print("TPM: ");
    Serial.println(val_sample_tpm);
    Serial.printf("Weigh: %3.f\n", val_sample_berat);
    connect1.connectWifi(config1);
    // connect1.update_secure(config1, val_sample_tpm, val_sample_berat, main_indicator);
    // sim.connect_gprs();
    Serial.println(connect1.checkwifi());

    //STEP-M2: Connection Management
    //STEP-M3: Send Data n Update Indicator
    if(connect1.checkwifi()){
        // connect1.connectWifi(config1);
        Serial.println("WIFI TERKONEKSI");
        connect1.update_secure(config1, val_sample_tpm, val_sample_berat, main_indicator);
    // }else if (false) { //Cek bisa sim atau tidak
    //     sim.connect_gprs();
    }
    else {
        Serial.println("WIFI GAGAL");
        //State Lost Connection
        //STEP-LC1: Show Error
    }
    delay(2000);
}
