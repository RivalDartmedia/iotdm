//-----------ESP32
#include "mem_set.h"
#include "sensorinfus.h"
#include "koneksi_wifi.h"
#include "koneksi_sim.h"
// #include "indikator.h"
#include "display_led.h"
#include "buzzer.h"

//-----------Buat object dari class
InfusConfig config1;
ConnectionWiFi connect1;
ConnectionSIM sim;
// indi_state main_indicator;
Tpm tpm;
Weigh weigh;
LoadCellConfig loadconfig;
Button button;
DisplayLed displed;
buzzer buzz;

//-----------Inisialisasi pin
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
    //-----------STEP0: begin serial communication
    Serial.begin(9600);
    
    //-----------STEP1: Init
    button.init(configWiFiButton);
    displed.init();
    init_fs();
    buzz.init(5);
    
    //-----------STEP2: Load Config
    displed.print("Cek memoriWiFi...");
    config1.load(LittleFS);
    config1.print();

    //-----------STEP3: Init Connection
    connect1.connectWifi(config1);

    //-----------STEP4: Config if needed and check wifi connection
    if(connect1.checkwifi()){
        Serial.println("WiFi Connected");
        displed.print("WiFi      tersambung");
        delay(2000);
    }else{
        Serial.println("Wifi Not Connected");
        displed.print("WiFi tidaktersambung");
        delay(2000);
        Serial.println("Starting Captive Portal...");
        displed.print("Mengatur  WiFi...");
        buzz.buzzbeep(500);
        start_portal(config1);
        vTaskDelay(1);
    }

    int cnt_config = 10;
    unsigned long previousMillis = 0;
    unsigned long interval = 1000;
    while(cnt_config > 0 && !button.is_push()){
        unsigned long currentMillis = millis();
        if (currentMillis - previousMillis >= interval) {
            previousMillis = currentMillis;
    
            if (cnt_config > 0) {
                Serial.print("Setting WiFi ? ");
                Serial.println(cnt_config);
                displed.settingWiFi(cnt_config);
                cnt_config--;
            }
        }
    }
    if (cnt_config > 0){
            Serial.println("Starting Captive Portal...");
            displed.print("Mengatur  WiFi...");
            buzz.buzzbeep(500);
            start_portal(config1);
            vTaskDelay(1);
    }
    config1.print();
    // config1.edit(tokenID_p, "2nrtIgwDCHP5SF3CToAWWdWZFPGtz6oX");
    // config1.save(LittleFS);

    //-----------STEP5: Setup SIM Card
    Serial.println("Setup SIM...");
    displed.print("MenyiapkanSIM...");
    sim.init();

    //-----------STEP6: Init Sensor
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
        displed.weighCallib(weigh_callib);
        delay(1000);
        weigh_callib--;
    }
    loadconfig.load(LittleFS);
    Serial.println("");
    weigh.set_callib(loadconfig.get());
    Serial.printf("Load Param: %f", loadconfig.get());
    Serial.println("");

    displed.print("Gantung   infus !");
    // buzz.buzzonoff(2000);
    delay(2000);
}

void loop() {
    //State Monitoring
    
    //-----------STEP-M1: Get Sensor Data & Displaying
    float val_sample_berat = weigh.get_unit();
    int val_sample_tpm = tpm.get();
    Serial.print("TPM: ");
    Serial.println(val_sample_tpm);
    Serial.printf("Weigh: %3.f\n", val_sample_berat);
    
    displed.sample(val_sample_tpm, val_sample_berat);

    delay(2000);

    // connect1.connectWifi(config1);

    //-----------STEP-M2: Connection Management & Send Data
    if(connect1.checkwifi()){
        // connect1.update_secure(config1, val_sample_tpm, val_sample_berat, main_indicator);
        connect1.update_secure(config1, val_sample_tpm, val_sample_berat);
        delay(2500);
    }else{ //Cek bisa sim atau tidak
        displed.print("WiFi tidaktersambung");
        delay(2000);
        displed.print("Kirim datavia SIM");
        Serial.println("KONEKSI SIM");
        vTaskDelay(1);
        sim.connect_gprs(config1, val_sample_tpm, val_sample_berat);
        delay(1000);
    }
    // delay(1000);
}
