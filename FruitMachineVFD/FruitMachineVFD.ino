
// https://www.fruitemu.co.uk/ib/index.php?app=forums&module=forums&controller=topic&id=17419

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
