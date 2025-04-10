#include "Colors.h"
#include "ColorPalettes.h"

#define NUM_PUMPS  7

bool ENABLE_COLOR_PATTERN = false;
bool ENABLE_AXIDRAW = false;

ColorPattern colorPattern = WARM_COLOR;

// ROYGBV, WATER
const int PUMP_PINS[NUM_PUMPS]         = {11, 12, 9, 10, 7, 8, 6};
const int PRIME_BUTTON_PINS[NUM_PUMPS] = {A0, A1, A2, A3, A6, A7, 3}; // A4 and A5 are hardwired for I2C, can't override
const int AXIDRAW_PIN = 2;

// motor state
bool motorState[NUM_PUMPS]             = {false, false, false, false, false, false, false};
bool isDrawing = false;

unsigned long previousMillis = 0;
const unsigned long motorOnTime = 30;  
const unsigned long motorOffTime = 5000; 
// const unsigned long motorOnTime = 50;  
// const unsigned long motorOffTime = 4000; 

// prime button state
bool primeState[NUM_PUMPS]             = {false, false, false, false, false, false, false};
bool lastPrimeButtonState[NUM_PUMPS]   = {false, false, false, false, false, false, false};
bool primeButtonState[NUM_PUMPS]       = {false, false, false, false, false, false, false};

// axidraw pen state
int penDownState = LOW;
int lastPenDownState = LOW;
bool penJustLowered = false;
bool penLoweredFirstTime = false;

/**
  IMPORTANT
  The color/motor number that is active, i.e. can dispense paint
*/
int activeColor = RED;

void setup() {
  Serial.begin(9600);

  for(int i=0; i < NUM_PUMPS; i++) {
    pinMode(PUMP_PINS[i], OUTPUT);
    pinMode(PRIME_BUTTON_PINS[i], INPUT);
    digitalWrite(PUMP_PINS[i], LOW);
  }

  pinMode(AXIDRAW_PIN, INPUT);

  if (ENABLE_COLOR_PATTERN) {
    colorPattern.startPattern();
  }
}

void loop() {
  readSerial();
  penDownState = digitalRead(AXIDRAW_PIN);

  for(int i = 0; i < NUM_PUMPS; i++) {
    primeButtonState[i] = digitalRead(PRIME_BUTTON_PINS[i]);
  }

  // if drawing sketch, obey pen up/down state
  if ((penDownState != lastPenDownState && ENABLE_AXIDRAW)) {
    if (penDownState == HIGH) {
      motorState[activeColor] = true;
    } else {
      motorState[activeColor] = false;
    }
    lastPenDownState = penDownState;
  } 
  // check all prime pump buttons, only one HIGH at a time
  else {
    for(int i = 0; i < NUM_PUMPS; i++) {
      if (primeButtonState[i] == LOW && lastPrimeButtonState[i] == HIGH) {
        primeState[i] = !primeState[i];

        // if one just enabled, disable others
        if (primeState[i]) {
          Serial.print("Priming pump ");
          Serial.println(i);
          activeColor = i;
          for (int j = 0; j < NUM_PUMPS; j++) {
            if (j != i) {
              primeState[j] = false;
            }
          }
        } else {
          Serial.print("Stopping pump ");
          Serial.println(i);
        }
        previousMillis = millis();
      }
    }
  }

  for(int i = 0; i < NUM_PUMPS; i++) {
    lastPrimeButtonState[i] = primeButtonState[i];
  }

  if (any(motorState) || any(primeState)) {
    unsigned long currentMillis = millis();

    if (any(primeState)) {
      isDrawing = true;
      previousMillis = currentMillis;
    } else if (isDrawing && currentMillis - previousMillis >= motorOnTime) {
      Serial.println("MOTOR OFF");
      isDrawing = false;
      previousMillis = currentMillis;
    } else if (!isDrawing && currentMillis - previousMillis >= motorOffTime) {
      Serial.println("MOTOR ON");
      isDrawing = true;
      previousMillis = currentMillis;
    }
  } else {
    isDrawing = false;
  }
 
  // if (ENABLE_COLOR_PATTERN && !any(primeState)) {
  //   // can update `activeColor`
  //   colorPattern.updateColorPattern();
  // }

  if (isDrawing) {
    pumpOn(activeColor);
  } else {
    pumpOff(activeColor);
  }

  delay(1);
}

/**
 Enforce only one pump on at a time
 One pin is always low
*/
void pumpOn(int i) {
  for(int j=0; j < NUM_PUMPS; j++) {
    if (i != j) {
      pumpOff(j);
    }
  }
  digitalWrite(PUMP_PINS[i], HIGH);
}

/**
 Pump off, one pin is always low
*/
void pumpOff(int i) {
  digitalWrite(PUMP_PINS[i], LOW);
}


bool any(bool arr[NUM_PUMPS]) {
  for (int i = 0; i < NUM_PUMPS; i++) {
    if (arr[i]) return true;
  }
  return false;
}

void readSerial() {
  if (Serial.available() > 0) {
    String message = Serial.readStringUntil('\n');

    if (message.startsWith("COLOR:")) {
      String indexStr = message.substring(6); 
      int index = indexStr.toInt();
      
      if (index >= 0 && index < NUM_PUMPS) {
        activeColor = index;
      }
    } 
    else if(message.startsWith("DRAW:")) {
      String stateStr = message.substring(5);
      int state = stateStr.toInt();
      if (state == 0) {
        ENABLE_AXIDRAW = false;
      } else if (state == 1) {
        ENABLE_AXIDRAW = true;
      }
    } 
    else if (message.startsWith("PRIME:")) {
      String indexStr = message.substring(6);
      int index = indexStr.toInt();
      if (index >= 0 && index < NUM_PUMPS) {
        primeButtonState[index] = LOW;
        lastPrimeButtonState[index] = HIGH;
        // primeState[index] = !primeState[index];
        // activeColor = index;
      }
    }
    else {
      Serial.println("Invalid message format");
    }
  }
}
