//ESP32

// #include "mem_set.h"
// #include "sensor.h"
#include "koneksi_blynk.h"
// #include "indikator.h"

BlynkTimer SensorTimer;

void send_sens()
{
  float sens1 = tpm.get(), sens0 = weigh.get();
  Serial.printf("TPM : %.2f, weigh : %.2f\n", sens0, sens1);
  Blynk.virtualWrite(V0, sens0);
  Blynk.virtualWrite(V1, sens1);
  // Blynk.virtualWrite(V1, tpm.get());
  // Blynk.virtualWrite(V0, weigh.get());
}

void setup(){
    //STEP1: Init Memory

    //STEP2: Load Config

    //STEP2.1: Config if needed

    //STEP4: Init Connection
    Serial.begin(115200);
    delay(100);
    BlynkEdgent.begin();
    SensorTimer.setInterval(4000L, send_sens);

    //STEP5: Init Sensor

}

void loop() {
    //State Monitoring
    //STEP-M1: Connection Management
    BlynkEdgent.run();
    SensorTimer.run();

    //STEP-M2: Check Command

    //STEP-M3: Get Sensor Data

    //STEP-M4: Send Data

    //State Lost Connection
    //STEP-LC1: Connection Management

    //STEP-LC2: Show Error
}