#include "Colors.h"
#include "ColorPalettes.h"

#define NUM_PUMPS  7

bool ENABLE_COLOR_PATTERN = false;
bool ENABLE_AXIDRAW = false;

ColorPattern colorPattern = RAINBOW_CYCLE;

// ROYGBV, WATER
const int PUMP_PINS[NUM_PUMPS]         = {12, 11, 10, 9, 8, 7, 6};
const int PRIME_BUTTON_PINS[NUM_PUMPS] = {A0, A1, A2, A3, 4, 5, A6}; // A4 and A5 are hardwired for I2C, can't override
const int AXIDRAW_PIN = 2;

// motor state
bool motorState[NUM_PUMPS]             = {false, false, false, false, false, false, false};
bool isDrawing = false;

unsigned long previousMillis = 0;
const unsigned long motorOnTime = 20;  
const unsigned long motorOffTime = 2000; 
// const unsigned long motorOnTime = 50;  
// const unsigned long motorOffTime = 4000; 

// prime button state
bool primeState[NUM_PUMPS]             = {false, false, false, false, false, false, false};
bool lastPrimeButtonState[NUM_PUMPS]   = {false, false, false, false, false, false, false};
bool primeButtonState[NUM_PUMPS]       = {false, false, false, false, false, false, false};

// axidraw pen state
int penDownState = LOW;
int lastPenDownState = LOW;

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
  }

  pinMode(AXIDRAW_PIN, INPUT);

  if (ENABLE_COLOR_PATTERN) {
    colorPattern.startPattern();
  }
}

void loop() {
  // digitalWrite(PUMP_PINS[0], HIGH);
  // delay(2000);
  // digitalWrite(PUMP_PINS[0], LOW);
  // digitalWrite(PUMP_PINS[1], HIGH);
  // delay(2000);
  // digitalWrite(PUMP_PINS[1], LOW);
  // digitalWrite(PUMP_PINS[3], HIGH);
  // delay(2000);
  // digitalWrite(PUMP_PINS[3], LOW);
  // digitalWrite(PUMP_PINS[5], HIGH);
  // delay(2000);
  // digitalWrite(PUMP_PINS[5], LOW);
  // pumpOn(0);
  // delay(2000);
  // pumpOn(1);
  // delay(2000);
  // pumpOn(3);
  // delay(2000);
  // pumpOn(5);
  // delay(2000);
  for(int i = 0; i < NUM_PUMPS; i++) {
    primeButtonState[i] = digitalRead(PRIME_BUTTON_PINS[i]);
  }
  penDownState = digitalRead(AXIDRAW_PIN);

  // if drawing sketch, obey pen up/down state
  if (penDownState != lastPenDownState && ENABLE_AXIDRAW) {
    if (penDownState == HIGH) {
      motorState[activeColor] = true;
    } else {
      motorState[activeColor] = false;
    }
    lastPenDownState = penDownState;

    if (ENABLE_COLOR_PATTERN) {
      // can update `activeColor`
      colorPattern.updateColorPattern();
    }
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