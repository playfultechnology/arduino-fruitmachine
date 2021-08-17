/**
 * Arduino Fruit Machine Coin Counter
 * 
 * This is a simple coin counter that uses an "IR obstacle avoidance sensor", with 
 * the IR LED and photodiode placed on opposite sides of the Y-chute, so that the
 * beam is broken and counted each time a coin is inserted.
 */

// DEFINES
// If possible, it is preferable to connect the output of the sensor to a GPIO pin that supports external interrupt
// i.e. Pin 2,3 on an Arduino UNO/Nano, or 2,3,18,19,20,21 on a Mega
// That means we don't have to poll the sensor, and less likely to fail to detect coins
// #define USE_INTERRUPT
 
// INCLUDES
// For interfacing with MP3 sound board
#include "SerialMP3Player.h"
#include "AltSoftSerial.h"

// CONSTANTS
const byte sensorPin = 2;
const byte relayPin = 4;

// There are 7 buttons on the front fact of the "Max-A-Million" machine
const byte numButtons = 7; 
const byte buttonPins[numButtons] = { 10, 11, 12, A0, A1, A2, A3 };

// GLOBALS
// Uses Rx 8, Tx 9
AltSoftSerial altSerial;
// Will initialise a software serial emulation on the specified Rx/Tx pins
SerialMP3Player mp3;

byte relayState = 0;

// Number of times the trigger has fired
volatile unsigned int coinCount = 0;
volatile bool hasChanged = false;

void setup() {

  Serial.begin(115200);
  Serial.println(__FILE__ __DATE__);

  pinMode(sensorPin, INPUT);
  #ifdef USE_INTERRUPT
    attachInterrupt(digitalPinToInterrupt(sensorPin), coinTrigger, FALLING);
  #endif

  // Start MP3-communication
  altSerial.begin(9600);
  mp3.begin(altSerial);        
  mp3.sendCommand(CMD_SEL_DEV, 0, 2);   //select sd-card

  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);

  for(int i=0; i<numButtons; i++){
    pinMode(buttonPins[i], INPUT_PULLUP);
  }

  
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

  // If not using interrupts, poll the sensor manually
  #ifndef USE_INTERRUPT
    // Sensor goes HIGH when a coin passes through
    if(digitalRead(sensorPin) == HIGH) {
      coinTrigger();
    }
  #endif

  for(int i=0; i<numButtons; i++) {
    if(digitalRead(buttonPins[i] == LOW)){
      mp3.play(i); 
    }
  }


  if(hasChanged) {
    // Play the specified SFX
    mp3.play(18);
      
    Serial.println(coinCount);

    // Toggle relay
    relayState = 1-relayState;
    digitalWrite(relayPin, relayState);
    
    hasChanged = false;
  }
}
