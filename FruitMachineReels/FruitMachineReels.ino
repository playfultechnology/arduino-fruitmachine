/**
 * Arduino Fruit Machine Reel Controller
 * Copyright (c) 2021 Playful Technology
 */
// DEFINES
// The total number of steps required for one complete revolution of the motor
// Common values for motors are: 
// - 7.5° step angle = 48 steps
// - 1.8° step angle = 200 steps
#define NUM_STEPS 48
// STEPS_PER_VALUE is the number of steps between adjacent symbols on the reel
// Calculated as NUM_STEPS / (Number of Symbols)
// For 16 values on a 7.5° step angle reel (48 steps), STEPS_PER_VALUE 3 etc.
// For 12 values on a 7.5° step angle reel (48 steps), STEPS_PER_VALUE 4 etc.
// For 20 values on a  1.8° step angle reel (200 steps), STEPS_PER_VALUE 10 etc.
#define STEPS_PER_VALUE 3
// What sort of shield is being used? USE_CNCSHIELD or USE_RAMPS
#define USE_CNCSHIELD
// Pin definitions for Arduino MEGA with RAMPS v1.4 shield
// See https://github.com/MarlinFirmware/Marlin/blob/2.0.x/Marlin/src/pins/ramps/pins_RAMPS.h
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
// Pin definitions for Arduino UNO with CNC Shield v3
// see https://wiki.keyestudio.com/Ks0160_keyestudio_CNC_Shield_V3
// see https://blog.protoneer.co.nz/arduino-cnc-shield-v3-00-assembly-guide/
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
// Use the MIN END STOP pins as inputs
// NOTE IF USING THE CNC SHIELD IT HAS 4 DRIVERS BUT ONLY SUPPORTS 3 ENDSTOPS, so using HOLD_PIN instead
const byte sensorPins[4] = {X_MIN_PIN, Y_MIN_PIN, Z_MIN_PIN, HOLD_PIN};

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
// Array of Bounce objects to help read each time reel tab spins past the opto-sensor
Bounce optoSensors[4] = {};

// Initial setup
void setup() {
  // Start a serial connection
  Serial.begin(115200);
  Serial.setTimeout(50); // Don't wait for ages for input
  Serial.println(__FILE__ __DATE__);

  Serial.println(F("Send 999 to calibrate"));
  Serial.println(F("Send 1/2/3/4 to nudge reels up"));
  Serial.println(F("Send 5/6/7/8 to nudge reels down"));  
  Serial.println(F("Send 200 for random spin"));  
  Serial.println(F("Send 300 for winning spin"));

  // Note: if using stepper drivers (A4988 etc.), these need to be enabled by pulling the EN pin low. 
  // On some shields, the pin is automatically pulled to GND via a jumper. If not, write a LOW signal to 
  // whatever pin is connected to the enable pin of the chip. 
  pinMode(X_ENABLE_PIN, OUTPUT);  digitalWrite(X_ENABLE_PIN, LOW);
  pinMode(Y_ENABLE_PIN, OUTPUT);  digitalWrite(Y_ENABLE_PIN, LOW);
  pinMode(Z_ENABLE_PIN, OUTPUT);  digitalWrite(Z_ENABLE_PIN, LOW);
  pinMode(E_ENABLE_PIN, OUTPUT);  digitalWrite(E_ENABLE_PIN, LOW);
  
  // Set the MIN END STOP pins as inputs
  for(int i=0; i<numReels; i++) {
    optoSensors[i].attach(sensorPins[i], INPUT);
  }
  
  // Stepper motor speed (steps/sec) and acceleration (steps per second^2)
  // These values need to be tweaked by experimentation!
  for(int i=0;i<numReels;i++){
    steppers[i].setMaxSpeed(60);
    steppers[i].setAcceleration(150);
  }
}

void CalibrateReels(){
  Serial.println(F("Calibrating Reels"));

  for(int i=0; i<numReels; i++) {
    // Store the current max speed
    float maxSpeed = steppers[i].maxSpeed();
    
    // Set the stepper to a slower speed for calibration if desired
   // steppers[i].setMaxSpeed(24);

    // Set a target point several rotations away from the current position
    steppers[i].move(NUM_STEPS*4);

    // numTriggers is the number of times the reel tab crosses the opto-sensor
    int numTriggers = 0;

    // Process until we reach the target
    while(steppers[i].distanceToGo() != 0) {
      steppers[i].run();
      optoSensors[i].update();
      // Most opto-sensors go HIGH when an obstruction is detected
      if(optoSensors[i].rose()) {
        numTriggers++;
      }
      // When the tab crosses through the sensor for the third time
      if(numTriggers > 2){
        // Zero everything for this reel
        steppers[i].setCurrentPosition(0);
        steppers[i].setSpeed(0);
        steppers[i].move(0);
        steppers[i].stop();
        break;
      }
    }

    // Carry on calling run() after stop() - see https://groups.google.com/g/accelstepper/c/i92AHogeAXU?pli=1
    for(int i=0; i<10; i++){
      steppers[i].run();
    }
    
    // Now that calibration is complete, restore the normal max motor speed
    steppers[i].setMaxSpeed(maxSpeed);
    delay(500);

   // Serial.println(F("Stepper calibrated"));
  }
}

// See https://forum.arduino.cc/t/rounding-an-integer-up-only/350784/3
long roundUp(long val, long factor) {
  long rem = val % factor;
  val -= rem;
  if (rem > 0) val += factor;
  return val;
}

void loop() {

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
    else if(command == 1){
      steppers[0].move(STEPS_PER_VALUE);
    }
    else if(command == 2){
      steppers[1].move(STEPS_PER_VALUE);
    }
    else if(command == 3){
      steppers[2].move(STEPS_PER_VALUE);
    }
    else if(command == 4){
      steppers[3].move(STEPS_PER_VALUE);
    }
    else if(command == 5){
      steppers[0].move(-STEPS_PER_VALUE);
    }
    else if(command == 6){
      steppers[1].move(-STEPS_PER_VALUE);
    }
    else if(command == 7){
      steppers[2].move(-STEPS_PER_VALUE);
    }
    else if(command == 8){
      steppers[3].move(-STEPS_PER_VALUE);
    }
    else if(command == 200) {
      // Assign a random, increasing target to each reel
      for(int i=0; i<numReels; i++) {
        // Set *relative* position for each target
        steppers[i].move(random(NUM_STEPS*(i+2),NUM_STEPS*(i+4)));
      }
    }
    else if(command == 300) {
      for(int i=0; i<numReels; i++) {
        // Calculate *absolute* position that is next multiple of whole number of revolutions
        long target = roundUp(steppers[i].currentPosition(), NUM_STEPS);
        target += NUM_STEPS * ((i+1)*2);
        steppers[i].moveTo(target);
      }
    }
  }
  // This gets called every frame, and processes any movement of the steppers as necessary
  for(int i=0; i<numReels; i++){
    steppers[i].run();
  }
}
