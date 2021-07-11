/**
 * Arduino Fruit Machine Feature Reel Controller
 * Copyright (c) 2018 Playful Technology
 *
 * Tested with a Saia-Burgess 6-wire 50Ω/ph UFD23N07RAZ18 stepper motor, recovered from the
 * top reel assembly of a Bell Fruit "Snakes and Ladders" fruit machine.
 * https://www.johnsonelectric.com/en/product-technology/motion/stepper-motors/rotary-stepper-motors
 * 
 * If using an Arudino UNO "CNC shield", heck the following for pin mapping and availability:
 * https://wiki.keyestudio.com/Ks0160_keyestudio_CNC_Shield_V3
 * https://blog.protoneer.co.nz/arduino-cnc-shield-v3-00-assembly-guide/
 */
// DEFINES
// The total number of steps required for one complete revolution of the motor
// Common values for motors are: 
// - 7.5° step angle = 48 steps
// - 1.8° step angle = 200 steps
#define NUM_STEPS 48
// There are 12 different values on the reel - numbers 1-12. So, define the number
// of steps between each value as NUM_STEPS / 12.
// For 20 values on a reel with 200 steps, STEPS_PER_VALUE 10 etc.
#define STEPS_PER_VALUE 3

// INCLUDES
// Not strictly necessary, but the AccelStepper library provides nice utility functions for acceleration/deceleration
// See: http://www.airspayce.com/mikem/arduino/AccelStepper/
#include <AccelStepper.h>

// CONSTANTS
// The sensor sends a HIGH signal when it is blocked, which occurs when the 12 is facing the front
const byte sensorPin = 9;

// GLOBALS
// If using a motor driver (e.g. L298N), use FULL4WIRE constructor specifying pins A1, A2, B1, B2
// AccelStepper stepper(AccelStepper::FULL4WIRE, 7, 8, 9, 10);
// If using a stepper driver (e.g. A4988), use DRIVER constructor with stepPin, dirPin
AccelStepper stepper(AccelStepper::DRIVER, 2, 5);
// The number on the reel that we'd like to display
int targetValue;

// Initial setup
void setup() {
  // Start a serial connection
  Serial.begin(115200);
  Serial.println(__FILE__ __DATE__);

  // Note - if using a CNC shield, the stepper drivers need to be enabled. Sometimes, the enabled pin is automatically
  // pulled to ground by a jumper but, if not, write a LOW signal to the enable pin. 
  pinMode(8, OUTPUT);
  digitalWrite(8, LOW);
  
  // Set the sensor pin as an input
  pinMode(sensorPin, INPUT);
  // Stepper motor speed (steps/sec) and acceleration (steps per second^2)
  stepper.setMaxSpeed(50);
  stepper.setAcceleration(500);

  for(int i=i;i<1000;i++){
    stepper.runSpeed();
  }

  CalibrateReel();
}

void CalibrateReel(){
  Serial.println("Calibrating");
  // Set the stepper to a slow speed
  stepper.setMaxSpeed(50);
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
    // Sending 999 re-calibrates the reel
    // (don't use 0 as that is the value parseInt() returns from reading, e.g. a linebreak)
    if(targetValue == 999){
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
