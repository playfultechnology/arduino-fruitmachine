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
// There are 9 chained digits
const byte numDigits = 9;

char valueToDisplay[numDigits] = {'s', 't', 'a', 'r', 't', 'n', 'o', 'b', 's'};

void CountUp() {
  // Find out the number of seconds passed since the code started
  unsigned long secondsElapsed = 123456789 + millis()/100;
  // Loop over each digit starting from the right-most
  for(int i=numDigits-1; i>=0; i--){
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

  clear();
  delay(2000);
 pattern1();
}

void clear() {
  // Set strobe pin low to begin storing bits 
  digitalWrite(strobePin, LOW);
  
  for(int i=0; i<numDigits-1; i++) {
    // Inverse logic so set all segments to 1 to turn them off
    shiftOut(dataPin, clockPin, MSBFIRST, ~0);  
  }
  // Set strobe pin high to stop storing bits
  digitalWrite(strobePin, HIGH);
}

void pattern1() {
  // This function essentially recreates the functionality of shiftOut() but at the bit level rather 
  // than 8-bit byte values https://forum.arduino.cc/t/shiftout-but-with-less-than-8-bits/17902/2
  for(int y=0; y<(numDigits*8)-1; y++){
    // Set strobe pin low to begin storing bits 
    digitalWrite(strobePin, LOW);
    // Send after padding, first!
    for(int i=0;i<numDigits*8-y-1;i++){
      digitalWrite(dataPin, HIGH);
      digitalWrite(clockPin, HIGH);
      digitalWrite(clockPin, LOW);
    }
    // Light one segment
    digitalWrite(dataPin, LOW);
    digitalWrite(clockPin, HIGH);
    digitalWrite(clockPin, LOW);
    // Send before padding after
    for(int z=0;z<y;z++){
      digitalWrite(dataPin, HIGH);
      digitalWrite(clockPin, HIGH);
      digitalWrite(clockPin, LOW);
    }
    // Set strobe pin high to stop storing bits
    digitalWrite(strobePin, HIGH);
    delay(50);
  }

/*
    for (int m=0; m<n; m++) {
      // Shift in the value to determine which segments of this digit will be lit
      shiftOut(dataPin, clockPin, MSBFIRST, 1);  
    }
  */  
  /*
    for(int i=0; i<8; i++){
      clear();
      // Set strobe pin low to begin storing bits 
      digitalWrite(strobePin, LOW);

      // Shift in the value to determine which segments of this digit will be lit
      shiftOut(dataPin, clockPin, MSBFIRST, ~1<<i);  

  if (bitOrder == LSBFIRST)
                  digitalWrite(dataPin, !!(val & (1 << i)));
            else      
                  digitalWrite(dataPin, !!(val & (1 << (7 - i))));
                  
            digitalWrite(clockPin, HIGH);
            digitalWrite(clockPin, LOW);  
      

      // Set strobe pin high to stop storing bits
      digitalWrite(strobePin, HIGH);
      delay(200);
    }
  }
  */
}


// Return the binary representation of the supplied character
byte getSegments(char c) {
  // Notice how we invert the bitmask
  return ~ASCIIto7Segment[c-32];
};

void loop() {

  CountUp();
  
  // Set strobe pin low to begin storing bits 
  digitalWrite(strobePin, LOW);

  // Each value gets shifted down by the next that gets sent, 
  // so we loop backwards over the array to send the last value first
  for (int i=numDigits-1; i>=0; i--) {
    // Shift in the value to determine which segments of this digit will be lit
    shiftOut(dataPin, clockPin, MSBFIRST, getSegments(valueToDisplay[i]));  
  }
  // Set strobe pin high to stop storing bits
  digitalWrite(strobePin, HIGH);
  delay(100);
}
