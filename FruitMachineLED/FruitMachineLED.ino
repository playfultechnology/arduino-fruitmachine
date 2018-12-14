/**
 * 7-Segment Common Cathode LED display array 
 */

// LIBRARIES
#include "ASCIIto7Segment.h"

// CONSTANTS
// Pins connected to 74HC595 shift register
const int latchPin = 8;
const int clockPin = 12;
const int dataPin = 11;

// Return the binary representation of the supplied character
byte getSegments(char c) {
  return ASCIIto7Segment[c-32];
};

// Output pins go to the ULN2003 to ground the cathode of each digit
byte digitPins[5] = {A4, A3, A2, A1, A0};

// Define the values to display on each digit
// We'll define this as a char array rather than an integer number, to allow for
// display of arbitrary numbers, letters, or symbols 
char valueToDisplay[5] = {'s', 't', 'a', 'r', 't'};

// Keep track of timer for when to change the display
unsigned long lastTimeNumberSet;
unsigned long delayBetweenNumberChanging = 3000;

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
    "happy",
    "start",
    "tries",
    "right",
    "left ",
    "party"
    };
  // Choose a random word
  int chosenWordIndex = random(0,6);
  // Loop over each display
  for(int i=0; i<5; i++){
    // Look up the binary representation of the corresponding letter in the chosen word 
    valueToDisplay[i] = (char)words[chosenWordIndex][i];
  }
}

void setup() {
  // Define pin modes
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);  
  pinMode(clockPin, OUTPUT);
  for(int i=0; i<5; i++){
    pinMode(digitPins[i], OUTPUT);
  }
}


void loop() {

  unsigned long now = millis();
  if(now - lastTimeNumberSet > delayBetweenNumberChanging) {
    // Display a random number
    //setRandomNumber();
    // Display a random word
    setRandomWord();
    lastTimeNumberSet = now;
  }

  for(int i=4;i>=0;i--){

    // Deactivate the last digit before updating
    digitalWrite(digitPins[(i+1)%5], LOW);

    // Hold the latch pin low
    digitalWrite(latchPin, LOW);
    // Shift the new value in
    shiftOut(dataPin, clockPin, MSBFIRST, getSegments(valueToDisplay[i]));
    // Release the latch
    digitalWrite(latchPin, HIGH);

    // Re-activate the digit again
    digitalWrite(digitPins[i], HIGH);

    // Set this as large as possible before flickering occurs
    delay(3);
  }
}
