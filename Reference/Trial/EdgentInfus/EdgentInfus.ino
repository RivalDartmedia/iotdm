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

#define tpm_pin 23

BlynkTimer SensorTimer;

Tpm tpm;
Weigh weigh;

void updatetpm()
{
  tpm.update();
}

void begin_sens(int callib_param)
{
  tpm.init(tpm_pin);
  attachInterrupt(tpm_pin, updatetpm, FALLING);
  weigh.callib(callib_param);
}

void send_sens()
{
  float sens1 = tpm.get(), sens0 = weigh.get();
  Serial.printf("TPM : %.2f, weigh : %.2f\n", sens0, sens1);
  Blynk.virtualWrite(V0, sens0);
  Blynk.virtualWrite(V1, sens1);
  // Blynk.virtualWrite(V1, tpm.get());
  // Blynk.virtualWrite(V0, weigh.get());
}

void setup()
{
  Serial.begin(115200);
  delay(100);
  BlynkEdgent.begin();
  SensorTimer.setInterval(4000L, send_sens);
  begin_sens(12);

}

void loop() {
  BlynkEdgent.run();
  SensorTimer.run();
}
