/**
 * Arduino "Max-A-Million" Fruit Machine Controller
 * Copyright (c) 2021 Playful Technology
 */

// INCLUDES
#include "src/common.h"
// https://github.com/thomasfredericks/Bounce2
#include <Bounce2.h>
// See: http://www.airspayce.com/mikem/arduino/AccelStepper/
#include <AccelStepper.h>
// For interfacing with MP3 sound board
#include "SoftwareSerial.h"
#include "SerialMP3Player.h"
// For starburst VFD display
#include "src/Samsung_16LF01_VFD.h"

// DEFINES
// The total number of steps required for one complete revolution of the reel motor
#define NUM_STEPS 48 // - 7.5° step angle = 48 steps
// Number of steps between adjacent symbols on the reel
#define STEPS_PER_VALUE 4 // - i.e. NUM_STEPS / Number of Symbols
// RAMPS Pin definitions from https://github.com/MarlinFirmware/Marlin/blob/2.0.x/Marlin/src/pins/ramps/pins_RAMPS.h
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
#define LED_PIN            13
#define PS_ON_PIN          12
// Note Pin D8 is wired to the separate 11A power supply and does not share any power with the rest of the board
#define HEATER_1_PIN       8
// Pins D9 and D10 share the main 5A power supply. They are "common anode", in that the +ve terminals are connected 
// together, and are switched on only by grounding the separate -ve terminals.
#define FAN_PIN            9
#define HEATER_0_PIN       10

// CONSTANTS
const byte numReels = 4;
// Use the X/Y min/max end stop pins to wire the opto sensors
const byte optoSensorPins[numReels] = {X_MIN_PIN, X_MAX_PIN, Y_MIN_PIN, Y_MAX_PIN};
// Inputs are connected to GPIO pins on the AUX-2 board
const byte numButtons = 7;
const byte buttonPins[numButtons] = {63, 40, 42, 59, 64, 44, 66};
const byte coinPin = 65; // Top-right of AUX-2
// Serial audio output connected to AUX-1
const byte audioTxPin = 57;
const byte audioRxPin = 58;
// LED 7-Segment driver connected to SERVOs
const byte LEDClockPin = 6;
const byte LEDLatchPin = 5;
const byte LEDDataPin = 4;
// VFD Display connected to AUX-3
const byte VFDClockPin = 52;
const byte VFDResetPin = 50;
const byte VFDDataPin = 51;

// GLOBALS
// Define each stepper and the pins it will use
AccelStepper stepperX(AccelStepper::DRIVER, X_STEP_PIN, X_DIR_PIN);
AccelStepper stepperY(AccelStepper::DRIVER, Y_STEP_PIN, Y_DIR_PIN);
AccelStepper stepperZ(AccelStepper::DRIVER, Z_STEP_PIN, Z_DIR_PIN);
AccelStepper stepperE(AccelStepper::DRIVER, E_STEP_PIN, E_DIR_PIN);
AccelStepper steppers[4] = {stepperX, stepperY, stepperZ, stepperE};
// Array of Bounce objects to help read each time a reel tab spins past the opto-sensor
Bounce optoSensors[numReels] = {};
Bounce buttons[numButtons] = {};
Bounce coinSensor;
SoftwareSerial softSerial(audioRxPin, audioTxPin);
SerialMP3Player mp3;
Samsung_16LF01_VFD vfd(VFDClockPin, VFDDataPin, VFDResetPin);

// Initial setup
void setup() {
  // Start a serial connection
  Serial.begin(115200);
  // Don't wait for ages for serial input
  Serial.setTimeout(50); 
  Serial.println(__FILE__ __DATE__);

  // Stepper drivers need to be enabled by pulling the EN pin low. 
  // On some shields, the pin is automatically pulled to GND via a jumper. If not, write a LOW signal to 
  // whatever pin is connected to the enable pin of the chip. 
  pinMode(X_ENABLE_PIN, OUTPUT);  digitalWrite(X_ENABLE_PIN, LOW);
  pinMode(Y_ENABLE_PIN, OUTPUT);  digitalWrite(Y_ENABLE_PIN, LOW);
  pinMode(Z_ENABLE_PIN, OUTPUT);  digitalWrite(Z_ENABLE_PIN, LOW);
  pinMode(E_ENABLE_PIN, OUTPUT);  digitalWrite(E_ENABLE_PIN, LOW);

  // Loop over every reel
  for(int i=0; i<numReels; i++) {
    // Set the optosensor pins as inputs and attach pullups and debounce objects to them
    optoSensors[i].attach(optoSensorPins[i], INPUT_PULLUP);
    
    // AccelStepper speed is express in steps/sec
    // So, for a reel with NUM_STEPS 48, setMaxSpeed(60), means (60*60/48)= 75RPM
    steppers[i].setMaxSpeed(60);
    steppers[i].setAcceleration(150);
  }
  // Initialise input buttons
  for(int i=0; i<numButtons; i++){
    buttons[i].attach(buttonPins[i], INPUT_PULLUP);
  }
  coinSensor.attach(coinPin, INPUT_PULLUP);

  softSerial.begin(9600);
  mp3.begin(softSerial);        
  mp3.sendCommand(CMD_SEL_DEV, 0, 2);   // Select SD-card as storage device

  // Initialize the VFD display (number of digits to use, brightness (in 1/31ths))
  vfd.begin (16,25);
  // Display a welcome static message
  vfd.print("Clued Up!");

  Serial.println(F("Send 999 to calibrate"));
  Serial.println(F("Send 1/2/3/4 to nudge reels up"));
  Serial.println(F("Send 5/6/7/8 to nudge reels down"));  
  Serial.println(F("Send 200 for random spin"));  
  Serial.println(F("Send 300 for winning spin"));

  // Calibrate reels to determine "zero" point
  CalibrateReels();
}

/**
 * Reset procedure as described in 20RM_MK2_STI manual
 *
 * I. Drive the motor at approximately 50RPM. 
 * II. At every motor step change, monitor the optic output. Immediately the tab is detected by the optic cease driving the motor. 
 * III. Wait 500mS then power up the motor on the Black and Yellow windings. (Black and Yellow are the A windings of both coils)
 * IV. Wait 500mS, this allows the motor/reel band to settle in position. Check the tab is in the optic. 
 *     If not repeat steps i] to iv], if the tab is still not in the optic there is a fault.
 * V. The reel mechanism and software are now initialised.
 */
void CalibrateReels(){
  Serial.println(F("Calibrating Reels"));

  for(int i=0; i<numReels; i++) {
    // Store the current max speed
    float maxSpeed = steppers[i].maxSpeed();
    
    // Set the stepper to a slower speed for calibration if desired
    // Manual suggests calibrating at 50RPM
    steppers[i].setMaxSpeed(50.0F/60.0F*NUM_STEPS);

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
      // When the tab crosses through the sensor for the second time
      if(numTriggers > 1){
        // Zero everything for this reel
        steppers[i].setCurrentPosition(0);
        steppers[i].setSpeed(0);
        steppers[i].move(0);
        steppers[i].stop();
        break;
      }
    }

    // Carry on calling run() after stop() - see https://groups.google.com/g/accelstepper/c/i92AHogeAXU?pli=1
    for(int n=0; n<10; n++){
      steppers[i].run();
    }

    delay(500);

    steppers[i].move(1);

    
    // Now that calibration is complete, restore the normal max motor speed
    steppers[i].setMaxSpeed(maxSpeed);
    delay(500);

    Serial.println(F("Stepper calibrated"));
  }
  Serial.println(F("Steppers calibrated"));
}


void loop() {

  // INPUTS
  // Update current state of all inputs
  for(int i=0; i<numReels; i++) { optoSensors[i].update(); }
  for(int i=0; i<numButtons; i++) { buttons[i].update(); }
  coinSensor.update();

  if(coinSensor.fell()){
        
  }
  if(buttons[2].fell()) {
    mp3.play(1);
    nudge(1,1);
  }
  if(buttons[3].fell()) {
    mp3.play(2);
    nudge(2,1);
  }
  if(buttons[4].fell()) {
    mp3.play(3);
    nudge(3,1);
  }   
  
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
      nudge(0, 1);
    }
    else if(command == 2){
      nudge(1, 1);
    }
    else if(command == 3){
      nudge(2, 1);
    }
    else if(command == 4){
      nudge(3, 1);
    }
    else if(command == 5){
      nudge(0, -1);
    }
    else if(command == 6){
      nudge(1, -1);
    }
    else if(command == 7){
      nudge(2, -1);
    }
    else if(command == 8){
      nudge(3, -1);
    }
    else if(command == 200) {
      randomSpin();
    }
    else if(command == 300) {
      winningSpin();
    }
  }
  // This gets called every frame, and processes any movement of the steppers as necessary
  for(int i=0; i<numReels; i++){
    steppers[i].run();
  }
}

/**
 * Nudge specified reel a certain number of positions
 */
void nudge(int reelNum, int positions){
  steppers[reelNum].move(positions * STEPS_PER_VALUE);
}

/**
 * Spin all reels to an arbitrary position
 */
void randomSpin(){
  // Assign a random, increasing target to each reel
  for(int i=0; i<numReels; i++) {
    // Set *relative* position for each target
    steppers[i].move(random(NUM_STEPS*(i+2),NUM_STEPS*(i+4)));
  }
}

/**
 * Spin all reels and stop on a whole number of revolutions from the zero point
 */
void winningSpin(){
  for(int i=0; i<numReels; i++) {
    // Calculate *absolute* position that is next multiple of whole number of revolutions
    long target = roundUp(steppers[i].currentPosition(), NUM_STEPS);
    target += NUM_STEPS * ((i+1)*2);
    // moveTo is absolute
    steppers[i].moveTo(target);
  }
}
