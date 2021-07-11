/**
 * Arduino Fruit Machine Reel Controller
 * Copyright (c) 2021 Playful Technology
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
// For 16 values on a reel with 48 steps, STEPS_PER_VALUE 3 etc.
#define STEPS_PER_VALUE 3
// What sort of shield is being used?
#define USE_CNCSHIELD
// Pin definitions for Arduino MEGA with RAMPS v1.4 shield, taken from Marlin pins.h 
#ifdef USE_RAMPS
  #define X_STEP_PIN         54
  #define X_DIR_PIN          55
  #define X_ENABLE_PIN       38
  #define X_MIN_PIN           3
  #define X_MAX_PIN           2
  #define Y_STEP_PIN         60
  #define Y_DIR_PIN          61
  #define Y_ENABLE_PIN       56
  #define Y_MIN_PIN          14
  #define Y_MAX_PIN          15
  #define Z_STEP_PIN         46
  #define Z_DIR_PIN          48
  #define Z_ENABLE_PIN       62
  #define Z_MIN_PIN          18
  #define Z_MAX_PIN          19
  #define E_STEP_PIN         26
  #define E_DIR_PIN          28
  #define E_ENABLE_PIN       24
  #define Q_STEP_PIN         36
  #define Q_DIR_PIN          34
  #define Q_ENABLE_PIN       30
  #define SDPOWER            -1
  #define SDSS               53
  #define LED_PIN            13
  #define FAN_PIN            9   
  #define PS_ON_PIN          12
  #define KILL_PIN           -1
  #define HEATER_0_PIN       10
  #define HEATER_1_PIN       8
  #define TEMP_0_PIN         13   // ANALOG NUMBERING
  #define TEMP_1_PIN         14   // ANALOG NUMBERING
#endif
// Pin definitions for Arduino UNO with CNC Shield v3 from https://wiki.keyestudio.com/Ks0160_keyestudio_CNC_Shield_V3
#ifdef USE_CNCSHIELD
  #define X_STEP_PIN          2
  #define X_DIR_PIN           5
  #define X_ENABLE_PIN        8 // Note all drivers are connected to same enable line
  #define X_MIN_PIN           9
  #define X_MAX_PIN           9 
  #define Y_STEP_PIN          3
  #define Y_DIR_PIN           6
  #define Y_ENABLE_PIN        8 // Note all drivers are connected to same enable line
  #define Y_MIN_PIN          10  
  #define Y_MAX_PIN          10  
  #define Z_STEP_PIN          4
  #define Z_DIR_PIN           7
  #define Z_ENABLE_PIN        8 // Note all drivers are connected to same enable line
  #define Z_MIN_PIN          11 
  #define Z_MAX_PIN          11  
  #define E_STEP_PIN         12 // Assumes jumper has been placed on board
  #define E_DIR_PIN          13 // Assumes jumper has been placed on board
  #define E_ENABLE_PIN        8 // Note all drivers are connected to same enable line
  #define LED_PIN            13
  #define SPN_EN_PIN         12 // Not usable if A has been attached   
  #define SPN_DIR            13 // Not usable if A has been attached
  #define COOL_EN_PIN        A3   
  #define ABORT_PIN          A0  
  #define HOLD_PIN           A1
  #define RESUME_PIN         A2
#endif

// CONSTANTS
const byte numReels = 3;

// INCLUDES
// See: http://www.airspayce.com/mikem/arduino/AccelStepper/
#include <AccelStepper.h>
// https://github.com/thomasfredericks/Bounce2
#include <Bounce2.h>

// GLOBALS
// Define each stepper and the pins it will use
AccelStepper stepperX(AccelStepper::DRIVER, X_STEP_PIN, X_DIR_PIN);
AccelStepper stepperY(AccelStepper::DRIVER, Y_STEP_PIN, Y_DIR_PIN);
AccelStepper stepperZ(AccelStepper::DRIVER, Z_STEP_PIN, Z_DIR_PIN);
AccelStepper stepperE(AccelStepper::DRIVER, E_STEP_PIN, E_DIR_PIN);
AccelStepper steppers[4] = {stepperX, stepperY, stepperZ, stepperE};


bool isSpinning = false;

// Use the MIN END STOP pins as inputs
// NOTE IF USING THE CNC SHIELD IT HAS 4 DRIVERS BUT ONLY SUPPORTS 3 ENDSTOPS!
int sensors[4] = {X_MIN_PIN, Y_MIN_PIN, Z_MIN_PIN, Z_MIN_PIN};

Bounce bounce = Bounce();

// The number on the reel that we'd like to display
int targetValues[4];

// Initial setup
void setup() {
  // Start a serial connection
  Serial.begin(115200);
  Serial.println(__FILE__ __DATE__);

  Serial.println(F("Send 999 to calibrate"));
  Serial.println(F("Send 100 to spin"));
  Serial.println(F("Send 200 to stop"));

  // Note: if using stepper drivers (A4988 etc.), these need to be enabled by pulling the EN pin low. 
  // On some shields, the pin is automatically pulled to GND via a jumper. If not, write a LOW signal to 
  // whatever pin is connected to the enable pin of the chip. 
  pinMode(X_ENABLE_PIN, OUTPUT);  digitalWrite(X_ENABLE_PIN, LOW);
  pinMode(Y_ENABLE_PIN, OUTPUT);  digitalWrite(Y_ENABLE_PIN, LOW);
  pinMode(Z_ENABLE_PIN, OUTPUT);  digitalWrite(Z_ENABLE_PIN, LOW);
  pinMode(E_ENABLE_PIN, OUTPUT);  digitalWrite(E_ENABLE_PIN, LOW);
  
  // Set the MIN END STOP pins as inputs
  pinMode(X_MIN_PIN, INPUT);
  pinMode(Y_MIN_PIN, INPUT);
  pinMode(Z_MIN_PIN, INPUT);

  bounce.attach(X_MIN_PIN, INPUT);
  
  // Stepper motor speed (steps/sec) and acceleration (steps per second^2)
  // These values need to be tweaked by experimentation!
  for(int i=0;i<numReels;i++){
    steppers[i].setMaxSpeed(60);
    steppers[i].setAcceleration(500);
  }
}

void CalibrateReel(int i) {
  Serial.print(F("Calibrating Reel #"));
  Serial.println(n);

  // Do three rotations
  steppers[i].move(NUM_STEPS * 3);
  steppers[i].setSpeed(50);
  while(steppers[i].distanceToGo() != 0) {
    steppers[i].runSpeedToPosition();
    bounce.update();
    if(bounce.rose()) {
      steppers[i].setCurrentPosition(0);
    }
  }

}





void CalibrateReels(){
  Serial.println(F("Calibrating Reels"));

  for(int i=0; i<numReels; i++) {
    // Store the current max speed
    float maxSpeed = steppers[i].maxSpeed();
    
    // Set the stepper to a slow speed
    steppers[i].setMaxSpeed(24);

    // We run the calibration twice for each stepper.
    // This helps reduce the chance of error in the reel actually starts in the correct position
    // Make at most one complete rotation from the current position
    steppers[i].move(NUM_STEPS);
    // Advance one step at a time until the sensor is obstructed
    do {
      steppers[i].run();
    } while(!digitalRead(sensors[i]));
    steppers[i].setCurrentPosition(0);

    // Then do a half-turn
    steppers[i].move(NUM_STEPS / 2);
    do {
      steppers[i].setSpeed(24);
      steppers[i].runSpeedToPosition();    
    } while(steppers[i].distanceToGo() != 0);
    
    steppers[i].move(NUM_STEPS);
    // Advance one step at a time until the sensor is obstructed
    do {
      steppers[i].run();
    } while(!digitalRead(sensors[i]));

    // Stop the motor and declare this the zero position
    steppers[i].stop();
    steppers[i].setCurrentPosition(0);
  
    // Carry on calling run() after stop() - see https://groups.google.com/g/accelstepper/c/i92AHogeAXU?pli=1
    for(int i=0; i<10; i++){
      steppers[i].run();
    }
    
    // Now that calibration is complete, restore the normal max motor speed
    steppers[i].setMaxSpeed(maxSpeed);
    delay(500);
  }
}

void RandomSpin() {
  Serial.println(F("Random Spin"));
  // Assign a random, increasing target to each reel
  for(int i=0; i<numReels; i++) {
    steppers[i].move(random(NUM_STEPS*i,NUM_STEPS*(i+1)));
  }

  while(true){
    bool allAtTarget = true;
    for(int i=0; i<numReels; i++) {
      steppers[i].run();
      if(steppers[i].distanceToGo() != 0) { allAtTarget = false; }
    }
    if(allAtTarget) break;
  }
}



void SpinReels(){
  Serial.println(F("Spinning Reels"));
  for(int i=0; i<numReels; i++) {
    steppers[i].setSpeed(40);
  }
  isSpinning = true;
}

void StopReel(int n) {
  steppers[n].setSpeed(0);
  steppers[n].move(0);
  steppers[n].stop();
  steppers[n].runToPosition();
}

void StopReels(){
  Serial.println(F("Stopping Reels"));
  for(int i=0; i<numReels; i++) {
    StopReel(i); 
  }
  isSpinning = false;
}

void SpinReels(float speed, unsigned long duration) {
  Serial.println(F("Spinning Reels"));
  for(int i=0; i<numReels; i++) {
    steppers[i].setSpeed(speed);
  }
  unsigned long startTime = millis();
  while(millis() < startTime + duration) {
    for(int i=0; i<numReels; i++) {
      steppers[i].runSpeed();
      // Keep the stepper position within the range
      if(steppers[i].currentPosition() > NUM_STEPS) {
        steppers[i].setCurrentPosition(steppers[i].currentPosition() - NUM_STEPS); 
        }
     }
  }
}

void loop() {

  bounce.update();

  // PROCESS INPUT
  // Test whether any input has been received on the serial connection
  if(Serial.available() > 0){
    // Read any integer sent
    int command = Serial.parseInt();
    // Sending 999 re-calibrates the reel
    // (don't use 0 as that is the value parseInt() returns from reading, e.g. a linebreak)
    if(command == 999){
      CalibrateReels();
      return;
    }
    else if(command == 100) {
      SpinReels();
      return;
    }
    else if(command == 200) {
      StopReels();
      return;
    }
    else if(command == 300) {
      RandomSpin();
      return;
    }
  }
  // This gets called every frame, and processes any movement of the stepper as necessary
  for(int i=0; i<numReels; i++){
    if(isSpinning) {
      steppers[i].runSpeed();
    }
    else {
      steppers[i].run();
    }
  }
}
