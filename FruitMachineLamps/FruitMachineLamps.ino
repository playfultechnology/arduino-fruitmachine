// DEFINES
#define ArrayLength(x) (sizeof(x) / sizeof((x)[0]))

// INCLUDES
#include <Wire.h>

// CONSTANTS
// Good explanation here: https://lastminuteengineers.com/74hc595-shift-register-arduino-tutorial/
// Strobe/Latch/RCLK (also sometimes confusingly called "Clock", as in "clocking in") (pin 1) of TPIC6B595
int strobePin = 10;
// Ser/Data/SerIn/MOSI (pin 2) of TPIC6B595
int dataPin = 11;
// Clock/SRCLK Pin connected to Clock (pin 3) of TPIC6B595
int clockPin = 13;

const byte numAnodes = 8;// 8;
const byte numCathodes = 14; //14;

// The predefined light patterns to toggle between
// If, rather than using predefined patterns you'd like to dynamically turn on or off outputs from the shift register,
// look at bitSet(), bitClear(), bitWrite()
const byte patterns[] = {B00111111, B10000110, B01011011, B11001111, B01100110, B11101101, B11111101, B00000111, B01111111, B11101111, B01110111, B01111100, B10111001, B01011110, B11111001, B01110001, B01001001}; 

// Each shift register holds a single byte
byte getBytePattern(int i){
  byte b = 1<<i;
  // byte b = i;
  return b; 
}

// If we have four daisy-chained shift registers, can represent their 32bits in a single long
unsigned long getLongPattern(int i) {
  unsigned long l;
  // Shift a 1 to the ith bit
  // unsigned long l = (unsigned long)1L<<i;
  // Or, use bitset to set an individual bit (count from right)
  bitSet(l, i);
  return l;
}

// GLOBALS
int i = 0;

void setup() {
  Serial.begin(115200);
  Serial.println(__FILE__ __DATE__);
  
  //set pins to output because they are addressed in the main loop
  pinMode(strobePin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  // If OE is not tied to GND, then you can manually enable the shift register by writing a LOW value to its pin
  // pinMode(oePin, OUTPUT);
  // digitalWrite(oePin, HIGH);
}


// See https://forum.arduino.cc/t/find-nearest-multiple-of-8-from-a-variable/255420/16
int numRegisters(int x){
  return (x + 7) >> 3;
}

void prntBits(byte b)
{
  for(int i = 7; i >= 0; i--)
    Serial.print(bitRead(b,i));
  Serial.println();  
}

void loop() {

  byte numAnodeRegisters = numRegisters(numAnodes);
  byte numCathodeRegisters = numRegisters(numCathodes);

  for(uint8_t a=0;a<numAnodes;a++){
    for(uint8_t b=0;b<numCathodes;b++){

      unsigned long l = (unsigned long)1L<<b;

      // Set strobe pin low to begin storing bits  
      digitalWrite(strobePin, LOW);
      
      // Shift in the values for each register - one byte per register, and last first
      shiftOut(dataPin, clockPin, MSBFIRST, (byte)(1<<a));
      
      // High 8 bits
      shiftOut(dataPin, clockPin, MSBFIRST, (l>>8));

      // Low 8 bits
      shiftOut(dataPin, clockPin, MSBFIRST, (byte)l);

      // Set strobe pin high to stop storing bits
      digitalWrite(strobePin, HIGH);
      delay(250);

      Serial.print(a);
      Serial.print(",");
      Serial.print(b);
      Serial.println("");
      prntBits(1<<a);
      prntBits(l>>8);
      prntBits(l);//&0x00FF);
      Serial.println("---");
      
    }
  }
/*
  // Set strobe pin low to begin storing bits  
  digitalWrite(strobePin, LOW);
  
  // Then shift in the values for each register - one byte per register, and last first
  shiftOut(dataPin, clockPin, MSBFIRST, getBytePattern(i));
  shiftOut(dataPin, clockPin, MSBFIRST, getBytePattern(i));
  shiftOut(dataPin, clockPin, MSBFIRST, 255);
  shiftOut(dataPin, clockPin, MSBFIRST, 255);


/*
  for(int x = 31; x >= 0; x--)
    Serial.print(bitRead(getLongPattern(i),x));
  Serial.println();  
  */
  /*
  // Set strobe pin high to stop storing bits
  digitalWrite(strobePin, HIGH);
  delay(250);

  i++;
  if(i>31) {
    i=0;
  }
  */
}
