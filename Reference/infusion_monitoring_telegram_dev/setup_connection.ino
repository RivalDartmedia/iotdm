void setup_connection(){
  swState = digitalRead(swPin);
  WiFi.mode(WIFI_STA);
  WiFiManager wm;
  // int swState = digitalRead(swPin);
  if (swState == HIGH){
    WiFi.mode(WIFI_OFF);
    wm.resetSettings();
    WiFi.begin(ssid, password);
    Serial.print("Connecting to ");
    Serial.print(ssid);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print("Connecting to ");
      Serial.println(ssid);
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0,0);
      display.println("CONNECTING");
      display.println("NETWORK");
      display.display();
    }
  } else if (swState == LOW){
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,0);
    display.println("CONNECTING");
    display.println("WIFI");
    display.display();
    WiFi.mode(WIFI_OFF);
    bool res;
    res = wm.autoConnect("AutoConnectAP","password");
    if(!res) {
      Serial.println("Failed to connect");
      ESP.restart();
    }
    else {
      //if you get here you have connected to the WiFi    
      Serial.println("connected...)");
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0,0);
      display.println("WIFI");
      display.println("CONNECTED");
      display.display();
      delay(2000);
    }
  }
}