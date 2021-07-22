/*
See https://www.mikrocontroller.net/attachment/163201/Zaehlwerk_27-080_01__man.pdf
Also https://forum.arduino.cc/t/how-fast-is-shiftout/369057/2
Useful info here: https://arduino.stackexchange.com/questions/16348/how-do-you-use-spi-on-an-arduino
 */

// THE FOLLOWING CODE USES SHIFTIN/SHIFTOUT (doesn't work)  
// CONSTANTS
const byte resetPin = A5;
const byte dataOutPin = 11;
const byte dataInPin = 12;
const byte clockPin = 13;

// GLOBALS
// Array to hold received responses
byte response[8];

void setup() {
  Serial.begin(115200);

  pinMode(dataOutPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(resetPin, OUTPUT);
  pinMode(dataInPin, INPUT);

  // If desired, pull reset pin LOW to reset the counter
  digitalWrite(resetPin, LOW);
  delay(1000);
  digitalWrite(resetPin, HIGH);
  delay(8000);

  // Clear the response buffer
  memset(response, 0, sizeof(response));  

  // Set clock line to idle HIGH
  digitalWrite(clockPin, HIGH);

}

void printBuffer() {
  for(int i=0; i<8; i++) {
    Serial.print(response[i]);
    if(i<7) Serial.print(",");
  }
  Serial.println("");
}

void loop() {
  // Clear the response buffer
  memset(response, 0, sizeof(response));
  
  // Set clock line to idle HIGH
  // "the SEC unit uses SPI mode 2.  In this  mode, the clock line (SCLOCK) idle state is HIGH (logic 1)"
  digitalWrite(clockPin, HIGH);

  // Send a command - see "Command Format From Host" (p.10) for general structure
  // This is "t. <5C> Self Test" (p.19)
  customShiftOut(dataOutPin, clockPin, MSBFIRST, 0x5C); // CMD
  customShiftOut(dataOutPin, clockPin, MSBFIRST, 0x00); // ID starts at 00 and increments with each message sent
  customShiftOut(dataOutPin, clockPin, MSBFIRST, 0x00); // DCNT - number of databytes following (not including CHKSUM)
  customShiftOut(dataOutPin, clockPin, MSBFIRST, 0x5C); // CHECKSUM is lower 8 bits of sum of all preceding bytes

  // Then fill the buffer with 4 byte response
  for (int i=0; i<4; i++) {
    response[i] = customShiftIn(dataInPin, clockPin, MSBFIRST);
  }
  // Print the response
  printBuffer();

  delay(5000);  
}


// Shift functions adapted from the Arduino source code at https://github.com/arduino/ArduinoCore-avr/blob/master/cores/arduino/wiring_shift.c
// to accommodate data being clocked on the *falling* edge, and at a slower speed 
void customShiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val) {
  uint8_t i;
  for (i = 0; i < 8; i++)  {
    if (bitOrder == LSBFIRST)
      digitalWrite(dataPin, !!(val & (1 << i)));
    else  
      digitalWrite(dataPin, !!(val & (1 << (7 - i))));

    // 50% duty cycle @ 1KHz
    digitalWrite(clockPin, LOW); 
    delayMicroseconds(500);
    digitalWrite(clockPin, HIGH); 
    delayMicroseconds(500);   
  }
}
uint8_t customShiftIn(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder) {
  uint8_t value = 0;
  uint8_t i;
  for (i = 0; i < 8; ++i) {
    // 1KHz
    digitalWrite(clockPin, LOW);
    if (bitOrder == LSBFIRST)
      { value |= digitalRead(dataPin) << i; }
    else
      { value |= digitalRead(dataPin) << (7 - i); }

   delayMicroseconds(500);
   digitalWrite(clockPin, HIGH);
   delayMicroseconds(500);
  }
}




/* THE FOLLOWING CODE USES HARDWARE SPI (doesn't work) */
/*
// DEFINES
// Response code values
#define ACK 61
#define DATA 60
#define NAK 62

// CONSTANTS
const byte resetPin = A5;
const byte dataOutPin = 11;
const byte dataInPin = 12;
const byte clockPin = 13;

// INCLUDES
#include <SPI.h>

// GLOBALS
// Array to hold received responses
byte response[8];

void setup() {
  Serial.begin(115200);
  
  pinMode(clockPin, OUTPUT);
  pinMode(resetPin, OUTPUT);

  digitalWrite(resetPin, LOW);
  delay(1000);
  digitalWrite(resetPin, HIGH);
  delay(8000);

  // Clear the response buffer
  memset(response, 0, sizeof(response));  
 
  // Start the SPI interface
  SPI.begin();
  // This shouldn't be necessary because SPISettings should set speed anyway, but let's try it...
  SPI.setClockDivider(SPI_CLOCK_DIV128);

  // Set clock line to idle HIGH
  digitalWrite(clockPin, HIGH);

  // According to "Communications Interface" (p.8)
  // Uses SPI Mode 2, MSB, Clock speed between 100Hz - 5kHz
  SPI.beginTransaction(SPISettings(1000, MSBFIRST, SPI_MODE2));
  digitalWrite(clockPin, HIGH);
  // Send a command - see "Command Format From Host" (p.10) for general structure
  // This is "t. <5C> Self Test" (p.19)
  SPI.transfer(0x5C); // CMD
    digitalWrite(clockPin, HIGH);
  SPI.transfer(0x00); // ID starts at 00 and increments with each message sent
    digitalWrite(clockPin, HIGH);
  SPI.transfer(0x00); // DCNT - number of databytes following (not including CHKSUM)
    digitalWrite(clockPin, HIGH);
  SPI.transfer(0x5C); // CHECKSUM is lower 8 bits of sum of all preceding bytes
  digitalWrite(clockPin, HIGH);
  SPI.endTransaction();

  // Set clock line to idle HIGH
  digitalWrite(clockPin, HIGH);

  // Then fill it with 4 byte response
  for (int i=0; i<4; i++) {
    // Set clock line to idle HIGH
    digitalWrite(clockPin, HIGH);
    response[i] = SPI.transfer(0x00); // Dummy value just to receive SPI response
    // Set clock line to idle HIGH
    digitalWrite(clockPin, HIGH);
  }
  // Print the response
  printBuffer();
}

void printBuffer() {
  for(int i=0; i<8; i++) {
    Serial.print(response[i]);
    if(i<7) Serial.print(",");
  }
  Serial.println("");
}

void loop() {
}
*/
