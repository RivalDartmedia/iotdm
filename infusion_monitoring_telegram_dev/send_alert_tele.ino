void send_alert_tele(){
  bot.sendMessage(CHAT_ID, "ALERT!, INFUS HABIS/MAMPET", "");
  
  Serial.println("Alert terkirim");
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("ALERT SENT");
  display.display();
  delay(1000);
}