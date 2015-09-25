unsigned int sensorPin = 7;
unsigned int resetPin = 8;
unsigned int ledPinReady = 6;
unsigned int ledPinStop = 5;
unsigned int threshold = 1000;

unsigned long lastRoundTime = 0;
unsigned int currentRound = 0;
boolean countDownRunning = false;

void setup() {
  Serial.begin(9600);
  
  pinMode(sensorPin, INPUT);
  pinMode(resetPin, INPUT);
  pinMode(ledPinReady, OUTPUT);
  pinMode(ledPinStop, OUTPUT);
}

void loop() { 
  // start to count
  if(countDownRunning == false) {
    lastRoundTime = millis();
    countDownRunning = true;
    delay(1000);
    
    digitalWrite(ledPinStop, LOW);
    digitalWrite(ledPinReady, HIGH);
  }

  // check for reset
  if(currentRound > 0 && digitalRead(resetPin) == HIGH) {
    reset();
  }

  // check for round
  if(countDownRunning && digitalRead(sensorPin) == HIGH) {
    unsigned long currentTime = millis();

    if(currentRound == 0) {
      lastRoundTime = currentTime;
      currentRound = 1;
      
      digitalWrite(ledPinReady, LOW);
      digitalWrite(ledPinStop, HIGH);
      
      Serial.println("Countdown started");
    }
    else if (currentTime - lastRoundTime >= threshold) {
      Serial.print("Round #");
      Serial.print(currentRound);
      Serial.print(" completed in ");
      Serial.print(currentTime - lastRoundTime);
      Serial.println("ms");
      
      lastRoundTime = currentTime;
      currentRound = currentRound + 1;
    }
  }
}

void reset() {
  currentRound = 0;
  countDownRunning = false;
  Serial.println("Countdown resetted");
}
