//ESP32

// #include "memory.h"
// #include "sensor.h"
// #include "koneksi.h"
#include "indikator.h"

#define pin_led 2
#define pin_buzz 5

indicator main_indi;

void setup(){
    //STEP1: Init Memory

    //STEP2: Load Config

    //STEP2.1: Config if needed

    //STEP4: Init Connection

    //STEP5: Init Sensor

    main_indi.init(pin_led, pin_buzz);
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

    // Set Mode 
    // main_indi.set_state(off);
    // main_indi.set_state(blink_slow);
    // main_indi.set_state(blink_fast);
    // main_indi.set_state(on);
    //General
    //STEP-
    main_indi.show();
}