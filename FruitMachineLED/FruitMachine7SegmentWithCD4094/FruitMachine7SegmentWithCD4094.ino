// DEFINES
#define ArrayLength(x) (sizeof(x) / sizeof((x)[0]))

// LIBRARIES
#include "ASCIIto7Segment.h"

// CONSTANTS
//Pin connected to Strobe (pin 1) of 4094
int strobePin = 10;
//Pin connected to Data (pin 2) of 4094
int dataPin = 11;
//Pin connected to Clock (pin 3) of 4094
int clockPin = 13;
const byte numDisplays = 9;


char valueToDisplay[numDisplays] = {'s', 't', 'a', ' ', ' ', ' ', ' ', ' ', ' '};

void CountUp() {
  // Find out the number of seconds passed since the code started
  unsigned long secondsElapsed = millis()/100;
  // Loop over each display
  for(int i=0; i<=numDisplays; i++){
    // Get the value of the current "unit" (ones, tens, hundreds etc.)
    uint8_t digit = secondsElapsed % 10;
    // Look up the binary representation of the corresponding digit 
    valueToDisplay[i] = (char)digit+48;
    // Divide by 10 to move onto the next "unit"
    secondsElapsed /= 10;
  }
}

void setup() {
  //set pins to output because they are addressed in the main loop
  pinMode(strobePin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
 // pinMode(oePin, OUTPUT);
 // digitalWrite(oePin, HIGH);
}

// Return the binary representation of the supplied character
byte getSegments(char c) {
  // Notice how we invert the bitmask
  return ~ASCIIto7Segment[c-32];
};

void loop() {

  CountUp();

  digitalWrite(strobePin, LOW);
  //count up routine
  for (int i = 0; i < numDisplays; i++) {
    //set strobe pin low to begin storing bits 
  //  shiftOut(dataPin, clockPin, MSBFIRST,  1<<-i);
    // Then shift in the value of which segment anodes will be lit
    shiftOut(dataPin, clockPin, MSBFIRST, getSegments(valueToDisplay[i]));  
    }
    //set strobe pin high to stop storing bits
    digitalWrite(strobePin, HIGH);
    delay(100);
  
}
