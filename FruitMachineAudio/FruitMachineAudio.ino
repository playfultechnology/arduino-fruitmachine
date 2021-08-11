/**
 * Input / Audio module
 * Plays samples from VS1053 corresponding to different input pins
 */
// INCLUDES
#include <SPI.h>
// For reading files stored on SD card
#include <SD.h>
// For interfacing with MP3 sound board
#include <Adafruit_VS1053.h>
#include <Bounce2.h>

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
// GPIO pins which, when grounded, will cause associated indexed sound effects to play
const byte numInputs = 9;
const byte inputPins[numInputs] = { A0, A1, A2, A3, A4, A5, 3, 4, 5 };
const char* sfxFiles[numInputs] = { "POP.MP3", "DOMPH.MP3", "PING.MP3", "BRING.MP3", "CLICK.MP3", "DAHDALA.MP3", "DAHDALOO.MP3", "CHINK.MP3", "TRILL.MP3" };

// GLOBALS
Adafruit_VS1053_FilePlayer musicPlayer = Adafruit_VS1053_FilePlayer(BREAKOUT_RESET, BREAKOUT_CS, BREAKOUT_DCS, DREQ, CARDCS);
Bounce2::Button inputs[numInputs] = {};

void setup() {
  // Serial connection for debugging
  Serial.begin(115200);
  Serial.println(__FILE__ __DATE__);

  // Initialise input pins 
  for(int i=0; i<numInputs; i++) {
    pinMode(inputPins[i], INPUT_PULLUP);
    inputs[i].attach(inputPins[i], INPUT_PULLUP);
  }

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

  // Play one file, don't return until complete
  // Serial.println(F("Playing track 001"));
  // musicPlayer.playFullFile("CHINK.MP3");
  // Play another file in the background, REQUIRES interrupts!
  // Serial.println(F("Playing track 002"));
  // musicPlayer.startPlayingFile("/track002.mp3");

  // If desired, play a tone to indicate VS1053 is operational
  // musicPlayer.sineTest(0x44, 100);
  // musicPlayer.sineTest(0x50, 100);
}

void loop() {

  for(int i=0; i<numInputs; i++) {
    // Check for any changes
    inputs[i].update();

    // Has the input been pressed this frame?
    if(inputs[i].fell()){
      // Only start a new track playing if old one has stopped
      if(!musicPlayer.playingMusic){
        Serial.print(F("Now playing "));
        Serial.println(sfxFiles[i]);
        musicPlayer.startPlayingFile(sfxFiles[i]);
      }
      // If a previous SFX was playing, stop it before starting new one
      else {
        musicPlayer.stopPlaying();
        delay(10);
        musicPlayer.startPlayingFile(sfxFiles[i]);
      } 
    }
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
