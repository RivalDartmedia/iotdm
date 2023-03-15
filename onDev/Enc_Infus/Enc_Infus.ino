//ESP32
//Button Test
//Fokus sistem monitoring tanpa koneksi

// #include "mem_set.h"
#include "sensorinfus.h"
// #include "koneksi.h"
// #include "indikator.h"

#define tpm_pin 21
#define sett_pin 23
#define tare_pin 19
#define led_pin 15
#define LOADCELL_DOUT_PIN 4
#define LOADCELL_SCK_PIN 2

Tpm tpm;
Weigh weigh;
// LoadCellConfig loadconfig;
Button data_edit, weigh_set;

void ICACHE_RAM_ATTR updatetpm()
{
  tpm.update();
}

void ICACHE_RAM_ATTR updateedit()
{
  data_edit.update();
}

void ICACHE_RAM_ATTR updatetare()
{
  weigh_set.update();
}

void beginsens(){
    tpm.init(tpm_pin);
    attachInterrupt(tpm_pin, updatetpm, FALLING);
    weigh.init(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
    data_edit.init(sett_pin);
    attachInterrupt(sett_pin, updateedit, CHANGE);
    weigh_set.init(tare_pin);
    attachInterrupt(tare_pin, updatetare, CHANGE);
}

void setup(){
    Serial.begin(115200);
    //STEP1: Init Memory
    // init_fs();

    //STEP2: Init Sensor
    beginsens();

    //STEP3.0: Config if needed
    
    //STEP3: Load Config
    

    //STEP4: Init Connection
    
    //Callib and Save
    // weigh.callib(); // Lakukan proses callib, atau Load
    // Serial.print(weigh.get_scale());
    // loadconfig.edit(weigh.get_scale());
    // //Save to Config
    // loadconfig.save(LittleFS);

    //Load and Callibr
    // loadconfig.load(LittleFS);
    // weigh.set_callib(loadconfig.get());
    // // loadconfig.edit(weigh.get_scale());
    // // weigh.set_callib(loadconfig.get());
    // // Serial.printf("Load Param: %f", loadconfig.get());
    // // loadconfig.edit(weigh.get_scale());

    // loadconfig.save(LittleFS);
    // LoadCellConfig loadconfig;
    // weigh.set_callib(loadconfig.get(););

}

void loop() {
    //State Monitoring
    //STEP-M1: Connection Management

    //STEP-M2: Check Command
    Serial.print("DATA");
    data_edit.print();
    Serial.print("TARE");
    weigh_set.print();

    //STEP-M3: Get Sensor Data
    // Serial.printf("TPM: %.2f", tpm.get());
    // Serial.printf("Weigh: %3.f\n", weigh.get_unit());

    //STEP-M4: Send Data

    //State Lost Connection
    //STEP-LC1: Connection Management

    //STEP-LC2: Show Error
    delay(100);
}