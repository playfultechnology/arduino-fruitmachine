/**
 * Arduino Fruit Machine Reel Controller
 * Copyright (c) 2021 Playful Technology
 */
// DEFINES
// Helper function to get length of an arbitrary array
#define ArrayLength(x) (sizeof(x) / sizeof((x)[0]))

// The total number of steps required for one complete revolution of the reel motor
// Common values for motors are: 
// - 7.5° step angle = 48 steps
// - 1.8° step angle = 200 steps
#define NUM_STEPS 48

// STEPS_PER_VALUE is the number of steps between adjacent symbols on the reel
// Calculated as NUM_STEPS / (Number of Symbols)
// For 16 values on a 7.5° step angle reel (48 steps), STEPS_PER_VALUE 3 etc.
// For 12 values on a 7.5° step angle reel (48 steps), STEPS_PER_VALUE 4 etc.
// For 20 values on a  1.8° step angle reel (200 steps), STEPS_PER_VALUE 10 etc.
#define STEPS_PER_VALUE 4

// What sort of shield is being used to wire the components to the Arduino? 
// #define USE_CNCSHIELD (UNO) or #define USE_RAMPS (Mega)
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
  #define PS_ON_PIN          12
  //#define KILL_PIN           -1
  // Note Pin D8 is wired to the separate 11A power supply and does not share any power with the rest of the board
  #define HEATER_1_PIN       8
  // Pins D9 and D10 share the main 5A power supply. They are "common anode", in that the +ve terminals are connected 
  // together, and are switched on only by grounding the separate -ve terminals.
  #define FAN_PIN            9
  #define HEATER_0_PIN       10  
  #define TEMP_0_PIN         13   // ANALOG NUMBERING
  #define TEMP_1_PIN         14   // ANALOG NUMBERING
  #define KILL_PIN 41 //[RAMPS14-SMART-ADAPTER]  
  #define LCD_PINS_RS 16 //[RAMPS14-SMART-ADAPTER]  
  #define LCD_PINS_ENABLE 17 //[RAMPS14-SMART-ADAPTER]  
  #define LCD_PINS_D4 23 //[RAMPS14-SMART-ADAPTER]  
  #define LCD_PINS_D5 25 //[RAMPS14-SMART-ADAPTER]  
  #define LCD_PINS_D6 27 //[RAMPS14-SMART-ADAPTER]  
  #define LCD_PINS_D7 29 //[RAMPS14-SMART-ADAPTER]  
  #define BTN_EN1 31 //[RAMPS14-SMART-ADAPTER]  
  #define BTN_EN2 33 //[RAMPS14-SMART-ADAPTER]  
  #define BTN_ENC 35 //[RAMPS14-SMART-ADAPTER]  
  #define BEEPER 37 //[RAMPS14-SMART-ADAPTER] / 37 = enabled; -1 = disabled / (if you don't like the beep sound ;-)
  #define SDCARDDETECT 49 //[RAMPS14-SMART-ADAPTER]  
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
const byte numReels = 1;

#ifdef USE_RAMPS
  // Use the X/Y min/max end stop pins to wire the opto sensors
  const byte sensorPins[4] = {X_MIN_PIN, X_MAX_PIN, Y_MIN_PIN, Y_MAX_PIN};
#elif defined(USE_CNCSHIELD)
  // The CNC shield only supports 3 endstops, so if we have >3 reels need to use another pin for the extra optosensors 
  const byte sensorPins[4] = {X_MIN_PIN, Y_MIN_PIN, Z_MIN_PIN, HOLD_PIN};
#endif

// INCLUDES
// See: http://www.airspayce.com/mikem/arduino/AccelStepper/
#include <AccelStepper.h>
// https://github.com/thomasfredericks/Bounce2
#include <Bounce2.h>
// 20x04 character LCD display
#include <LiquidCrystal.h>
// Rotary encoder
#include <Encoder.h>

// GLOBALS
// Define each stepper and the pins it will use
AccelStepper stepperX(AccelStepper::DRIVER, X_STEP_PIN, X_DIR_PIN);
AccelStepper stepperY(AccelStepper::DRIVER, Y_STEP_PIN, Y_DIR_PIN);
AccelStepper stepperZ(AccelStepper::DRIVER, Z_STEP_PIN, Z_DIR_PIN);
AccelStepper stepperE(AccelStepper::DRIVER, E_STEP_PIN, E_DIR_PIN);
//AccelStepper stepperQ(AccelStepper::DRIVER, Q_STEP_PIN, Q_DIR_PIN);
AccelStepper steppers[4] = {stepperX, stepperY, stepperZ, stepperE};
// Array of Bounce objects to help read each time a reel tab spins past the opto-sensor
Bounce optoSensors[4] = {};
// Initialize the LCD with specified interface pins
#ifdef USE_RAMPS
  LiquidCrystal lcd(LCD_PINS_RS, LCD_PINS_ENABLE, LCD_PINS_D4, LCD_PINS_D5, LCD_PINS_D6, LCD_PINS_D7);
#endif
// Create a char array to store the contents of the LCD screen
char displayBuffer[4][20];
// Encoder object
#ifdef USE_RAMPS
  Encoder encoder(BTN_EN2, BTN_EN1);
  long lastEncoderPosition  = -999;
  long currentEncoderPosition;
  // encButton is when encoder button (LS1) is pushed in
  Bounce2::Button encButton = Bounce2::Button();
  // button is the small black button (K1)
  Bounce2::Button button = Bounce2::Button();
#endif

/**
 * Copies the current contents of the display buffer to the LCD
 */
void updateDisplay() {
  #ifdef USE_RAMPS
  for(int i=0; i<4; i++) {
    lcd.setCursor(0,i);
    lcd.print(displayBuffer[i]);
  }
  #endif
}

/**
 * Displays a string on LCD screen and via serial connection
 */
void logMessage(char* message){
  for(int i=0; i<3; i++){
    strncpy(displayBuffer[i], displayBuffer[i+1], sizeof(displayBuffer[i]));
  }
  // Note use of right-padding to fill whole line
  snprintf(displayBuffer[3], sizeof(displayBuffer[3]), "%-20s", message);
  Serial.println(message);
  updateDisplay();
}

/**
 * Displays a PROGMEM string on LCD screen and via serial connection
 */
void logMessage(const __FlashStringHelper* message){
  for(int i=0; i<3; i++){
    strncpy(displayBuffer[i], displayBuffer[i+1], sizeof(displayBuffer[i]));
  }
  // Note use of right-padding to fill whole line
  snprintf_P(displayBuffer[3], sizeof(displayBuffer[3]), PSTR("%-20S") , message);
  Serial.println(message);
  updateDisplay();
}

// Initial setup
void setup() {
  // Start a serial connection
  Serial.begin(115200);
  // Don't wait for ages for serial input
  Serial.setTimeout(50); 
  Serial.println(__FILE__ __DATE__);

  Serial.println(F("Send 999 to calibrate"));
  Serial.println(F("Send 1/2/3/4 to nudge reels up"));
  Serial.println(F("Send 5/6/7/8 to nudge reels down"));  
  Serial.println(F("Send 200 for random spin"));  
  Serial.println(F("Send 300 for winning spin"));

  // Start the LCD display with specified columns and rows
  #ifdef USE_RAMPS
  lcd.begin(20, 4);
  #endif
  
  // Print a message to the LCD
  sprintf(displayBuffer[0], "%-16s %3s", "Fruit Machine", "1.0"); 
  updateDisplay();
   
  // Initialise the buttons
    #ifdef USE_RAMPS
  encButton.attach(BTN_ENC, INPUT_PULLUP);
  button.attach(KILL_PIN, INPUT_PULLUP);
  #endif

  // Note: if using stepper drivers (A4988 etc.), these need to be enabled by pulling the EN pin low. 
  // On some shields, the pin is automatically pulled to GND via a jumper. If not, write a LOW signal to 
  // whatever pin is connected to the enable pin of the chip. 
  pinMode(X_ENABLE_PIN, OUTPUT);  digitalWrite(X_ENABLE_PIN, LOW);
  pinMode(Y_ENABLE_PIN, OUTPUT);  digitalWrite(Y_ENABLE_PIN, LOW);
  pinMode(Z_ENABLE_PIN, OUTPUT);  digitalWrite(Z_ENABLE_PIN, LOW);
  pinMode(E_ENABLE_PIN, OUTPUT);  digitalWrite(E_ENABLE_PIN, LOW);
  
  // Set the optosensor pins as inputs and attach debounce objects to them
  for(int i=0; i<numReels; i++) {
    optoSensors[i].attach(sensorPins[i], INPUT_PULLUP);
  }
  
  // Stepper motor speed (steps/sec) and acceleration (steps per second^2)
  // These values need to be tweaked by experimentation!
  for(int i=0;i<numReels;i++){
    // AccelStepper speed is express in steps/sec
    // So, for a reel with NUM_STEPS 48, setMaxSpeed(60), means (60*60/48)= 75RPM
    steppers[i].setMaxSpeed(60);
    steppers[i].setAcceleration(150);
  }
  /*
  logMessage(F("Testing beeper"));
  pinMode(BEEPER, OUTPUT);
  digitalWrite(BEEPER, HIGH);
  delay(100);
  digitalWrite(BEEPER, LOW);
  */

  // Calibrate reels to determine "zero" point
  logMessage(F("Calibrating Reels"));    
  CalibrateReels();
  
  logMessage(F("Setup Complete"));  
}

/**
 * Returns modulo, and deals correctly with negative numbers (unlike builtin %)
 */
int mod(int x, int y ){
  return x<0 ? ((x+1)%y)+y-1 : x%y;
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
    
    // Now that calibration is complete, restore the normal max motor speed
    steppers[i].setMaxSpeed(maxSpeed);
    delay(500);

    Serial.println(F("Stepper calibrated"));
  }
  Serial.println(F("Steppers calibrated"));
}

// See https://forum.arduino.cc/t/rounding-an-integer-up-only/350784/3
long roundUp(long val, long factor) {
  long rem = val % factor;
  val -= rem;
  if (rem > 0) val += factor;
  return val;
}

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
void targetSpin(){
  for(int i=0; i<numReels; i++) {
    // Calculate *absolute* position that is next multiple of whole number of revolutions
    long target = roundUp(steppers[i].currentPosition(), NUM_STEPS);
    target += NUM_STEPS * ((i+1)*2);
    // moveTo is absolute
    steppers[i].moveTo(target);
  }
}

void loop() {

  // INPUTS
  for(int i=0; i<numReels; i++) {
    optoSensors[i].update();
  }  
  #ifdef USE_RAMPS
    button.update();
    encButton.update();
  // Read rotary encoder input
  currentEncoderPosition = encoder.read();
  // If it's changed, output new value
  if (currentEncoderPosition != lastEncoderPosition) {
    lastEncoderPosition = currentEncoderPosition;
    sprintf(displayBuffer[1], "Encoder: %-7d", currentEncoderPosition>>2); 
    updateDisplay();
  }
  if(encButton.pressed()) {
    logMessage(F("encButton Pressed"));
    targetSpin();
  }
  if(button.pressed()) {
    logMessage(F("button Pressed"));
    randomSpin();
    /*
    digitalWrite(BEEPER, HIGH);
    delay(100);
    digitalWrite(BEEPER, LOW);
    */
  }
  #endif
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
      targetSpin();
    }
  }
  // This gets called every frame, and processes any movement of the steppers as necessary
  for(int i=0; i<numReels; i++){
    steppers[i].run();
  }
}
