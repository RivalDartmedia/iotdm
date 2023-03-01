void sensor_read(){
  // noInterrupts();
  if (RPM == 0){
    delay(5000);
    if (RPM == 0){
      Serial.println("IINFUSION LOW");
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0,0);
      display.println("INFUSION");
      display.println("LOW");
      display.display();
      
      send_alert_tele();
    }
  }else{
    Serial.println("INFUSION RUNNING");
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,0);
    display.println("INFUSION");
    display.println("RUNNING");
    display.display();
  }
  RPM=0;
  // interrupts();
  delay(1000);  //1 second.
}