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
// Pins to control shift register(s)
const int latchPin = 10; // (Storage Register Clock Input/STCP/RCLK)
const int clockPin = 13; // (Shift Register Clock Input/SHCP/SRCLK)
const int dataPin = 11; // (Din/SER)
// Note that OE needs to be pulled LOW, and SCLR/RST needs to be pulled HIGH

// Return the binary representation of the supplied character
byte getSegments(char c) {
  return ASCIIto7Segment[c-32];
};

// Define the values to display on each digit
// We'll define this as a char array rather than an integer number, to allow for
// display of arbitrary numbers, letters, or symbols 
char valueToDisplay[5] = {'s', 't', 'a', 'r', 't'};

// Keep track of timer for when to change the display
unsigned long lastTimeNumberSet;
unsigned long delayBetweenValueChanging = 3000;

// Assign a random 5-digit number to the display
void setRandomNumber() {
  for(int i=0; i<5; i++){
    // Digits 0-9 correspond to ASCII codes 48-57
    valueToDisplay[i] = (char)(random(48,58));
  }
}

// Choose a random word from a list to display
void setRandomWord() {
  // Array of words that can be spelt on a 7-segment display
  char words[][5] = {
    "Start",
    "Play ",
    "Stop ",
    };
  // Choose a random word
  int chosenWordIndex = random(0,ArrayLength(words));
  // Loop over each display
  for(int i=0; i<5; i++){
    // Look up the binary representation of the corresponding letter in the chosen word 
    valueToDisplay[i] = (char)words[chosenWordIndex][i];
  }
}

void CountUp() {
  // Find out the number of seconds passed since the code started
  unsigned long secondsElapsed = millis()/1000;
  // Loop over each display
  for(int i=4; i>=0; i--){
    // Get the value of the current "unit" (ones, tens, hundreds etc.)
    uint8_t digit = secondsElapsed % 10;
    // Look up the binary representation of the corresponding digit 
    valueToDisplay[i] = (char)digit+48;
    // Divide by 10 to move onto the next "unit"
    secondsElapsed /= 10;
  }
}

void setup() {
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
    // setRandomNumber();
    // Display a random word
       setRandomWord();
    // Record the current time
    lastTimeNumberSet = now;
  }
  */
  // To continuously set a new value, e.g. counting up every second
  // CountUp();

  // Loop over every digit
  for(int i=0; i<4; i++){
    // Hold the latch pin low
    digitalWrite(latchPin, LOW);
    // First, shift in the value that determines which display cathodes will be grounded by the ULN2803
    shiftOut(dataPin, clockPin, MSBFIRST, getSegments(valueToDisplay[i]));    
    //shiftOut(dataPin, clockPin, MSBFIRST, getSegments(valueToDisplay[i]));
    // Then shift in the value of which segment anodes will be lit by the UDN2981
    shiftOut(dataPin, clockPin, MSBFIRST, 1<<i);
    // Release the latch
    digitalWrite(latchPin, HIGH);
    // Set this as large as possible before flickering occurs
    delay(5);
  }
}
