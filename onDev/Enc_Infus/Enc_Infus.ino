//ESP32
#include "mem_set.h"
#include "sensorinfus.h"
#include "koneksi_wifi.h"
#include "koneksi_sim.h"
#include "indikator.h"
#include "display_infusion.h"

InfusConfig config1;
ConnectionWiFi connect1;
ConnectionSIM sim;
indi_state main_indicator;
Tpm tpm;
Weigh weigh;
LoadCellConfig loadconfig;
Button button;
Display disp;

#define tpm_pin 18
#define LOADCELL_DOUT_PIN 4
#define LOADCELL_SCK_PIN 2
#define configWiFiButton 19

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
    
    //STEP1: Init
    button.init(configWiFiButton);
    disp.init();
    init_fs();
    
    //STEP2: Load Config
    config1.load(LittleFS);
    config1.print();

    //STEP2.1: Config if needed

    //STEP4: Init Connection
    connect1.connectWifi(config1);
    if(connect1.checkwifi()){
        Serial.println("WiFi Connected");
        disp.print("WiFi Connected");
        delay(2000);
    }else{ //Cek bisa sim atau tidak
        Serial.println("Wifi Not Connected");
        Serial.println("Starting Captive Portal...");
        disp.wiFiNotConnected();
        delay(2000);
        disp.print("Setting WiFi...");
        start_portal(config1);
        vTaskDelay(1);
    }

    int cnt_config_lim = 0, cnt_config = 10;
    while(cnt_config > cnt_config_lim && !button.is_push()){
        Serial.println("Setting WiFi ?");
        disp.settingWiFi(cnt_config);
        cnt_config--;
        delay(1000);
    }
    if (cnt_config > cnt_config_lim){
            Serial.println("Starting Captive Portal...");
            disp.print("Setting WiFi...");
            start_portal(config1);
            vTaskDelay(1);
    }
    // config1.print();
    config1.edit(tokenID_p, "2nrtIgwDCHP5SF3CToAWWdWZFPGtz6oX");
    config1.save(LittleFS);

    Serial.println("Setup SIM...");
    disp.print("Setup SIM...");
    sim.init();

    //STEP5: Init Sensor
    beginsens();
    
    //Callib and Save
    // weigh.callib(); // Lakukan proses callib, atau Load
    // Serial.print(weigh.get_scale());
    // loadconfig.edit(weigh.get_scale());

    // //Save to Config
    // loadconfig.save(LittleFS);

    //Load and Callibr
    int weigh_callib_lim = 0, weigh_callib = 10;
    while(weigh_callib > weigh_callib_lim){
        Serial.println(weigh_callib);
        disp.weighCallib(weigh_callib);
        delay(1000);
        weigh_callib--;
    }
    loadconfig.load(LittleFS);
    Serial.println("");
    weigh.set_callib(loadconfig.get());
    Serial.printf("Load Param: %f", loadconfig.get());
    Serial.println("");
    disp.weighCallibrated();
    delay(5000);
}

void loop() {
    //State Monitoring
    
    //STEP-M1: Get Sensor Data
    float val_sample_berat = weigh.get_unit();
    int val_sample_tpm = tpm.get();
    Serial.print("TPM: ");
    Serial.println(val_sample_tpm);
    Serial.printf("Weigh: %3.f\n", val_sample_berat);

    disp.sample(val_sample_tpm, val_sample_berat);
    delay(2000);

    connect1.connectWifi(config1);

    //STEP-M2: Connection Management
    //STEP-M3: Send Data n Update Indicator
    if(connect1.checkwifi()){
        connect1.update_secure(config1, val_sample_tpm, val_sample_berat, main_indicator);
        delay(2500);
    }else{ //Cek bisa sim atau tidak
        disp.wiFiNotConnected();
        delay(2000);
        disp.sendSim();
        Serial.println("KONEKSI SIM");
        vTaskDelay(1);
        sim.connect_gprs();
        delay(1000);
    }
    // else {
        // Serial.println("KONEKSI GAGAL");
        //State Lost Connection
        //STEP-LC1: Show Error
    // }
    // delay(1000);
}
