#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3); // tx, rx

// Pins
short sensorPin = 8;
short checkPointPins[] = { 8, 9, 10, 11, 12 };
boolean *checkPointStatuses;

short resetPin = 7;
short ledPinReady = 6;
short ledPinStop = 5;

// Settings
const int TRESHOLD = 600;
const boolean DEBUGGING = true;
const byte BT_START = '>';
const byte BT_SEPERATOR = ',';
const byte BT_END = '\n';

// DONT CHANGE!
boolean countDownRunning = false;
unsigned long startTime;
unsigned long lastRoundTime;
unsigned long currentTime;

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

  // check for reset
  if(digitalRead(resetPin) == HIGH) {
    reset();
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
  // check for round
  else {
    for(int i = 0; i < 5; i++) {
      if(checkPointStatuses[i] == 0) continue; // skip if the checkpoint is not connected
      
      if(digitalRead(checkPointPins[i]) == LOW) {
        currentTime = millis();

        if(startTime == 0) {
          startTime = currentTime;
          lastRoundTime = currentTime;
          
          digitalWrite(ledPinReady, LOW);
          digitalWrite(ledPinStop, HIGH);
          
          sendCheckPointPass(i, 0);
        }
        else if (currentTime - lastRoundTime >= TRESHOLD) {
          sendCheckPointPass(i, currentTime - startTime);
          lastRoundTime = currentTime;
        }
      }
    }
  }

  /*
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
  */
}

// Returns how many checkpoints are connected
int discoverCheckPoints() {
  boolean checkPointsStatusTemp[sizeof(checkPointPins) / sizeof(int)];
  
  int checkPointsConnected = 0;
  for (int i = 0; i < sizeof(checkPointPins) / sizeof(int); i++) {
    if(digitalRead(checkPointPins[i]) == HIGH) {
      checkPointsStatusTemp[i] = 1;
      checkPointsConnected += 1;
    }
    else {
      checkPointsStatusTemp[i] = 0;
    }
  }
  checkPointStatuses = checkPointsStatusTemp;
  sendCheckPointDiscover(checkPointsConnected);
  
  return checkPointsConnected;
}

// Reset, and discover checkpoints
void reset() {
  startTime = 0;
  lastRoundTime = 0;
  currentTime = 0;
  countDownRunning = false;

  if(DEBUGGING) {
    Serial.println("Countdown resetted");
  }
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
    Serial.println(" checkpoints");
  }
}

// When a checkpoint has been passed.
// Example: >checkpoint,0,5000\n
void sendCheckPointPass(int checkPointId, unsigned long passedTime) {
  /*
  mySerial.write(BT_START);
  mySerial.write("checkpoint");
  mySerial.write(BT_SEPERATOR);
  mySerial.write(checkPointId);
  mySerial.write(BT_SEPERATOR);
  mySerial.write(passedTime);
  mySerial.write(BT_END);
  */

  if(DEBUGGING) {
    Serial.print("Checkpoint #");
    Serial.print(checkPointId);
    Serial.print(" passed at ");
    Serial.print(passedTime);
    Serial.println("ms");
  }
}

