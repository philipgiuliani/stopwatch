#include <SoftwareSerial.h>

// bluetooth initialization
SoftwareSerial mySerial(2, 3); // tx, rx

// pins
int sensorPin = 8;
int checkPointPins[] = { 8, 9, 10, 11, 12 };
int checkPointsConnected = 0;

int resetPin = 7;
int ledPinReady = 6;
int ledPinStop = 5;

// settings
int threshold = 600;

// DONT CHANGE!
unsigned long lastRoundTime;
unsigned int currentRound;
unsigned long currentTime;
boolean countDownRunning = false;

void setup() {
  // set pins for the pumps
  for(int i = 0; i < sizeof(checkPointPins) / sizeof(int); i++) {
    pinMode(checkPointPins[i], INPUT);
  }
  
  pinMode(resetPin, INPUT);
  pinMode(ledPinReady, OUTPUT);
  pinMode(ledPinStop, OUTPUT);

  // start serial
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // bluetooth
  Serial.println("Starting config");
  mySerial.begin(57600);
  delay(1000);

  // Should respond with OK
  mySerial.print("AT");
  waitForResponse();

  // Should respond with its version
  mySerial.print("AT+VERSION");
  waitForResponse();

  // Set pin to 0000
  mySerial.print("AT+PIN0000");
  waitForResponse();

  // Set the name to ROBOT_NAME
  mySerial.print("AT+NAME");
  mySerial.print("Stoppuhr");
  waitForResponse();

  // Set baudrate to 57600
  mySerial.print("AT+BAUD7");
  waitForResponse();

  Serial.println("Done!");
}

void waitForResponse() {
    delay(1000);
    while (mySerial.available()) {
      Serial.write(mySerial.read());
    }
    Serial.write("\n");
}

void loop() {
  // read bluetooth input
  if(mySerial.available() > 0) {
    char input = mySerial.read();
    // String input = mySerial.readStringUntil('\n');
    // if(input == "reset") {
    if(input == 'r') {
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
  if(countDownRunning && digitalRead(sensorPin) == HIGH) {
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

      mySerial.print(currentTime - lastRoundTime);
      
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
    if(digitalRead(checkPointPins[i]) == LOW) {
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

