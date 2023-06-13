#ifndef buzzer_h
#define buzzer_h

class buzzer
{
  private:
    int buzzPin;
    int buzzState = LOW;
    unsigned long previousMillis = 0;
    unsigned long buzzerStartTime = 0;
  
  public:
    void init(int buzzPin)
    {
      this->buzzPin = buzzPin;
      pinMode(buzzPin, OUTPUT);
    }

    void buzzbeep(){
      digitalWrite(buzzPin, HIGH);
      delay(2000);
    }

    void buzzbeep1(unsigned long buzzerDuration)
    {
      unsigned long currentMillis = millis();
      if (currentMillis - buzzerStartTime < buzzerDuration) {
        digitalWrite(buzzPin, HIGH); // Mengaktifkan buzzer
      } else {
        digitalWrite(buzzPin, LOW); // Mematikan buzzer
      }

      if (currentMillis - buzzerStartTime >= buzzerDuration) {
        buzzerStartTime = currentMillis;
      }
    }

    void buzzbeepc()
    {
      int limit_try = 15, cnt = 0;
      while (limit_try > cnt){
        digitalWrite(buzzPin, HIGH);
        Serial.println(limit_try);
        limit_try--;
        delay(1000);
      }
      digitalWrite(buzzPin, LOW);
    }

    void buzzonoff(const long interval)
    {
      unsigned long currentMillis = millis();
      if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        if (buzzState == LOW) {
          buzzState = HIGH;
        } else {
          buzzState = LOW;
        }
      digitalWrite(buzzPin, buzzState);
      }
    }
};

#endif