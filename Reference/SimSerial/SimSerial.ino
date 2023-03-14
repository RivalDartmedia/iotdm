 #define RXD2 16
 #define TXD2 17

 void setup() {
   Serial.begin(115200);
   Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
   delay(1000);
   Serial.println("Loopback program started");
 }
 void loop() {
   if(Serial.available()){
    //  Serial.write("-");
     Serial2.write(Serial.read());  
   }
   if(Serial2.available()){
    //  Serial.write(".");
     Serial.write(Serial2.read());  
   }
 }

 //AT Command Simple Set up

 /*
 AT : Reply OK, berhasil
 AT+IPR=115200 : Set Baud rate
 ATI : Informasi device
 AT+COPS=? : Bisa ke jaringan apa aja
 AT+COPS? : Sekarang konek kemana
 AT+CSQ : Kualitas Sinyal
 AT+SAPBR=3,1,"CONTYPE","GPRS"
 */