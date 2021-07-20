/*
See https://picaxeforum.co.uk/threads/using-spi-serial-vfds-from-fruit-machines-with-picaxe-comprehensive-code-examples.22552/ 

// However, have had little luck converting to Arduino - not sure if that's because the hardware is DOA

*/

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
