//-----------ESP32
#include "mem_set.h"
#include "sensorinfus.h"
#include "koneksi_wifi.h"
#include "koneksi_sim.h"
#include "display_led.h"
#include "buzzer.h"

//-----------Buat object dari class
InfusConfig config1;
ConnectionWiFi connect1;
ConnectionSIM sim;
Tpm tpm;
Weigh weigh;
LoadCellConfig loadconfig;
Button button;
DisplayLed displed;
buzzer buzz;
Bat bat;

//-----------Inisialisasi pin dan variabel
#define tpm_pin 18
#define LOADCELL_DOUT_PIN 4
#define LOADCELL_SCK_PIN 2
#define configWiFiButton 19
#define pinBat 35
#define pinBuzz 5
bool pauseState;
bool pauseBeep;
unsigned long buttonPressStartTime = 0;

void IRAM_ATTR updatetpm()
{
  tpm.update();
}

void beginsens(){
    tpm.init(tpm_pin);
    attachInterrupt(tpm_pin, updatetpm, FALLING);
    weigh.init(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
}

void pauseMonitoring(){
    if(button.is_push()){
        unsigned long currentTime = millis();
        if ((currentTime - buttonPressStartTime) >= 500) {
            Serial.println("PAUSE");
            detachInterrupt(configWiFiButton);
            pauseState = HIGH;
            pauseBeep = HIGH;
        }
    }
}

void setup(){
    //-----------STEP0: begin serial communication
    Serial.begin(115200);
    
    //-----------STEP1: Init
    button.init(configWiFiButton);
    displed.init();
    init_fs();
    buzz.init(pinBuzz);
    bat.init(pinBat);
    
    //-----------STEP2: Load Config
    displed.print("   Mulai", 0, 0);
    delay(1000);
    displed.print("Cek memoriWiFi...", 0, 0);
    config1.load(LittleFS);
    config1.print();

    //-----------STEP3: Init Connection
    connect1.connectWifi(config1);

    //-----------STEP4: Config if needed and check wifi connection
    if(connect1.checkwifi()){
        Serial.println("WiFi Connected");
        String wifi = config1.get(wifi_ssid_p);
        Serial.println(wifi);
        wifi = wifi.substring(0,10);
        displed.wifiCon(wifi);
        buzz.buzzbeep(500);
        delay(1500);
    }else{
        Serial.println("Wifi Not Connected");
        displed.print("WiFi tidaktersambung", 0, 0);
        delay(2000);
        Serial.println("Starting Captive Portal...");
        displed.print("Mengatur  WiFi...", 0, 0);
        buzz.buzzbeep(500);
        start_portal(config1);
        connect1.connectWifi(config1);
        if (connect1.checkwifi()){
            Serial.println("WiFi Connected");
            String wifi = config1.get(wifi_ssid_p);
            Serial.println(wifi);
            wifi = wifi.substring(0,10);
            displed.wifiCon(wifi);
            buzz.buzzbeep(500);
            delay(1500);
        } else {
            Serial.println("Wifi Not Connected");
            displed.print("WiFi tidaktersambung", 0, 0);
            delay(2000);
            displed.print("Restart...", 0, 0);
            buzz.buzzbeep(500);
            delay(1500);
            esp_task_wdt_init(1, true);
            esp_task_wdt_add(NULL);
            ESP.restart();
        }
        vTaskDelay(1);
    }

    int cnt_config = 5;
    unsigned long previousMillis = 0;
    unsigned long interval = 1000;
    while(cnt_config > -1 && !button.is_push()){
        unsigned long currentMillis = millis();
        if (currentMillis - previousMillis >= interval) {
            previousMillis = currentMillis;
    
            if (cnt_config > -1) {
                Serial.print("Setting WiFi ? ");
                Serial.println(cnt_config);
                displed.settingWiFi(cnt_config);
                cnt_config--;
            }       
        }
    }
    if (cnt_config > -1){
        Serial.println("Starting Captive Portal...");
        displed.print("Mengatur  WiFi...", 0, 0);
        buzz.buzzbeep(500);
        start_portal(config1);
        connect1.connectWifi(config1);
        if (!connect1.checkwifi()){
            Serial.println("Wifi Not Connected");
            displed.print("WiFi tidaktersambung", 0, 0);
            delay(2000);
            displed.print("Restart...", 0, 0);
            buzz.buzzbeep(500);
            delay(1500);
            esp_task_wdt_init(1, true);
            esp_task_wdt_add(NULL);
            ESP.restart();
        }
        vTaskDelay(1);
    }
    Serial.println("WiFi Connected");
    String wifi = config1.get(wifi_ssid_p);
    Serial.println(wifi);
    wifi = wifi.substring(0,10);
    displed.wifiCon(wifi);
    buzz.buzzbeep(500);
    delay(500);
    config1.print();
    // config1.edit(tokenID_p, "2nrtIgwDCHP5SF3CToAWWdWZFPGtz6oX");
    // config1.save(LittleFS);

    //-----------STEP5: Setup SIM Card
    Serial.println("Setup SIM...");
    displed.print("MenyiapkanSIM...", 0, 0);
    sim.init();
    buzz.buzzbeep(500);
    delay(1500);

    //-----------STEP6: Init Sensor
    beginsens();
    
    //--------------Callib and Save----------------------
    // weigh.callib(); // Lakukan proses callib, atau Load
    // Serial.print(weigh.get_scale());
    // loadconfig.edit(weigh.get_scale());

    // //Save to Config
    // loadconfig.save(LittleFS);
    //---------------------------------------------------

    //-------------Load and Callibr---------------------
    int weigh_callib_lim = 0, weigh_callib = 10;
    while(weigh_callib > weigh_callib_lim){
        Serial.println(weigh_callib);
        displed.weighCallib(weigh_callib);
        buzz.buzzbeep(500);
        delay(500);
        weigh_callib--;
    }
    loadconfig.load(LittleFS);
    Serial.println("");
    weigh.set_callib(loadconfig.get());
    Serial.printf("Load Param: %f", loadconfig.get());
    Serial.println("");

    displed.print("Gantung   infus !", 0, 0);
    buzz.buzzbeep(1000);
    delay(1000);
    //-------------------------------------------------
}

void loop() {
    //State Monitoring
    attachInterrupt(configWiFiButton, pauseMonitoring, FALLING);
    
    //-----------STEP-M1: Get Sensor Data & Displaying
    int val_sample_berat = weigh.get_unit();
    if (val_sample_berat < 0){
      val_sample_berat = 0;
    }
    int val_sample_tpm = tpm.get();
    Serial.print("TPM: ");
    Serial.println(val_sample_tpm);
    Serial.print("Weigh: ");
    Serial.println(val_sample_berat);
    
    displed.sample(val_sample_tpm, val_sample_berat);

    delay(2000);

    // connect1.connectWifi(config1);

    //-----------STEP-M2: Connection Management & Send Data
    if(connect1.checkwifi()){
        if(connect1.update_secure(config1, val_sample_tpm, val_sample_berat) != 200){
            buzz.buzzbeep(500);
            delay(2000);
        } else {
            delay(2500);
        }
    }else{ //Cek bisa sim atau tidak
        displed.print("WiFi tidaktersambung", 0, 0);
        delay(2000);
        displed.print("Kirim datavia SIM", 0, 0);
        Serial.println("KONEKSI SIM");
        vTaskDelay(1);
        if (sim.connect_gprs(config1, val_sample_tpm, val_sample_berat) != 200){
            displed.print("Kirim datagagal", 0, 0);
            buzz.buzzbeep(500);
            delay(500);
        } else {
            delay(1000);
        }
    }
    if (bat.cek()){
        displed.print("Battery   Low", 0, 0);
        buzz.buzzbeep(1000);
    }
    while(pauseState == HIGH){
        displed.print("  PAUSED", 0, 0);
        if(pauseBeep == HIGH){
            buzz.buzzbeep(500);
        }
        if(pauseBeep == HIGH){
            pauseBeep = LOW;
        }
        if(button.is_push()){
            delay(1000);
            detachInterrupt(configWiFiButton);
            pauseState = LOW;
        }
    }
}
