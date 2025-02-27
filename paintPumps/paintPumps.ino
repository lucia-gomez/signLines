#include <Adafruit_PWMServoDriver.h>
#include <Wire.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

const int NUM_PUMPS = 7;

const int PUMP_PIN_RED = 12;
const int PUMP_PIN_ORANGE = 11;
const int PUMP_PIN_YELLOW = 10;
const int PUMP_PIN_GREEN = 9;
const int PUMP_PIN_BLUE = 8;
const int PUMP_PIN_PURPLE = 7;
const int PUMP_PIN_WATER = 6;

const int MOTOR_BUTTON_PIN = 5;
const int PRIME_BUTTON_PIN = 4;

const int AXIDRAW_PIN = 2;

// motor + button state
bool motorState = false;
bool lastMotorButtonState = LOW;
bool motorButtonState = LOW;
bool motorRunning = false;

unsigned long previousMillis = 0;
const unsigned long motorOnTime = 50;  
const unsigned long motorOffTime = 4000; 

// prime button state
int primeState = false;
bool lastPrimeButtonState = LOW;
bool primeButtonState = LOW;

// axidraw pen state
int penDownState = LOW;
int lastPenDownState = LOW;

void setup() {
  Serial.begin(9600);
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(1600);
  Wire.setClock(400000);

  pinMode(AXIDRAW_PIN, INPUT);
  pinMode(PUMP_PIN_A, OUTPUT);
  pinMode(PUMP_PIN_B, OUTPUT);
  pinMode(MOTOR_BUTTON_PIN, INPUT);
  pinMode(PRIME_BUTTON_PIN, INPUT);
}

void loop() {
  motorButtonState = digitalRead(MOTOR_BUTTON_PIN);
  primeButtonState = digitalRead(PRIME_BUTTON_PIN);
  penDownState  = digitalRead(AXIDRAW_PIN);

  if (penDownState != lastPenDownState) {
    if (penDownState == HIGH) {
      Serial.println("Pen is DOWN");
      motorState = true;
      // motorRunning = true;
      // previousMillis = millis();
    } else {
      Serial.println("Pen is UP");
      motorState = false;
      // motorRunning = false;
    }
    lastPenDownState = penDownState;
  } else {
    if (primeButtonState == LOW && lastPrimeButtonState == HIGH) {
      primeState = !primeState;
      previousMillis = millis();
    }

    if (motorButtonState == LOW && lastMotorButtonState == HIGH) {
      motorState = !motorState;
      motorRunning = false;     
      previousMillis = millis(); 
    }
  }

  lastMotorButtonState = motorButtonState;
  lastPrimeButtonState = primeButtonState;

  if (motorState || primeState) {
    unsigned long currentMillis = millis();

    if (primeState) {
      motorRunning = true;
      previousMillis = currentMillis;
    } else if (motorRunning && currentMillis - previousMillis >= motorOnTime) {
      Serial.println("MOTOR OFF");
      // pumpOff();
      motorRunning = false;
      previousMillis = currentMillis;
    } else if (!motorRunning && currentMillis - previousMillis >= motorOffTime) {
      Serial.println("MOTOR ON");
      motorRunning = true;
      // pumpOn();
      previousMillis = currentMillis;
    }
  } else {
    motorRunning = false;
    // pumpOff();
  }

  if (motorRunning) {
    pumpOn();
  } else {
    pumpOff();
  }

  delay(1);
}

void pumpOn() {
  digitalWrite(PUMP_PIN_A, HIGH);
  digitalWrite(PUMP_PIN_B, LOW);
  // motorRunning = true;
}

void pumpOff() {
  digitalWrite(PUMP_PIN_A, LOW);
  digitalWrite(PUMP_PIN_B, LOW);
  // motorRunning = false;
}
