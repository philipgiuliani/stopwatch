// pins
int sensorPin = 8;
int checkPointPins[] = { 8, 9, 10, 11, 12 };

int resetPin = 7;
int ledPinReady = 6;
int ledPinStop = 5;

// settings
int threshold = 1000;

// DONT CHANGE!
unsigned long lastRoundTime = 0;
unsigned int currentRound = 0;
boolean countDownRunning = false;
unsigned long currentTime;

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
  
  for (int i = 0; i < sizeof(checkPointPins) / sizeof(int); i = i + 1) {
    if(digitalRead(checkPointPins[i]) == HIGH) {
      Serial.print("Sensor at pin #");
      Serial.print(checkPointPins[i]);
      Serial.println(" detected.");

      sensorCount = sensorCount + 1;
    }
  }

  Serial.print(sensorCount);
  Serial.println(" sensors has been detected.");
}

