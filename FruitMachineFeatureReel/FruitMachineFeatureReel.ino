/**
 * Arduino Fruit Machine Feature Reel Controller
 * Copyright (c) 2018 Playful Technology
 *
 * Tested with a Saia-Burgess 6-wire 50Ω/ph UFD23N07RAZ18 stepper motor, recovered from the
 * top reel assembly of a Bell Fruit "Snakes and Ladders" fruit machine.
 * https://www.johnsonelectric.com/en/product-technology/motion/stepper-motors/rotary-stepper-motors
 */
// DEFINES
// The total number of steps required for one complete revolution
// This reel has a (common) step angle of 7.5°, which means 48 full steps  (for 1.8' = 200 steps)
#define NUM_STEPS 200
// There are 12 different values on the reel - numbers 1-12. So, define the number
// of steps between each value as NUM_STEPS / 12
#define STEPS_PER_VALUE 10

// INCLUDES
// Not strictly necessary, but the AccelStepper library provides nice utility functions for acceleration/deceleration
// See: http://www.airspayce.com/mikem/arduino/AccelStepper/
#include <AccelStepper.h>

// CONSTANTS
// The sensor sends a HIGH signal when it is blocked, which occurs when the 12 is facing the front
const byte sensorPin = 2;

// GLOBALS
// Define a stepper and the pins it will use
AccelStepper stepper(AccelStepper::FULL4WIRE, 7, 8, 9, 10);
// The number on the reel that we'd like to display
int targetValue;

// Initial setup
void setup() {
  // Start a serial connection
  Serial.begin(9600);
  // Set the sensor pin as an input
  pinMode(sensorPin, INPUT);
  // Stepper motor speed and acceleration
  stepper.setMaxSpeed(100);
  stepper.setAcceleration(100);
  CalibrateReel();
}

void CalibrateReel(){
  // Set the stepper to a slow speed
  stepper.setMaxSpeed(1000);
  // Make at most one complete rotation from the current position
  stepper.move(NUM_STEPS*4);
  // Advance one step at a time until the sensor is obstructed
  while(!digitalRead(sensorPin)) {
    stepper.run();
  }
  // Stop the motor and declare this the zero position
  stepper.stop();
  stepper.setCurrentPosition(0);
  // Now that calibration is complete, set faster normal motor speed
  stepper.setMaxSpeed(100);
}

void loop() {
  // Test whether any input has been received on the serial connection
  if(Serial.available() > 0){
    // Read any integer sent
    targetValue = Serial.parseInt();
    // Sending a zero re-calibrates the reel
    if(targetValue == 0){
      CalibrateReel();
      return;
    }   
    // A little debug
    Serial.print("Now going to:");
    Serial.println(targetValue);
    // We subtract the result from NUM_STEPS because the reel labels increase anti-clockwise rather than clockwise
    stepper.moveTo(NUM_STEPS - (STEPS_PER_VALUE * targetValue));
  }
  // This gets called every frame, and processes any movement of the stepper as necessary
  stepper.run();
}
