// pins
int sensorPin = 8;
int checkPointPins[] = { 8, 9, 10, 11, 12 };
int checkPointsConnected = 0;

int resetPin = 7;
int ledPinReady = 6;
int ledPinStop = 5;

// settings
int threshold = 1000;

// DONT CHANGE!
unsigned long lastRoundTime;
unsigned int currentRound;
unsigned long currentTime;
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
    
    delay(500);

    if(checkPointsConnected > 0) {
      countDownRunning = true;
      digitalWrite(ledPinStop, LOW);
      digitalWrite(ledPinReady, HIGH);
    }
    else {
      digitalWrite(ledPinStop, HIGH);
      digitalWrite(ledPinReady, LOW);
    }
  }

  // check for reset
  if(digitalRead(resetPin) == HIGH) {
    reset();
  }

  // check for round
  if(countDownRunning && digitalRead(sensorPin) == LOW) {
    currentTime = millis();

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
      currentRound += 1;
    }
  }
}

void reset() {
  currentRound = 0;
  countDownRunning = false;
  Serial.println("Countdown resetted");
}

void readCheckPoints() {
  checkPointsConnected = 0;
  
  for (int i = 0; i < sizeof(checkPointPins) / sizeof(int); i++) {
    if(digitalRead(checkPointPins[i]) == HIGH) {
      checkPointsConnected += 1;
    }
  }

  if(checkPointsConnected > 0) {
    Serial.print(checkPointsConnected);
    Serial.println(" checkpoint(s) detected");
  }
  else {
    Serial.println("No checkpoints detected..");
  }
}

