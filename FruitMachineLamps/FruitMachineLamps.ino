// DEFINES
#define ArrayLength(x) (sizeof(x) / sizeof((x)[0]))

// LIBRARIES
//#include "ASCIIto7Segment.h"

// CONSTANTS
//Pin connected to Strobe (Latch) (pin 1) of TPIC6B595
int strobePin = 11;
//Pin connected to Data (pin 2) of TPIC6B595
int dataPin = A5;
//Pin connected to Clock (pin 3) of TPIC6B595
int clockPin = 5;
const byte numDisplays = 1;

//char valueToDisplay[numDisplays] = {'0', '0', '0', ' ', ' ', ' ', ' ', ' ', ' '};

const byte patterns[] = {B00111111, B10000110, B01011011, B11001111, B01100110, B11101101, B11111101, B00000111, B01111111, B11101111, B01110111, B01111100, B10111001, B01011110, B11111001, B01110001, B01001001}; 


void setup() {
  //set pins to output because they are addressed in the main loop
  pinMode(strobePin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
 // pinMode(oePin, OUTPUT);
 // digitalWrite(oePin, HIGH);
}

bool flip;

void loop() {

  byte pattern = flip ? patterns[0] : ~patterns[0];
  flip = !flip;
  
  digitalWrite(strobePin, LOW);
  //set strobe pin low to begin storing bits
  // Then shift in the value of which segment anodes will be lit
  shiftOut(dataPin, clockPin, LSBFIRST, pattern);  
  //set strobe pin high to stop storing bits
  digitalWrite(strobePin, HIGH);
  delay(1000);
}
