#ifndef koneksi_sim_h
#define koneksi_sim_h

// #include "mem_set.h"
// #include "indikator.h"
// #include "koneksi_cred.h"

#include <Arduino.h>
#include <SoftwareSerial.h>
#include "SIM800L.h"
#include <ArduinoJson.h>
#include "mem_set.h"
#include "display_led.h"

#define SIM800_TX_PIN 16
#define SIM800_RX_PIN 17
#define SIM800_RST_PIN 23

DisplayLed displed_sim;

class ConnectionSIM
{
private:
  // Update server+token+send_p+berat_v+   +tpm_v
  // get Blink Indicator server + get_p + blink_v

  const char *APN = "internet";
//  const char *APN = "3gprs";
  SIM800L *sim800l;
  
public : 

  void init()
  {
    SoftwareSerial *serial = new SoftwareSerial(SIM800_TX_PIN, SIM800_RX_PIN);
    serial->begin(9600);
    delay(1000);

    sim800l = new SIM800L((Stream *)serial, SIM800_RST_PIN, 200, 512);
    // sim800l = new SIM800L((Stream *)serial, SIM800_RST_PIN, 200, 512, (Stream *)&Serial);
    Serial.println("Waiting for module settings...");

    // Setup module for GPRS communication
    int PowerMode = sim800l->getPowerMode();
    if (PowerMode == 1)
    {
      Serial.println(F("Module in normal power mode"));
      Serial.println(F("Ready for next setup..."));
    }
    else
    {
      Serial.println(F("Switch to normal power mode ..."));
      sim800l->setPowerMode(NORMAL);
      delay(200);
    }
    setupModule();
  }

  bool setupModule()
  {
    // Wait until the module is ready to accept AT commands
    while (!sim800l->isReady())
    {
      Serial.println(F("Problem to initialize AT command, retry in 1 sec"));
      delay(1000);
    }
    Serial.println(F("Setup Complete!"));

    // Wait for operator network registration (national or roaming network)
    displed_sim.print("Mendaftar jaringan...", 0, 0);
    int cnt_sim_lim = 10, cnt_sim = 0;
    NetworkRegistration network = sim800l->getRegistrationStatus();
    while (network != REGISTERED_HOME && network != REGISTERED_ROAMING && cnt_sim < cnt_sim_lim)
    {
      delay(1000);
      network = sim800l->getRegistrationStatus();
      cnt_sim++;
      Serial.println(cnt_sim);
    }

    if (cnt_sim < 10){
      Serial.println(F("Network registration OK"));
      displed_sim.print("SIMCARD OK", 0, 0);
      return true;
    }else{
      Serial.println("SIM network error");
      displed_sim.print("No SIM /  No Signal", 0, 0);
      return false;
    }
    delay(1000);

    // Setup APN for GPRS configuration
    bool success = sim800l->setupGPRS(APN);
    while (!success)
    {
      success = sim800l->setupGPRS(APN);
      delay(5000);
    }
    Serial.println(F("GPRS config OK"));
  }

  int connect_gprs(InfusConfig &infusconfig, int tpm, int weigh)
  {
    Serial.println("Mulai...");
    // Establish GPRS connectivity (5 trials)
    bool connected = false;
    for (uint8_t i = 0; i < 5 && !connected; i++)
    {
      delay(1000);
      connected = sim800l->connectGPRS();
    }

    // Check if connected, if not reset the module and setup the config again
    if (connected)
    {
      Serial.println(F("GPRS connected !"));
    }
    else
    {
      Serial.println(F("GPRS not connected !"));
      Serial.println(F("Reset the module."));
      sim800l->reset();
      setupModule();
      // return;
    }

    Serial.println(F("Start HTTP GET..."));

    // Write TS Channel
    // WriteTS();
    String tokenid = infusconfig.get(tokenID_p);
    String infusid = infusconfig.get(infus_name_p);
    // String address="http://date.jsontest.com/";
    // String address=server_dom + send_p + token + tokenid + berat_v + String(weigh) + tpm_v + String(tpm);
    String address = server_dom + send_p + get_p + token + tokenid + "&text=" + "ID+Device+=+" + String(infusid) + ";+TPM+=+" + String(tpm) + "+;+Weigh+=+" + String(weigh);
    Serial.println(address);
    // int number = random(0, 100);
    // String numberstr;
    // numberstr = String(number);
    // Serial.println(numberstr);
    // String address = "http://sgp1.blynk.cloud/external/api/update?token=ZIjaYVCHA9Vota0HFas5xh49JGXrM3Zy&V4=" + numberstr;
    char URL1[135];

    // Do HTTP GET communication with 10s for the timeout (read)
    address.toCharArray(URL1, 135);

    uint16_t rc = sim800l->doGet(URL1, 10000);
    if (rc == 200)
    {
      // Success, output the data received on the serial
      Serial.print(F("HTTP GET successful ("));
      Serial.print(sim800l->getDataSizeReceived());
      Serial.println(F(" bytes)"));
      Serial.print(F("Received : "));
      String TS_data = sim800l->getDataReceived();
      Serial.println(TS_data);
      displed_sim.print("Kirim databerhasil", 0, 0);
    }
    else
    {
      // Failed...
      Serial.print(F("HTTP GET error "));
      Serial.println(rc);
      // displed_sim.print("Kirim datagagal", 0, 0);
    }

    // Close GPRS connectivity (5 trials)
    bool disconnected = sim800l->disconnectGPRS();
    for (uint8_t i = 0; i < 5 && !connected; i++)
    {
      delay(1000);
      disconnected = sim800l->disconnectGPRS();
    }

    if (disconnected)
    {
      Serial.println(F("GPRS disconnected !"));
    }
    else
    {
      Serial.println(F("GPRS still connected !"));
    }
    sim800l->reset();
    return rc;
    delay(1000);
  }

  // void WriteTS(InfusConfig &infusconfig, double tpm, int weigh)
  // {
  //   String tokenid = infusconfig.get(tokenID_p);
  //   // String address="http://date.jsontest.com/";
  //   String address=server_dom + send_p + token + tokenid + berat_v + String(weigh) + tpm_v + String(tpm);
  //   Serial.println(address);
  //   // int number = random(0, 100);
  //   // String numberstr;
  //   // numberstr = String(number);
  //   // Serial.println(numberstr);
  //   // String address = "http://sgp1.blynk.cloud/external/api/update?token=ZIjaYVCHA9Vota0HFas5xh49JGXrM3Zy&V4=" + numberstr;
  //   char URL1[100];

  //   // Do HTTP GET communication with 10s for the timeout (read)
  //   address.toCharArray(URL1, 100);

  //   uint16_t rc = sim800l->doGet(URL1, 10000);
  //   if (rc == 200)
  //   {
  //     // Success, output the data received on the serial
  //     Serial.print(F("HTTP GET successful ("));
  //     Serial.print(sim800l->getDataSizeReceived());
  //     Serial.println(F(" bytes)"));
  //     Serial.print(F("Received : "));
  //     String TS_data = sim800l->getDataReceived();
  //     Serial.println(TS_data);
  //     // disp_sim.print("Send data succed");
  //     displed_sim.print("Kirim databerhasil");
  //   }
  //   else
  //   {
  //     // Failed...
  //     Serial.print(F("HTTP GET error "));
  //     Serial.println(rc);
  //     // disp_sim.print("Send data failed");
  //     displed_sim.print("Kirim datagagal");
  //   }
  // }
};

#endif // !1
