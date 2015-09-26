// pins
unsigned int sensorPin = 8;
unsigned int checkpointPins[] = { 8, 9, 10, 11, 12 };

unsigned int resetPin = 7;
unsigned int ledPinReady = 6;
unsigned int ledPinStop = 5;

// settings
unsigned int threshold = 1000;

// DONT CHANGE!
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
  // read bluetooth input
  if(Serial.available() > 0) {
     String input = Serial.readStringUntil('\n');
     if(input == "reset") {
       reset();
     }
  }
  
  // start to count
  if(countDownRunning == false) {
    digitalWrite(ledPinStop, HIGH);
    digitalWrite(ledPinReady, LOW);
    
    readCheckPoints();
    countDownRunning = true;

    delay(1000);
    
    digitalWrite(ledPinStop, LOW);
    digitalWrite(ledPinReady, HIGH);
  }

  // check for reset
  if(digitalRead(resetPin) == HIGH) {
    reset();
  }

  // check for round
  if(countDownRunning && digitalRead(sensorPin) == LOW) {
    unsigned long currentTime = millis();

    if(currentRound == 0) {
      lastRoundTime = currentTime;
      currentRound = 1;
      
      digitalWrite(ledPinReady, LOW);
      digitalWrite(ledPinStop, HIGH);
      
      Serial.println("Round #1 started");
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

void readCheckPoints() {
  int sensorCount = 0;
  
  for (int i = 0; i < sizeof(checkpointPins) / sizeof(int); i = i + 1) {
    if(digitalRead(checkpointPins[i]) == HIGH) {
      Serial.print("Sensor at pin #");
      Serial.print(checkpointPins[i]);
      Serial.println(" detected.");

      sensorCount = sensorCount + 1;
    }
  }

  Serial.print(sensorCount);
  Serial.println(" sensors has been detected.");
}

