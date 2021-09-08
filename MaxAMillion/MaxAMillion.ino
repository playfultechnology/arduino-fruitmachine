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
#include "SerialMP3Player.h"
// For starburst VFD display
#include "src/Samsung_16LF01_VFD.h"
// I2C connection to 7-segment daughterboard
#include <Wire.h>

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
#define NO_SERIALMP3_DELAY

// CONSTANTS
const byte numReels = 4;
// Use the X/Y min/max end stop pins to wire the opto sensors
const byte optoSensorPins[numReels] = {X_MIN_PIN, X_MAX_PIN, Y_MIN_PIN, Y_MAX_PIN};
const byte stepperEnablePins[numReels] = {X_ENABLE_PIN, Y_ENABLE_PIN, Z_ENABLE_PIN, E_ENABLE_PIN};
// Inputs are connected to GPIO pins on the AUX-2 board
const byte numButtons = 7;
const byte buttonPins[numButtons] = {63, 59, 40, 64, 42,  44, 65};
const byte coinPin = 66;
// VFD Starburst Display connected to AUX-3
const byte VFDClockPin = 51;
const byte VFDResetPin = 52;
const byte VFDDataPin = 50;
// RC input channels connected to Servos
const byte numRCs = 4;
const byte RCinputPins[numRCs] = {11, 6, 5, 4};
// Relay outputs connected to lower AUX-4 pins
const byte numRelays = 8;
const byte relayOutPins[numRelays] = {35, 37, 39, 41, 43, 45, 47, 32};
// I2C address of daughterboard that will control 7-segment LED display
const byte slave7SegmentAddress = 9;


// GLOBALS
// Define each stepper and the pins it will use
AccelStepper stepperX(AccelStepper::DRIVER, X_STEP_PIN, X_DIR_PIN);
AccelStepper stepperY(AccelStepper::DRIVER, Y_STEP_PIN, Y_DIR_PIN);
AccelStepper stepperZ(AccelStepper::DRIVER, Z_STEP_PIN, Z_DIR_PIN);
AccelStepper stepperE(AccelStepper::DRIVER, E_STEP_PIN, E_DIR_PIN);
AccelStepper steppers[4] = {stepperX, stepperY, stepperZ, stepperE};

unsigned long vfdTimer = 0;
char message[64] = "Welcome to CluedUp casino! Insert one quarter to play!         ";
char digits[17] = "                ";
int firstDigit = 0;
int lastDigit = strlen(message) - 1;

unsigned int credits;

byte stepperActive[4] = {0,0,0,0};


bool stepperWasRunning[numReels] = {};

// Array of Bounce objects to help read each time a reel tab spins past the opto-sensor
Bounce optoSensors[numReels] = {};
Bounce buttons[numButtons] = {};
Bounce RCinputs[numRCs] = {};
Bounce coinSensor;
SerialMP3Player mp3;

Samsung_16LF01_VFD vfd(VFDClockPin, VFDDataPin, VFDResetPin);

// Initial setup
void setup() {
  // Start a serial connection
  Serial.begin(115200);
  // Don't wait for ages for serial input
  Serial.setTimeout(50); 
  Serial.println(__FILE__ __DATE__);

  // Loop over every reel
  for(int i=0; i<numReels; i++) {

    // Stepper drivers need to be enabled by pulling the EN pin low. 
    // On some shields, the pin is automatically pulled to GND via a jumper. If not, write a LOW signal to 
    // whatever pin is connected to the enable pin of the chip.
    pinMode(stepperEnablePins[i], OUTPUT);
    digitalWrite(stepperEnablePins[i], LOW);
    
    // Set the optosensor pins as inputs and attach pullups and debounce objects to them
    optoSensors[i].attach(optoSensorPins[i], INPUT_PULLUP);
    
    // AccelStepper speed is express in steps/sec
    // So, for a reel with NUM_STEPS 48, setMaxSpeed(60), means (60*60/48)= 75RPM
    steppers[i].setMaxSpeed(60);

    steppers[0].setMaxSpeed(60);
    steppers[1].setMaxSpeed(20);
    steppers[2].setMaxSpeed(24);
    steppers[3].setMaxSpeed(28);


    steppers[0].setSpeed(60);
    steppers[1].setSpeed(20);
    steppers[2].setSpeed(24);
    steppers[3].setSpeed(24); 

    
    steppers[i].setAcceleration(150);
  }
  // Initialise input buttons
  for(int i=0; i<numButtons; i++){
    buttons[i].attach(buttonPins[i], INPUT_PULLUP);
  }
  coinSensor.attach(coinPin, INPUT_PULLUP);

  for(int i=0; i<numRCs; i++){
    RCinputs[i].attach(RCinputPins[i], INPUT_PULLUP);
  }
  for(int i=0; i<numRelays; i++){
    pinMode(relayOutPins[i], OUTPUT);
    digitalWrite(relayOutPins[i], HIGH);
  }
  
  // Join I2C bus (address optional for master)
  Wire.begin(); 

  // MP3 player is connected to pins 18/19, A.K.A. ZMin/ZMax, A.K.A. Serial1
  Serial1.begin(9600);
  mp3.begin(Serial1);        
  mp3.sendCommand(CMD_SEL_DEV, 0, 2);   // Select SD-card as storage device

  // Initialize the VFD display (number of digits to use, brightness (in 1/31ths))
  vfd.begin (16,25);
  // Display a welcome static message
  vfd.print("Clued Up!");
  delay(1000);
  vfd.clear();
  /*Go to the first digit display*/
  vfd.setCursor(0);

  // Display a welcome static message
  vfd.print("Calibrating...");
  delay(1000);
  vfd.clear();
  /*Go to the first digit display*/
  vfd.setCursor(0);

  Serial.println(F("Send 999 to calibrate"));
  Serial.println(F("Send 1/2/3/4 to nudge reels up"));
  Serial.println(F("Send 5/6/7/8 to nudge reels down"));  
  Serial.println(F("Send 200 for random spin"));  
  Serial.println(F("Send 300 for winning spin"));

  // Calibrate reels to determine "zero" point
  // CalibrateReels();
}


void StartCountdown() {
  // Write command to slave
  Wire.beginTransmission(slave7SegmentAddress);
  Wire.write(0);
  Wire.endTransmission();
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

    digitalWrite(stepperEnablePins[i], LOW);
    
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
        //logMessage(F("Optosensor triggered"));
        //mp3.play(1);
        
        numTriggers++;
      }
      // When the tab crosses through the sensor for the third time
      if(numTriggers > 2){
        // Immediately set motor speed to zero
        //steppers[i].setSpeed(0);
        //steppers[i].stop();
        steppers[i].setCurrentPosition(0);
        //steppers[i].move(NUM_STEPS);
        delay(500);
        break;
      }
    }
    if(numTriggers <= 1) {
      logMessage(F("Calibration failed"));
    }
    // Carry on calling run() after stop() - see https://groups.google.com/g/accelstepper/c/i92AHogeAXU?pli=1
    for(int n=0; n<50; n++){
      steppers[i].run();
    }
    delay(500);

    // According to calibration procedure, we then need to move one more step
    //steppers[i].move(-1);
    //while(steppers[i].distanceToGo() != 0) {
    //  steppers[i].run();
   // }
    
    // Set zero position
    steppers[i].setCurrentPosition(0);

    // Now that calibration is complete, restore the normal max motor speed
    steppers[i].setMaxSpeed(maxSpeed);

    // Disable motor driver to conserve power
    // digitalWrite(stepperEnablePins[i], HIGH);

    mp3.play(i+1);
    
    delay(500);
  }
  Serial.println(F("Calibration complete"));
}


void loop() {


if(credits == 0){
  strcpy(message, "Welcome to CluedUp casino! Insert one quarter to play!         ");
}
else {
  strcpy(message, "Spin the reels! Get three Jackpots to win a prize!             ");
}

  unsigned long now = millis();

  if(now - vfdTimer > 250){

    for(int i=0; i<15; i++) {
      digits[i] = digits[i + 1];
    }
    digits[15] = message[firstDigit];
  
    if (firstDigit == lastDigit)
      firstDigit = 0;
    else
      firstDigit = firstDigit + 1;
  
    for (int i=0; i<16; i++) {
      vfd.print(digits[i]);
  
      /*Workaround, "points" and "commas" are in the same digit*/
      if (digits[i] == ',' || digits[i] == '.')
        vfd.print(" ");
    }
    vfdTimer = now;
  }
  
  // INPUTS
  // Update current state of all inputs
  for(int i=0; i<numReels; i++) { optoSensors[i].update(); }
  for(int i=0; i<numButtons; i++) { buttons[i].update(); }
  for(int i=0; i<numRCs; i++) { RCinputs[i].update(); }
  coinSensor.update();

  if(coinSensor.fell()){
    logMessage(F("Coin inserted"));

    credits++;
    Wire.beginTransmission(9);
    Wire.write(0x01);
    Wire.endTransmission();
    
    mp3.play(17); 
  }
  if(buttons[0].fell()) {
    logMessage(F("Button 0 pressed"));
    mp3.play(7);
    digitalWrite(relayOutPins[0], LOW);
    ResetReels();
  }
  else if(buttons[0].rose()) {
    logMessage(F("Button 0 released"));
    digitalWrite(relayOutPins[0], HIGH);
  }    
  if(buttons[1].fell()) {
    logMessage(F("Button 1 pressed"));
    mp3.play(1);
    digitalWrite(relayOutPins[1], LOW);
    nudge(1,1);
    nudge(2,2);  
  }
  else if(buttons[1].rose()) {
    logMessage(F("Button 1 released"));
    digitalWrite(relayOutPins[1], HIGH);
  }   
  if(buttons[2].fell()) {
    logMessage(F("Button 2 pressed"));
    digitalWrite(relayOutPins[2], LOW);
    mp3.play(2);
    //nudge(2,1);
    //nudge(3,1); 
    stepperActive[1] = !stepperActive[1];
  }
  else if(buttons[2].rose()) {
    logMessage(F("Button 2 released"));
    digitalWrite(relayOutPins[2], HIGH);
  }   
  if(buttons[3].fell()) {
    logMessage(F("Button 3 pressed"));
    digitalWrite(relayOutPins[3], LOW);
    mp3.play(3);
    //nudge(2,1);
    //nudge(4,1); 
    stepperActive[2] = !stepperActive[2];
  }
  else if(buttons[3].rose()) {
    logMessage(F("Button 3 released"));
    digitalWrite(relayOutPins[3], HIGH);
  }   
  if(buttons[4].fell()) {
    logMessage(F("Button 4 pressed"));
    digitalWrite(relayOutPins[4], LOW);
    mp3.play(4);
    /*
    Wire.beginTransmission(9);
    Wire.write(0x04);
    Wire.endTransmission();
    nudge(3,1);
    */
    stepperActive[3] = !stepperActive[3];
  }
  else if(buttons[4].rose()) {
    logMessage(F("Button 4 released"));
    digitalWrite(relayOutPins[4], HIGH);
  }   
  if(buttons[5].fell()) {
    logMessage(F("Button 5 pressed"));
    digitalWrite(relayOutPins[5], LOW);
    mp3.play(5);
    Wire.beginTransmission(9);
    Wire.write(0x05);
    Wire.endTransmission();
    randomSpin();
  }
  else if(buttons[5].rose()) {
    logMessage(F("Button 5 released"));
    digitalWrite(relayOutPins[5], HIGH);
  }   
  if(buttons[6].fell()) {
    logMessage(F("Button 6 pressed"));
    digitalWrite(relayOutPins[6], LOW);
    mp3.play(6);
    Wire.beginTransmission(9);
    Wire.write(0x06);
    Wire.endTransmission();
    winningSpin();
  }
  else if(buttons[6].rose()) {
    logMessage(F("Button 6 released"));
    digitalWrite(relayOutPins[6], HIGH);
  }   
  if(RCinputs[0].fell()) {
    logMessage(F("RC Channel 0 pressed"));
    Wire.beginTransmission(9);
    Wire.write(0x00);
    Wire.endTransmission();
    randomSpin();
    mp3.play(6);
  }
  else if(RCinputs[0].rose()) {
    logMessage(F("RC Channel 0 released"));
    digitalWrite(relayOutPins[7], HIGH);
  }
  if(RCinputs[1].fell()) {
    logMessage(F("RC Channel 1 pressed"));
    Wire.beginTransmission(9);
    Wire.write(0x01);
    Wire.endTransmission();
    winningSpin();
  }  
  if(RCinputs[2].fell()) {
    logMessage(F("RC Channel 2 pressed"));
    Wire.beginTransmission(9);
    Wire.write(0x02);
    Wire.endTransmission();
    mp3.play(6);
  }  
  if(RCinputs[3].fell()) {
    logMessage(F("RC Channel 3 pressed"));
    Wire.beginTransmission(9);
    Wire.write(0x13);
    Wire.endTransmission();
    CalibrateReels();
    mp3.play(6);
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
    //steppers[i].run();

    if(stepperActive[i]){
      steppers[i].runSpeed();
    }

    /*
    // The following line is only used to conserve power when the stepper motors are not running
    if(stepperWasRunning[i] && !steppers[i].isRunning()){
      digitalWrite(stepperEnablePins[i], HIGH);
    }
    */
    //stepperWasRunning[i] = steppers[i].isRunning();    
  }
}

/**
 * Nudge specified reel a certain number of positions
 */
void nudge(int reelNum, int positions){
  digitalWrite(stepperEnablePins[reelNum], LOW);
  steppers[reelNum].move(positions * STEPS_PER_VALUE);
}



/**
 * Spin all reels to an arbitrary position
 */
void randomSpin(){
  // Use playSL to loop music
  mp3.playSL(18);  
  // Assign a random, increasing target to each reel
  for(int i=0; i<numReels; i++) {
    digitalWrite(stepperEnablePins[i], LOW);
    // Set *relative* position for each target
    steppers[i].move(random(STEPS_PER_VALUE*(i+1)*12,STEPS_PER_VALUE*(i+2)*12));
  }
  bool allReelsStopped;
  do {
    allReelsStopped = true;
    for(int i=0; i<numReels; i++){
      steppers[i].run();
      if(steppers[i].isRunning()){
        allReelsStopped = false;
      }
    }
  } while (!allReelsStopped);

  mp3.play(21);  
}

/**
 * Spin all reels and stop on a whole number of revolutions from the zero point
 */
void winningSpin(){
  // Use playSL to loop music
  mp3.playSL(18);  
  for(int i=0; i<numReels; i++) {
    digitalWrite(stepperEnablePins[i], LOW);
    // Calculate *absolute* position that is next multiple of whole number of revolutions
    long target = roundUp(steppers[i].currentPosition(), NUM_STEPS);
    target += NUM_STEPS * ((i+1)*2);
    int winningSymbolPosition[] = {5, 3, 3, 3}; 
    target += winningSymbolPosition[i] * STEPS_PER_VALUE;
    // moveTo is absolute
    steppers[i].moveTo(target);
  }
  bool allReelsStopped;
  do {
    allReelsStopped = true;
    for(int i=0; i<numReels; i++){
      steppers[i].run();
      if(steppers[i].isRunning()){
        allReelsStopped = false;
      }
    }
  } while (!allReelsStopped);
  
  mp3.play(20);
}

void ResetReels() { 
  for(int i=0; i<numReels; i++) {
    digitalWrite(stepperEnablePins[i], LOW);
    // Calculate *absolute* position that is next multiple of whole number of revolutions
    long target = roundUp(steppers[i].currentPosition(), NUM_STEPS);
    // moveTo is absolute
    steppers[i].moveTo(target);
  }
  bool allReelsStopped;
  do {
    allReelsStopped = true;
    for(int i=0; i<numReels; i++){
      steppers[i].run();
      if(steppers[i].isRunning()){
        allReelsStopped = false;
      }
    }
  } while (!allReelsStopped);
}
