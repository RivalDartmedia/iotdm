#include <SoftwareSerial.h>
#include "SIM800L.h"
#include <ArduinoJson.h>
 
// #define SIM800_TX_PIN 13  //Rx ESP8266 D7
// #define SIM800_RX_PIN 15  //Tx ESP8266 D8
// #define SIM800_RST_PIN 12 //D6

#define SIM800_TX_PIN 16  //Rx ESP8266 D7
#define SIM800_RX_PIN 17  //Tx ESP8266 D8
#define SIM800_RST_PIN 22 //D22

const char APN[] = "internet"; // ==> your APN

SIM800L* sim800l;

void setup() {
  Serial.begin(9600);
  while(!Serial);

  // Initialize a SoftwareSerial
  SoftwareSerial* serial = new SoftwareSerial(SIM800_TX_PIN, SIM800_RX_PIN);
  serial->begin(9600); 
  delay(1000);
  
  sim800l = new SIM800L((Stream *)serial, SIM800_RST_PIN, 200, 512);
  // sim800l = new SIM800L((Stream *)serial, SIM800_RST_PIN, 200, 512, (Stream *)&Serial);
  Serial.println("Waiting for module settings...");
  
  // Setup module for GPRS communication
   int PowerMode= sim800l->getPowerMode(); 
    if(PowerMode==1) {
      Serial.println(F("Module in normal power mode"));
      Serial.println(F("Ready for next setup..."));
    } else {
      Serial.println(F("Switch to normal power mode ..."));
      sim800l->setPowerMode(NORMAL);
      delay(200);
    }
  setupModule();
}
 
void loop() {
   
  // Establish GPRS connectivity (5 trials)
  bool connected = false;
  for(uint8_t i = 0; i < 5 && !connected; i++) {
    delay(1000);
    connected = sim800l->connectGPRS();
  }

  // Check if connected, if not reset the module and setup the config again
  if(connected) {
    Serial.println(F("GPRS connected !"));
  } else {
    Serial.println(F("GPRS not connected !"));
    Serial.println(F("Reset the module."));
    sim800l->reset();
    setupModule();
    return;
  }
  
  Serial.println(F("Start HTTP GET..."));

  //Write TS Channel
  WriteTS();
  
  // Close GPRS connectivity (5 trials)
  bool disconnected = sim800l->disconnectGPRS();
  for(uint8_t i = 0; i < 5 && !connected; i++) {
    delay(1000);
    disconnected = sim800l->disconnectGPRS();
  }
  
  if(disconnected) {
    Serial.println(F("GPRS disconnected !"));
  } else {
    Serial.println(F("GPRS still connected !"));
  }
  
  delay(3000);
}

void WriteTS(){
//  String address="http://thick-lemons-fail-180-254-76-126.loca.lt/insertLog?cardID=DB1FD74F";
  // String address="http://date.jsontest.com/";
  // String address="http://api.telegram.org/bot6025777872:AAGOiaT-UVZCXnW7Ur9KS2xmuLIXgirmRE4/sendMessage?chat_id=-894765173&text=tes";
  String address="http://sgp1.blynk.cloud/external/api/update?token=ZIjaYVCHA9Vota0HFas5xh49JGXrM3Zy&V4=90";
  char URL1[100];
 
  // Do HTTP GET communication with 10s for the timeout (read)
  address.toCharArray(URL1, 100) ;
  
  uint16_t rc = sim800l->doGet(URL1, 10000);
  if(rc == 200) {
    // Success, output the data received on the serial
    Serial.print(F("HTTP GET successful ("));
    Serial.print(sim800l->getDataSizeReceived());
    Serial.println(F(" bytes)"));
    Serial.print(F("Received : "));
    String TS_data=sim800l->getDataReceived();
    Serial.println(TS_data);

    // StaticJsonDocument<1024> jsonBuffer;
    // deserializeJson(jsonBuffer, TS_data);
    // const char* nama = jsonBuffer["date"];`
    // Serial.println(nama);
    
  } else {
    // Failed...
    Serial.print(F("HTTP GET error "));
    Serial.println(rc);
  }
}

void setupModule() {
    // Wait until the module is ready to accept AT commands
  while(!sim800l->isReady()) {
    Serial.println(F("Problem to initialize AT command, retry in 1 sec"));
    delay(1000);
  }
  Serial.println(F("Setup Complete!"));
 
  // Wait for operator network registration (national or roaming network)
  NetworkRegistration network = sim800l->getRegistrationStatus();
  while(network != REGISTERED_HOME && network != REGISTERED_ROAMING) {
    delay(1000);
    network = sim800l->getRegistrationStatus();
  }
  Serial.println(F("Network registration OK"));
  delay(1000);

  // Setup APN for GPRS configuration
  bool success = sim800l->setupGPRS(APN);
  while(!success) {
    success = sim800l->setupGPRS(APN);
    delay(5000);
  }
  Serial.println(F("GPRS config OK"));
}
