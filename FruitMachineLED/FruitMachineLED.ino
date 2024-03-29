/**
 * 7-Segment LED display array 
 * 
 * This code uses two daisy-chained shift registers to control an array of (up to 8) multiplexed 7-segment displays: 
 * - one "high side" (e.g. MIC5981 or 74HC595+UDN2981) which supplies +ve voltage to the anodes 
 * - one "low side" (e.g. TPIC6B595 or 74HC595+ULN2803) which grounds the cathodes
 * 
 * With minor modifications to the code, this setup will work with either common anode or common cathode displays
 * (in common anode, the segments to be displayed are determined by the low side, in common cathode, segments to be displayed
 * are determined by the high side). Resistors should be added to each segment at that same side, according to (V-Vf)/If  
 */

// DEFINES
#define ArrayLength(x) (sizeof(x) / sizeof((x)[0]))

// LIBRARIES
#include "ASCIIto7Segment.h"

// CONSTANTS
const byte numDigits = 4;
// Pins to control shift register(s)
const int latchPin = 3; // (Storage Register Clock Input/STCP/RCLK)
const int clockPin = 2; // (Shift Register Clock Input/SHCP/SRCLK)
const int dataPin = 4; // (Din/SER)
// Note that OE needs to be pulled LOW, and SCLR/RST needs to be pulled HIGH

// Return the binary representation of the supplied character
byte getSegments(char c) {
  return ASCIIto7Segment[c-32];
};

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
byte segmentsToDisplay[numDigits] = {};

// Keep track of timer for when to change the display
unsigned long lastTimeNumberSet;
unsigned long delayBetweenValueChanging = 3000;

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

void setup() {

  Serial.begin(115200);
  Serial.println(__FILE__ __DATE__);
  
  // Define pin modes
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);  
  pinMode(clockPin, OUTPUT);
}

void loop() {

  // Decide what value to write on the display by calling the appropriate function to set "valueToDisplay"
  /*
  // To periodically set a new value, first check the current timestamp
  unsigned long now = millis();
  // If sufficient time has elapsed since last time the value was changed
  if(now - lastTimeNumberSet > delayBetweenValueChanging) {
    // Display a random number
    setRandomNumber();
    // Record the current time
    lastTimeNumberSet = now;
  }
  */
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
  //delay(200);
}
