/*
* https://gutenber3d.wordpress.com/2015/02/15/control-de-vfd-vacuum-fluorescent-display-16f01ua3-samsumg-con-arduino-nano/
* http://cfile203.uf.daum.net/attach/2504384856450F340E422D
* https://github.com/DnaX/Samsung_16LF01_VFD


* "Snakes and Ladders" has a 31-821-001 ISS 1 Bell Fruit 16-character display
* It uses an NEC F780204GF-045 chip https://pdf1.alldatasheet.net/datasheet-pdf/view/125501/NEC/D780206GF.html
* https://www.fruitmachinesales.co.uk/bell-fruit-16-character-alphanumeric-display
* 
* https://www.fruit-emu.com/forums/topic/6080-eclipse-cabs-bellfruit/
* https://www.fruitemu.co.uk/ib/index.php?app=forums&module=forums&controller=topic&id=17419
* states The display has a 6 pin connector :
1 - +12v
2 - Clock
3 - NC
4 - Data
5 - Reset
6 - Ground

Tuppenny Nudger had a Samsung 16L102DA4 board with an OKI M9202-04 chip

"Max-a-Million" has a Samsung 16LF01UA3 Rev D board
Datasheet http://cfile203.uf.daum.net/attach/2504384856450F340E422D
It also has a library available! https://github.com/DnaX/Samsung_16LF01_VFD

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


#ifdef Display_16LF01UA3

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
#endif


#ifdef Display_31-821-001

const byte DATA = 11;
const byte SCLK = 13;
const byte RST = 8;

//////////////////////////////////////////////////// ////////////////
// Functions to control the display
// See datasheet 16lF01UA3 samsumg and MSC1937-01 OKI semiconductor
//////////////////////////////////////////////////// ////////////////
/*
void ResetDisplay () // Reset the display
{                             
  digitalWrite (RST, LOW); // RST low more than 1ms
  delay (2);          
  digitalWrite (RST, HIGH); // RST high for
  delay (2); //Normal functioning
}
*/

// "To send the bits out you place the relevant bit value on pin 2 of the port and then provide it a LO clock pulse . That is a HI-LO. "
void WriteData (byte dat) {
  for (int i=7; i >=0; i--) {
    digitalWrite (SCLK, HIGH); // SCLK high:
    delay (20); // Wait for new data / commands
    digitalWrite (DATA, bitRead (dat, i)); // Write the i bit of the dat byte
    delay (20);
    digitalWrite (SCLK, LOW); // SCLK active on falling edge
    delay(20);
    digitalWrite (SCLK, HIGH); // SCLK high:
    /*
    digitalWrite (DATA, bitRead (dat, i)); // Write the i bit of the dat byte
    digitalWrite (SCLK, HIGH); // SCLK high:
    delay (30); // Wait for new data / commands
    digitalWrite (SCLK, LOW); // SCLK active on falling edge
    delay(30);
    digitalWrite(SCLK, HIGH);
    */
  }
}

// Following instructions at https://www.fruitemu.co.uk/ib/index.php?app=forums&module=forums&controller=topic&id=17419
void setup() {
  Serial.begin(115200);
  Serial.println(__FILE__ __DATE__);

  pinMode(SCLK, OUTPUT);
  digitalWrite (SCLK, LOW);
  pinMode(DATA, OUTPUT);
  pinMode(RST, OUTPUT);

  // "To reset the display you need to supply it with a +5v logic level for a minimum of 30ms."
  //digitalWrite(RST, HIGH);
  //delay(1000);
  //digitalWrite(RST, LOW);
  //delay(3000);


  // start printing at first character
  WriteData(144);

  // Scroll To Left
  WriteData(162);

  WriteData('T');
  WriteData('H');
  WriteData('I');
  WriteData('S');
  WriteData(' ');
  WriteData('I');
  WriteData('S');
  WriteData(' ');
  WriteData('G');
  WriteData('O');
  WriteData('O');
  WriteData('D');
  WriteData(' ');
  WriteData('I');
  WriteData('F');
  WriteData(' ');
  WriteData('I');
  WriteData('T');
  WriteData(' ');
  WriteData('W');
  WriteData('O');
  WriteData('R');
  WriteData('K');
  WriteData('S');

delay(1000);



  delay(3000);

  // Clear Screen
  WriteData(177);



    
/*
  for(byte i=65;i<96;i++){
  WriteData(i);
  //delay(50);
}
*/
  /*
  WriteData(10);
  WriteData(64);
  WriteData(172);
  */

  
}

void loop() {
  /*
  for(byte i=0; i<25; i++) {
    WriteData(i);
    delay(100);
  }
  */
}


#endif

/*
// https://www.fruitemu.co.uk/ib/index.php?app=forums&module=forums&controller=topic&id=17419

// INCLUDES
#include <SPI.h>

void setup() {
  Serial.begin(115200);
  Serial.println(__FILE__ __DATE__);
  

  pinMode(13, OUTPUT);
  pinMode(11, OUTPUT);

  // 10 is RESET
  pinMode(8, OUTPUT);
  digitalWrite(8, HIGH);
  delay(1000);
  digitalWrite(8, LOW);
  
  // Start the SPI interface
  SPI.begin();
  //SPI.setClockDivider(SPI_CLOCK_DIV128);
}

void sendCommand(byte command) {
  digitalWrite(13, HIGH);
  delay(1);
  // Set SPI Mode
  // "I am pulsing the clock on the display at about 30-40ms." = 25Hz - 33Hz? 
  SPI.beginTransaction(SPISettings(100, LSBFIRST, SPI_MODE0));  
  // "To send the bits out you place the relevant bit value on pin 2 of the port and then provide it a LO clock pulse . That is a HI-LO. "
  SPI.transfer(command); // Turn on display
  SPI.endTransaction();
  //  digitalWrite(13, HIGH);
}

void loop() {

 //sendCommand(130); // Turn on screen
 sendCommand(172); // Test
 sendCommand(20); // Test
 delay(1000);
}
*/                              
// See https://picaxeforum.co.uk/threads/using-spi-serial-vfds-from-fruit-machines-with-picaxe-comprehensive-code-examples.22552/ 
// Examples using the Samsung 16L102DA4 matrix display (SPI, 12V)
// and the Samsung 16LF01UA3 Starburst display (5V)

// INCLUDES
#include <SPI.h>

void setup() {
  Serial.begin(115200);

  pinMode(13, OUTPUT);
  pinMode(11, OUTPUT);
  // RESET
  pinMode(8, OUTPUT);
  digitalWrite(8, HIGH);

  // As the VFD will be the only thing on the bus for now, /SEL can remain low when idle.   
  pinMode(10, OUTPUT);
  digitalWrite(10, LOW);
  
  // Start the SPI interface
  SPI.begin();

  // Set SPI Mode
  // spimedium  (clock freq / 16)   (= 250kHz with 4MHz resonator)
  SPI.beginTransaction(SPISettings(250000, LSBFIRST, SPI_MODE0));


  // All segments ON
  sendCommand(0b01110011);
  
}

void sendCommand(byte command) {
// See also https://forum.arduino.cc/t/spi-transactions-not-working-with-atmega1280/319194/2
  // "Before sending an command, it (SEL) must then be pulsed high."
  // pulsout cs,1 is 10us at 4Mhz, but some versions of code seem to set seqfreq m16
  // https://picaxe.com/basic-commands/digital-inputoutput/pulsout/
  digitalWrite(10, HIGH);
  delayMicroseconds(100);
  digitalWrite(10, LOW);
  delayMicroseconds(10);

  for (int i=0; i <8; i++) {
    digitalWrite (13, LOW); // SCLK high:
    delay (1); // Wait for new data / commands
    digitalWrite (11, bitRead (command, i)); // Write the i bit of the dat byte
    delay (1);
    digitalWrite (13, HIGH); // SCLK active on falling edge
    delay(1);
    digitalWrite (13, LOW); // SCLK high:
  }
}

void sendData(byte data) {
  for (int i=0; i <8; i++) {
    digitalWrite (13, LOW); // SCLK high:
    delay (2); // Wait for new data / commands
    digitalWrite (11, bitRead (data, i)); // Write the i bit of the dat byte
    delay (2);
    digitalWrite (13, HIGH); // SCLK active on falling edge
    delay(2);
    digitalWrite (13, LOW); // SCLK high:
  }
}

void loop() {
 sendCommand(0x60); // Set display length
 sendCommand(0x57); // Set initial brightness to max
 sendCommand(0x73); // Turn on display

 sendCommand(0x10);
 sendData(0b01100100);

 delay(3000);
}
