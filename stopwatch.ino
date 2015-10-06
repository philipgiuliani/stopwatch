#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3); // tx, rx

// Pins
int sensorPin = 8;
int checkPointPins[] = { 8, 9, 10, 11, 12 };

int resetPin = 7;
int ledPinReady = 6;
int ledPinStop = 5;

// Settings
const int TRESHOLD = 600;
const boolean DEBUGGING = true;
const byte BT_START = '>';
const byte BT_SEPERATOR = ',';
const byte BT_END = '\n';

// DONT CHANGE!
unsigned long startTime;
unsigned long lastRoundTime;
unsigned long currentTime;
boolean countDownRunning = false;

void setup() {
  // set checkpoints as inputs
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
    
    delay(500);
    
    int checkPointsConnected = discoverCheckPoints();
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

    if(startTime == 0) {
      startTime = currentTime;
      lastRoundTime = currentTime;
      
      digitalWrite(ledPinReady, LOW);
      digitalWrite(ledPinStop, HIGH);
      
      sendCheckPointPass(0, 0);
    }
    else if (currentTime - lastRoundTime >= TRESHOLD) {
      sendCheckPointPass(0, startTime - currentTime);
      lastRoundTime = currentTime;
    }
  }
}

// Returns how many checkpoints are connected
int discoverCheckPoints() {
  int checkPointsConnected = 0;
  
  for (int i = 0; i < sizeof(checkPointPins) / sizeof(int); i++) {
    if(digitalRead(checkPointPins[i]) == HIGH) {
      checkPointsConnected += 1;
    }
  }

  sendCheckPointDiscover(checkPointsConnected);

  return checkPointsConnected;
}

// Reset, and discover checkpoints
void reset() {
  currentRound = 0;
  countDownRunning = false;
  Serial.println("Countdown resetted");
}

//
// BLUETOOTH
//

// When checkpoints have been discovered.
// Example: >discover,4\n
void sendCheckPointDiscover(int checkPointCount) {
  mySerial.write(BT_START);
  mySerial.write("discover");
  mySerial.write(BT_SEPERATOR);
  mySerial.write(checkPointCount);
  mySerial.write(BT_END);

  if(DEBUGGING) {
    Serial.print("Found ");
    Serial.print(checkPointCount);
    Serial.println("checkpoints");
  }
}

// When a checkpoint has been passed.
// Example: >checkpoint,0,5000\n
void sendCheckPointPass(int checkPointId, unsigned int passedTime) {
  mySerial.write(BT_START);
  mySerial.write("checkpoint");
  mySerial.write(BT_SEPERATOR);
  mySerial.write(checkPointId);
  mySerial.write(BT_SEPERATOR);
  mySerial.write(passedTime);
  mySerial.write(BT_END);

  if(DEBUGGING) {
    Serial.print("Checkpoint #");
    Serial.print(checkPointId);
    Serial.print(" completed in ");
    Serial.print(passedTime);
    Serial.println("ms");
  }
}

