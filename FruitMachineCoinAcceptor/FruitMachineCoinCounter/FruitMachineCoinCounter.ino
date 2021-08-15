/**
 * Arduino Fruit Machine Coin Counter
 * 
 * This is a simple coin counter that uses an "IR obstacle avoidance sensor", with 
 * the IR LED and photodiode placed on opposite sides of the Y-chute, so that the
 * beam is broken and counted each time a coin is inserted.
 */
// INCLUDES
#include <SPI.h>
// For reading files stored on SD card
#include <SD.h>
// For interfacing with MP3 sound board
//#include <Adafruit_VS1053.h>
#include "SerialMP3Player.h"



// DEFINES
// Note that, although the following are not explicitly used in the code, they are unavailable
// as referenced by the SPI interface to the VS1053
// #define CLK 13       // SPI Clock, shared with SD card
// #define MISO 12      // Input data, from VS1053/SD card
// #define MOSI 11      // Output data, to VS1053/SD card
#define BREAKOUT_RESET  8      // VS1053 reset pin (output) marked XRESET
#define BREAKOUT_CS     6     // VS1053 chip select pin (output) marked XCS
#define BREAKOUT_DCS    7      // VS1053 Data/command select pin (output) marked XDCS
#define CARDCS 9     // Card chip select pin
#define DREQ 2       // VS1053 Data request, ideally an Interrupt pin

// CONSTANTS
// GPIO pin must support external interrupts (i.e. Pin 2/3 on Arduino)
const byte interruptPin = 2;
const byte relayPin = 4;

// GLOBALS
//Adafruit_VS1053_FilePlayer musicPlayer = Adafruit_VS1053_FilePlayer(BREAKOUT_RESET, BREAKOUT_CS, BREAKOUT_DCS, DREQ, CARDCS);
SerialMP3Player mp3(8,9);

byte relayState = 0;

// Number of times the trigger has fired
volatile unsigned int coinCount = 0;
volatile bool hasChanged = false;

void setup() {

  Serial.begin(115200);
  Serial.println(__FILE__ __DATE__);

  pinMode(interruptPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(interruptPin), coinTrigger, FALLING);

/*
  // Initialise VS1053
  if(!musicPlayer.begin()) { // Initialise the VS1053 MP3 player
     Serial.println(F("ERROR: Could not find VS1053"));
     while(1);
  }
  Serial.println(F("VS1053 found"));
  delay(250);
  if(!SD.begin(CARDCS)) {
    Serial.println(F("Could not initialise SD Card"));
    while (1);
  }
  Serial.println(F("SD Card initialised"));
  delay(250);

  // List files on SD card
  printDirectory(SD.open("/"), 0);

  // Set volume for left, right channels. Lower numbers == louder volume!
  musicPlayer.setVolume(5,5);

  // If DREQ is on an interrupt pin (on uno, #2 or #3) we can play audio in background
  musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);  // DREQ int
*/

 mp3.begin(9600);        // start mp3-communication
  mp3.sendCommand(CMD_SEL_DEV, 0, 2);   //select sd-card

  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);
  
}

// ISR is called when sensor beam is broken by coin input
// Still need to debounce though, see https://forum.arduino.cc/t/programming-an-isr-to-debounce-a-switch/477685
void coinTrigger() {
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if(interrupt_time - last_interrupt_time > 200) {
    coinCount = coinCount + 1;
    hasChanged = true;
  }
  last_interrupt_time = interrupt_time;
}

void loop() {
  if(hasChanged) {
    /*
      // Only start a new track playing if old one has stopped
      if(!musicPlayer.playingMusic){
        Serial.print(F("Now playing "));
        musicPlayer.startPlayingFile("DAHDALA.MP3");
      }
      // If a previous SFX was playing, stop it before starting new one
      else {
        musicPlayer.stopPlaying();
        delay(10);
        musicPlayer.startPlayingFile("DAHDALA.MP3");
      }     
      */

    mp3.play(1);
      
    // put your main code here, to run repeatedly:
    Serial.println(coinCount);

    // Toggle relay
    relayState = 1-relayState;
    digitalWrite(relayPin, relayState);

    
    hasChanged = false;
  }
}


/// File listing helper
void printDirectory(File dir, int numTabs) {
   while(true) {     
     File entry =  dir.openNextFile();
     if (! entry) {
       // no more files
       //Serial.println("**nomorefiles**");
       break;
     }
     for (uint8_t i=0; i<numTabs; i++) {
       Serial.print('\t');
     }
     Serial.print(entry.name());
     if (entry.isDirectory()) {
       Serial.println("/");
       printDirectory(entry, numTabs+1);
     } else {
       // files have sizes, directories do not
       Serial.print("\t\t");
       Serial.println(entry.size(), DEC);
     }
     entry.close();
   }
}
