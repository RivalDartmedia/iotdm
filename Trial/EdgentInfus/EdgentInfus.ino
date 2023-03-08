/*************************************************************
  Blynk is a platform with iOS and Android apps to control
  ESP32, Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build mobile and web interfaces for any
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: https://www.blynk.io
    Sketch generator:           https://examples.blynk.cc
    Blynk community:            https://community.blynk.cc
    Follow us:                  https://www.fb.com/blynkapp
                                https://twitter.com/blynk_app

  Blynk library is licensed under MIT license
 *************************************************************
  Blynk.Edgent implements:
  - Blynk.Inject - Dynamic WiFi credentials provisioning
  - Blynk.Air    - Over The Air firmware updates
  - Device state indication using a physical LED
  - Credentials reset using a physical Button
 *************************************************************/

/* Fill in information from your Blynk Template here */
/* Read more: https://bit.ly/BlynkInject */
#define BLYNK_TEMPLATE_ID "TMPLbe3jTz-2"
#define BLYNK_TEMPLATE_NAME "Infus IoT"

#define BLYNK_FIRMWARE_VERSION        "0.1.0"

#define BLYNK_PRINT Serial
//#define BLYNK_DEBUG

#define APP_DEBUG

// Uncomment your board, or configure a custom board in Settings.h
#define USE_ESP32_DEV_MODULE
//#define USE_ESP32C3_DEV_MODULE
//#define USE_ESP32S2_DEV_KIT
//#define USE_WROVER_BOARD
//#define USE_TTGO_T7
//#define USE_TTGO_T_OI

#define BOARD_LED_PIN 2

#include "BlynkEdgent.h"
#include "Sensing.h"
#include "Indicator.h"

#define tpm_pin 23

BlynkTimer SensorTimer;

Tpm tpm(tpm_pin);
Weigh weigh;

void updatetpm()
{
    tpm.update();
}

void begin_sens(int callib_param)
{
    attachInterrupt(tpm_pin, updatetpm, FALLING);
    weigh.callib(callib_param);
}

void send_sens()
{
  float sens0 = tpm.get(), sens1 = weigh.get();
  Serial.printf("TPM : %.2f, weigh : %.2f\n", sens0, sens1);
  Blynk.virtualWrite(V1, sens0);
  Blynk.virtualWrite(V0, sens1);
  // Blynk.virtualWrite(V1, tpm.get());
  // Blynk.virtualWrite(V0, weigh.get());
}

void setup()
{
  Serial.begin(115200);
  delay(100);
  BlynkEdgent.begin();
  SensorTimer.setInterval(5000L, send_sens);
  begin_sens(tpm_pin, 12);

}

void loop() {
  BlynkEdgent.run();
  SensorTimer.run();
}
