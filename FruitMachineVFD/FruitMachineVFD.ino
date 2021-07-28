/*
* https://gutenber3d.wordpress.com/2015/02/15/control-de-vfd-vacuum-fluorescent-display-16f01ua3-samsumg-con-arduino-nano/
* http://cfile203.uf.daum.net/attach/2504384856450F340E422D
* https://github.com/DnaX/Samsung_16LF01_VFD
*/
/*
The module is controlled with only three input signals:
RST: Reset. Active low
DATA: input pin for data and commands
SCLK: Clock synchronism signal. Active on falling edge

From "4.8 Signal Interfacing" of 16LF01UA3 datasheet
1 VCC   2 VCC    +5V DC
3 N/P   4 N/C    No Pin, Not Connected
5 N/C   6 N/C    Not Connected
7 N/C   8 SCLK   Shift Clock (Falling Edge Active)
9 DATA 10 /RST   Data In line, Reset (Active Low)
11 GND 12 GND    Ground
*/


// INCLUDES
#include "src/Samsung_16LF01_VFD.h"

// CONSTANTS
//const byte SCLKpin = 7;
//const byte RSTpin = 6;
//const byte DATApin = 5;

// GLOBALS
// SCLK/DATA/RST
Samsung_16LF01_VFD vfd(13, 11, 8);
// The message to be scrolled
char message[] = "Clued Up! Norwich Casino Escape Room       ";
// The current value displayed on each digit of the display
char digits[17] = "                ";
int i;
int firstDigit = 0;
int lastDigit = strlen(message) - 1;

void setup() {
  /*Initialize the display (number of digits to use, brightness)*/
  vfd.begin (16,3);
  
  /*Test the screen with a welcome static message*/
  vfd.print("hello!!");
  delay(999);
  vfd.clear();

  /*Go to the first digit display*/
  vfd.setCursor(0);
}
void loop() {
  // Shuffle all the letters up
  for(i = 0; i < 15; i++) {
    digits[i] = digits[i + 1];
  }
  // Load the next letter on the end of the message
  digits[15] = message[firstDigit];

  if (firstDigit == lastDigit)
    firstDigit = 0;
  else
    firstDigit = firstDigit + 1;

  for (i = 0; i < 16; i++) {
    vfd.print(digits[i]);

    /*Workaround, "points" and "commas" are in the same digit*/
    if (digits[i] == ',' || digits[i] == '.')
      vfd.print(" ");
  }

  delay(250);
}
/*
//////////////////////////////////////////////////// ////////////////
// Functions to control the display
// See datasheet 16lF01UA3 samsumg and MSC1937-01 OKI semiconductor
//////////////////////////////////////////////////// ////////////////
void ResetDisplay () // Reset the display
{                             
  digitalWrite (RST, LOW); // RST low more than 1ms
  delay (2);		      
  digitalWrite (RST, HIGH); // RST high for
  delay (2); //Normal functioning
}
void WriteData (byte dat) {
  for (int i = 7; i> = 0; i--) {
    // data / byte-type commands. Writing becomes sequential
    // from most significant bit to least significant bit
    digitalWrite (DATA, bitRead (dat, i)); // Write the i bit of the dat byte
    delayMicroseconds (1);
    digitalWrite (SCLK, HIGH); // SCLK active on falling edge
    delayMicroseconds (1);
    digitalWrite (SCLK, LOW); // Falling edge: read the data / command
    delayMicroseconds (2);
    digitalWrite (SCLK, HIGH); // SCLK high:
    delayMicroseconds (1); // Wait for new data / commands
  }
}
*/






// https://www.fruitemu.co.uk/ib/index.php?app=forums&module=forums&controller=topic&id=17419
/*
// INCLUDES
#include <SPI.h>

void setup() {
  Serial.begin(115200);

  pinMode(13, OUTPUT);
  pinMode(11, OUTPUT);

  // 10 is RESET
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);
  delay(1000);
  digitalWrite(10, LOW);
  
  // Start the SPI interface
  SPI.begin();
  //SPI.setClockDivider(SPI_CLOCK_DIV128);
}

void sendCommand(byte command) {
  digitalWrite(13, HIGH);
  // Set SPI Mode
  // "I am pulsing the clock on the display at about 30-40ms." = 25Hz - 33Hz? 
  SPI.beginTransaction(SPISettings(4000, LSBFIRST, SPI_MODE0));  
  // "To send the bits out you place the relevant bit value on pin 2 of the port and then provide it a LO clock pulse . That is a HI-LO. "
  SPI.transfer(command); // Turn on display
  SPI.endTransaction();
    digitalWrite(13, HIGH);
}

void loop() {
 sendCommand(130); // Turn on screen
 sendCommand(172); // Test
 sendCommand(20); // Test
 delay(1000);
}
                              
// See https://picaxeforum.co.uk/threads/using-spi-serial-vfds-from-fruit-machines-with-picaxe-comprehensive-code-examples.22552/ 
/*
// INCLUDES
#include <SPI.h>

void setup() {
  Serial.begin(115200);

  pinMode(13, OUTPUT);
  pinMode(11, OUTPUT);

  // As the VFD will be the only thing on the bus for now, /SEL can remain low when idle.   
  pinMode(10, OUTPUT);
  digitalWrite(10, LOW);
  
  // Start the SPI interface
  SPI.begin();

  // Set SPI Mode
  SPI.beginTransaction(SPISettings(2000, LSBFIRST, SPI_MODE0));
}

void sendCommand(byte command) {
// See also https://forum.arduino.cc/t/spi-transactions-not-working-with-atmega1280/319194/2
  // "Before sending an command, it (SEL) must then be pulsed high."
  digitalWrite(10, HIGH);
  delayMicroseconds(10);
  digitalWrite(10, LOW);
  SPI.transfer(command); // Turn on display
}

void loop() {
 sendCommand(0x60); // Set display length
 sendCommand(0x57); // Set initial brightness to max
 sendCommand(0x70); // Turn on display

 sendCommand(0x10);
 SPI.transfer('B');

 delay(100);
}
*/
