/**
 * Illuminated Arcade Button using PL9823 Addressable LED
 * Modifies the hue of the LED each time the button is pressed
 */

// DEFINES
// Which pin on the Arduino is connected to the DataIn of the first PL9823?
#define DATA_PIN A0

// CONSTANTS
// Which pin is the button connected to?
const byte buttonPin = 2;

// INCLUDES
// Download from https://github.com/FastLED/FastLED/releases
#include <FastLED.h>

// GLOBALS
// Define the hue of the colour in which the button will be lit.
// This will change each time the button is pressed
uint8_t gHue = 0;
// What was last known state of the button?
bool lastButtonState;
// Define the array of leds. We'll just use a single button
CRGB leds[1];


void setup() { 
  // Set up the button input
  pinMode(buttonPin, INPUT_PULLUP);
  
  // Set up the LED output
  FastLED.addLeds<PL9823, DATA_PIN, RGB>(leds, 1);
}

void loop() { 
  // Is the button currently being pressed?
  bool buttonState = digitalRead(buttonPin);
  // If it's now down, but wasn't before, then it's just been pressed
  if(!buttonState && lastButtonState) {
    // Increase the hue
    gHue += 30;
  }
  // Update the button state
  lastButtonState = buttonState;

  // Set the LED to the given hue value
  leds[0] = CHSV(gHue, 255, 192);
  FastLED.show();
}
