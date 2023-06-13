const int buttonPin = 19;  // Pin tombol
const int buzzerPin = 5;  // Pin buzzer
int buttonState = 0;      // Variabel untuk menyimpan status tombol
bool isButtonPressed = false;  // Flag untuk menandai jika tombol ditekan

void setup() {
  pinMode(buttonPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
}

void loop() {
  buttonState = digitalRead(buttonPin);  // Membaca status tombol

  if (buttonState == HIGH && !isButtonPressed) {  // Jika tombol ditekan
    isButtonPressed = true;  // Set flag menjadi true
    digitalWrite(buzzerPin, HIGH);  // Aktifkan buzzer
    delay(2000);  // Tunggu selama 2 detik
    digitalWrite(buzzerPin, LOW);  // Matikan buzzer
  }
  
  if (buttonState == LOW) {  // Jika tombol dilepas
    isButtonPressed = false;  // Set flag menjadi false
  }
}
