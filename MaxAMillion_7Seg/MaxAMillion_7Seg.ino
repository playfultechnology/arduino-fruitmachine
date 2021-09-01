/**
 * Arduino "Max-A-Million" 7-Segment LED Controller
 * Copyright (c) 2021 Playful Technology
 */
// DEFINES
#define ArrayLength(x) (sizeof(x) / sizeof((x)[0]))

// INCLUDES
// I2C connection to 7-segment daughterboard
#include <Wire.h>
#include "src/ASCIIto7Segment.h"

// CONSTANTS
const byte slave7SegmentAddress = 9;
const byte numDigits = 4;
// Pins to control shift register(s)
const int latchPin = 3; // (Storage Register Clock Input/STCP/RCLK)
const int clockPin = 2; // (Shift Register Clock Input/SHCP/SRCLK)
const int dataPin = 4; // (Din/SER)
// Note that OE needs to be pulled LOW, and SCLR/RST needs to be pulled HIGH


// GLOBALS
// Define the segments that should be lit on each digit. 
// The 7 segments of a single digit (+1 d.p.) can be represented as a single byte, with
// the 8 bits representing the segments .GFEDCBA
//  --a--
// |     |
// f     b
// |     |
//  --g--
// |     |
// e     c
// |     |
//  --d--
int timeValue = 3600; // Assume 60 minute start time
byte segmentsToDisplay[numDigits] = {};
// Keep track of timer for when to change the display
unsigned long lastTimeNumberSet;
unsigned long delayBetweenValueChanging = 3000;
enum Mode { OFF, FIXED, COUNTING_DOWN };
Mode mode = Mode::FIXED;

// Initial setup
void setup() {
  // Start a serial connection
  Serial.begin(115200);
  // Don't wait for ages for serial input
  Serial.setTimeout(50); 
  Serial.println(__FILE__ __DATE__);

  // Join I2C bus
  Wire.begin(slave7SegmentAddress); 
  Wire.onReceive(receiveEvent);
  
  // Define pin modes
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);  
  pinMode(clockPin, OUTPUT);

  pinMode(LED_BUILTIN, OUTPUT);
  
}

// Assign a random 5-digit number to the display
void setRandomNumber() {
  for(int i=0; i<numDigits; i++){
    // Digits 0-9 correspond to ASCII codes 48-57
    segmentsToDisplay[i] = getSegments((char)(random(48,58)));
  }
}

void CountUp() {
  // Find out the number of seconds passed since the code started
  unsigned long secondsElapsed = millis()/100;
  // Loop over each display
  for(int i=numDigits-1; i>=0; i--){
    // Get the value of the current "unit" (ones, tens, hundreds etc.)
    uint8_t digit = secondsElapsed % 10;
    // Look up the binary representation of the corresponding digit 
    segmentsToDisplay[i] = getSegments((char)digit+48);
    // Divide by 10 to move onto the next "unit"
    secondsElapsed /= 10;
  }
}

// Return the binary representation of the supplied character
byte getSegments(char c) {
  return ASCIIto7Segment[c-32];
};

void receiveEvent(int howMany) {
  Serial.print(F("Message Received!"));
 for (int i=0; i<howMany; i++) {
    byte c = Wire.read();
    if(c = 0x00){
      mode = Mode::FIXED;
    }
    if(c == 0x01){
      mode = Mode::COUNTING_DOWN;
    }
  }  // end of for loop
  Serial.println(F("Receive Event"));
}


void loop() {


  if(mode == Mode::FIXED) { 
    digitalWrite(LED_BUILTIN, LOW);
  }
  else {
    digitalWrite(LED_BUILTIN, HIGH);
  }
  
    // To continuously set a new value, e.g. counting up every second
  CountUp();

  // Loop over every digit
  for(int i=0; i<numDigits; i++){
    for(int segment=0; segment<8; segment++){
    // Hold the latch pin low
    digitalWrite(latchPin, LOW);
    // Then shift in the value of which segment anodes will be lit by the UDN2981
    shiftOut(dataPin, clockPin, MSBFIRST, 1<<i);    
    // First, shift in the value that determines which display cathodes will be grounded by the ULN2803
    shiftOut(dataPin, clockPin, MSBFIRST, segmentsToDisplay[i]);
    // Release the latch
    digitalWrite(latchPin, HIGH);
    // Set this as large as possible before flickering occurs
    delayMicroseconds(100);
    }
  }
}
