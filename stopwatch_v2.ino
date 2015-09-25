unsigned int sensorPin = 7;
unsigned int resetPin = 8;
unsigned int ledPin = 13;
unsigned int threshold = 1000;

unsigned long lastRoundTime = 0;
unsigned int currentRound = 0;
boolean countDownRunning = false;

void setup() {
  Serial.begin(9600);
  pinMode(sensorPin, INPUT);
  pinMode(ledPin, OUTPUT);
}

void loop() { 
  // start to count
  if(countDownRunning == false) {
    lastRoundTime = millis();
    countDownRunning = true;
    delay(1000);
  }

  // check for reset
  if(currentRound == 0 && digitalRead(resetPin) == HIGH) {
    reset();
  }

  // check for round
  if(countDownRunning && digitalRead(sensorPin) == HIGH) {
    unsigned long currentTime = millis();

    if(currentRound == 0) {
      lastRoundTime = currentTime;
      currentRound = 1;
      
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
