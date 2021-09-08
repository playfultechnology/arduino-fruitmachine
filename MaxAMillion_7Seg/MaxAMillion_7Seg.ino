/**
 * Arduino "Max-A-Million" 7-Segment LED Controller
 * Copyright (c) 2021 Playful Technology
 */
// DEFINES
#define ArrayLength(x) (sizeof(x) / sizeof((x)[0]))

// INCLUDES
// I2C connection to 7-segment daughterboard
#include <Wire.h>
#include "src/ASCIIto7Segment.h"

// CONSTANTS
const byte slave7SegmentAddress = 9;
const byte numDigits = 4;
// Pins to control shift register(s)
const int latchPin = 8;//10; //3; // (Storage Register Clock Input/STCP/RCLK)
const int clockPin = 7;//13; //2; // (Shift Register Clock Input/SHCP/SRCLK)
const int dataPin = 9;//11; //4; // (Din/SER)
// Note that OE needs to be pulled LOW, and SCLR/RST needs to be pulled HIGH


// GLOBALS
// Define the segments that should be lit on each digit. 
// The 7 segments of a single digit (+1 d.p.) can be represented as a single byte, with
// the 8 bits representing the segments .GFEDCBA
//  --a--
// |     |
// f     b
// |     |
//  --g--
// |     |
// e     c
// |     |
//  --d--

unsigned long gameStartTime;
unsigned long gameDuration = 60 * 60000; // 60 mins
unsigned long gameDeductedTime;

unsigned long lastUpdateTime;

// The array of segments that should currently be lit
// We'll initialise these to initially show only segment G (the horizontal bar in the middle)
byte segmentsToDisplay[numDigits] = {0b01000000, 0b01000000, 0b01000000, 0b01000000};
// Keep track of the current state of the device
enum State {Inactive, Active};
State state = State::Inactive;

// Initial setup
void setup() {
  // Start a serial connection
  Serial.begin(115200);
  // Don't wait for ages for serial input
  Serial.setTimeout(50); 
  Serial.println(__FILE__ __DATE__);

  // Join I2C bus
  Wire.begin(slave7SegmentAddress); 
  // Define the callback
  Wire.onReceive(receiveEvent);
  
  // Define pin modes
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);  
  pinMode(clockPin, OUTPUT);

  pinMode(LED_BUILTIN, OUTPUT);

}

// Assign a random 5-digit number to the display
void setRandomNumber() {
  for(int i=0; i<numDigits; i++){
    // Digits 0-9 correspond to ASCII codes 48-57
    segmentsToDisplay[i] = getSegments((char)(random(48,58)));
  }
}


void Start(){
  state = State::Active;
  gameStartTime = millis();
  Serial.println(F("Timer activated!"));
}

void Stop() {
  Serial.println(F("Timer stopped"));
  unsigned long currentTime = millis();
  // Add the length of time elapsed during the most recent play session
  // to the total time elapsed 
  gameDeductedTime += (currentTime - gameStartTime);
  state = State::Inactive;
}

void Reset(){
  gameDeductedTime = 0;
  state = State::Inactive;
}


// Return the binary representation of the supplied character
byte getSegments(char c) {
  return ASCIIto7Segment[c-32];
};

void receiveEvent(int howMany) {
 Serial.print(F("I2C Message Received"));
  //for (int i=0; i<howMany; i++) {
    byte c = Wire.read(); 
    Serial.print(c, HEX);
    switch(c){
      case 0x00:
        Reset();
        break;
      case 0x01:
        Start();
        break;
      case 0x02:
        Stop();
        break;
      case 0xFF:
        if(howMany == 2) {
          gameDuration = (byte)Wire.read() * 60000;
        }
        break;        
      default:
        break;
    }
  //}
  Serial.println(F(""));
}


void loop() {

  unsigned long currentTime = millis();

  // What to do next depends on the current state of the device
  if(state == State::Active) {

    // The time remaining is the total game duration, less the time spent during the
    // current period of play, less the time elapsed during any previous sessions
    // or other deductions
    long timeRemaining = gameDuration - (currentTime - gameStartTime) - gameDeductedTime;

    if(timeRemaining <= 0) {
      // Game Timer has run out
      state = State::Inactive;
    }
    else {
      // Update clock display
      int seconds = (timeRemaining / 1000)%60;
      int minutes = timeRemaining / 60000;

      uint8_t s2 = seconds%10;
      segmentsToDisplay[3] = getSegments((char)s2+48);
      uint8_t s1 = (seconds/10)%10;
      segmentsToDisplay[2] = getSegments((char)s1+48);
      uint8_t m2 = minutes%10;
      segmentsToDisplay[1] = getSegments((char)m2+48) | 0b10000000;
      uint8_t m1 = (minutes/10)%10;
      segmentsToDisplay[0] = getSegments((char)m1+48);
      /*
      Serial.print(m1);
      Serial.print(m2);
      Serial.print(":");
      Serial.print(s1);
      Serial.println(s2);
      */  
    }
  }
  else if(state == State::Inactive){

    if(currentTime - lastUpdateTime > 3000){
      setRandomNumber();
      lastUpdateTime = currentTime;
    }
  }

  // Loop over every digit
  for(int i=0; i<numDigits; i++){
    // Hold the latch pin low
    digitalWrite(latchPin, LOW);
    // Shift in the value of which segment anodes will be lit by the UDN2981
    shiftOut(dataPin, clockPin, MSBFIRST, 1<<i);    
    // Shift in the value that determines which display cathodes will be grounded by the ULN2803
    shiftOut(dataPin, clockPin, MSBFIRST, segmentsToDisplay[i]);
    // Release the latch
    digitalWrite(latchPin, HIGH);
    // Set this as large as possible before flickering occurs
    delayMicroseconds(1000);
  }
}
