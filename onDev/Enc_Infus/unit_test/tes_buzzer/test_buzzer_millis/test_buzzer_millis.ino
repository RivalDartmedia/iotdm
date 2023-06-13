// Pin buzzer terhubung ke pin digital 9
int buzzerPin = 5;

// Waktu awal saat buzzer diaktifkan
unsigned long startMillis;

// Durasi aktif buzzer dalam milidetik
unsigned long buzzerDuration = 500;

// Status buzzer (0 = mati, 1 = aktif)
int buzzerStatus = 0;

void setup() {
  // Atur pin buzzer sebagai OUTPUT
  pinMode(buzzerPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // Periksa apakah buzzer dalam status mati
  if (buzzerStatus == 0) {
    // Aktifkan buzzer
    digitalWrite(buzzerPin, HIGH);
    Serial.println("Buzzer ON");
    
    // Simpan waktu saat ini
    startMillis = millis();
    
    // Ubah status buzzer menjadi aktif
    buzzerStatus = 1;
  }
  
  // Periksa apakah buzzer sudah aktif selama satu detik
  if (buzzerStatus == 1 && millis() - startMillis >= buzzerDuration) {
    // Matikan buzzer
    digitalWrite(buzzerPin, LOW);
    Serial.println("Buzzer OFF");
    
    // Ubah status buzzer menjadi mati
    // buzzerStatus = 0;
  }
}
