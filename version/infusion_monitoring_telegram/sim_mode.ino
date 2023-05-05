void sim_mode(){
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
  // WriteTS();
  // ESP.wdtDisable();
  bot.sendMessage(CHAT_ID, "(SIM MODE) ALERT!, OKSIGEN HABIS", "");
  Serial.println("Alert terkirim via SIM");
  // delay(2000);
  
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

// void WriteTS(){
// //  String address="http://thick-lemons-fail-180-254-76-126.loca.lt/insertLog?cardID=DB1FD74F";
//   String address="http://date.jsontest.com/";
//   char URL1[100];
 
//   // Do HTTP GET communication with 10s for the timeout (read)
//   address.toCharArray(URL1, 100) ;
  
//   uint16_t rc = sim800l->doGet(URL1, 10000);
//   if(rc == 200) {
//     // Success, output the data received on the serial
//     Serial.print(F("HTTP GET successful ("));
//     Serial.print(sim800l->getDataSizeReceived());
//     Serial.println(F(" bytes)"));
//     Serial.print(F("Received : "));
//     String TS_data=sim800l->getDataReceived();
//     Serial.println(TS_data);

//     StaticJsonDocument<1024> jsonBuffer;
//     deserializeJson(jsonBuffer, TS_data);
//     const char* nama = jsonBuffer["date"];
//     Serial.println(nama);
//     display.clearDisplay();
//     display.setTextSize(2);
//     display.setTextColor(SSD1306_WHITE);
//     display.println(nama);
//     display.setCursor(0,0);
//     display.display();
    
//   } else {
//     // Failed...
//     Serial.print(F("HTTP GET error "));
//     Serial.println(rc);
//   }
// }