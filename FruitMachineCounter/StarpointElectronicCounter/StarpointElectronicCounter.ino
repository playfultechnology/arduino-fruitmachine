/*
See https://www.mikrocontroller.net/attachment/163201/Zaehlwerk_27-080_01__man.pdf
 */

// DEFINES
// Response code values
#define ACK 61
#define DATA 60
#define NAK 62

// INCLUDES
#include <SPI.h>

// GLOBALS
// Array to hold received responses
byte response[8];

void setup() {
  Serial.begin(115200);


  pinMode(11, OUTPUT);
  pinMode(13, OUTPUT);
  /*
  while(true){
    digitalWrite(13, HIGH);
    for(int i=0;i<5;i++){
    digitalWrite(11, HIGH);
    delay(1);
    digitalWrite(11, LOW);
    delay(1);
    }
    delay(10);
    digitalWrite(13, LOW);
        for(int i=0;i<5;i++){
    digitalWrite(11, HIGH);
    delay(1);
    digitalWrite(11, LOW);
    delay(1);
    }
    delay(10);
  }
*/

  

  // Start the SPI interface
  SPI.begin();

  // According to "Communications Interface" (p.8)
  // Uses SPI Mode 2, MSB, Clock speed between 100Hz - 5kHz
  SPI.beginTransaction(SPISettings(100, MSBFIRST, SPI_MODE2));

  // Send a command - see "Command Format From Host" (p.10) for general structure
  // This is "r. <54> Cycle Counter Display" (p.19)
  // Which should cause counter to flash through all counter values
  SPI.transfer(0x54); // CMD
  SPI.transfer(0x00); // ID starts at 00 and increments with each message sent
  SPI.transfer(0x00); // DCNT - number of databytes following (not including CHKSUM)
  SPI.transfer(0x54); // CHECKSUM is lower 8 bits of sum of all preceding bytes

  // Clear the response buffer
  memset(response, 0, sizeof(response));
  // Then fill it with 4 byte response
  for (int i=0; i<4; i++) {
    response[i] = SPI.transfer(0x00); // Dummy value just to receive SPI response
  }

  // Release SPI interface
  //SPI.endTransaction();
  //SPI.end();

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

  // According to "Communications Interface" (p.8)
  // Uses SPI Mode 2, MSB, Clock speed between 100Hz - 5kHz
  //SPI.beginTransaction(SPISettings(200, MSBFIRST, SPI_MODE2));


  digitalWrite(13, HIGH);

  // Send a command - see "Command Format From Host" (p.10) for general structure
  // This is "r. <54> Cycle Counter Display" (p.19)
  // Which should cause counter to flash through all counter values
  SPI.transfer(0x54); // CMD
  SPI.transfer(0x02); // ID starts at 00 and increments with each message sent
  SPI.transfer(0x00); // DCNT - number of databytes following (not including CHKSUM)
  SPI.transfer(0x56); // CHECKSUM is lower 8 bits of sum of all preceding bytes

/*
  // Send a command - see "Command Format From Host" (p.10) for general structure
  // This is "r. <54> Cycle Counter Display" (p.19)
  // Which should cause counter to flash through all counter values
  SPI.transfer(0x21); // CMD
  SPI.transfer(0x01); // ID starts at 00 and increments with each message sent
  SPI.transfer(0x00); // DCNT - number of databytes following (not including CHKSUM)
  SPI.transfer(0x22); // CHECKSUM is lower 8 bits of sum of all preceding bytes
*/

  // Clear the response buffer
  memset(response, 0, sizeof(response));
  // Then fill it with 4 byte response
  for (int i=0; i<4; i++) {
    response[i] = SPI.transfer(0x00); // Dummy value just to receive SPI response
  }

  digitalWrite(13, HIGH);


  // Release SPI interface
  // SPI.endTransaction();
  // SPI.end();



  delay(1000);
  
}
