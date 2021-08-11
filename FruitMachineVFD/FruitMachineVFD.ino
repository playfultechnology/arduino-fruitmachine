/**
 * Code to interface with a Samsung 16LF01UA3 (rev D) VFD display, as found in "Max-A-Million" fruit machine 
 * 
 * References:
 * https://gutenber3d.wordpress.com/2015/02/15/control-de-vfd-vacuum-fluorescent-display-16f01ua3-samsumg-con-arduino-nano/
 * http://cfile203.uf.daum.net/attach/2504384856450F340E422D
 * https://github.com/DnaX/Samsung_16LF01_VFD
 * https://www.youtube.com/watch?v=ArL7DFZDl7g (have to "display all comments" to see code)
 *
 * From "4.8 Signal Interfacing" of 16LF01UA3 datasheet
 * 1 VCC   2 VCC    +5V DC  (typical 600mA, but surge on power on may be 3A)
 * 3 N/P   4 N/C    No Pin, Not Connected
 * 5 N/C   6 N/C    Not Connected
 * 7 N/C   8 SCLK   Shift Clock (Falling Edge Active)
 * 9 DATA 10 /RST   Data In line, Reset (Active Low)
 * 11 GND 12 GND    Ground
 */

// INCLUDES
// This library seems to pretty nicely encapsulate the timings as specified in the datasheet
#include "src/Samsung_16LF01_VFD.h"

// CONSTANTS
const byte SCLKpin = A4;
const byte DATApin = A5;
const byte RSTpin = A0; // Not really required, since integrated RC reset circuit on board

// GLOBALS
// SCLK / DATA / RST
Samsung_16LF01_VFD vfd(SCLKpin, DATApin, RSTpin);
// The message to be scrolled
char message[] = "Clued Up! Norwich Casino Escape Room       ";
// The current value displayed on each digit of the display
char digits[17] = "                ";
int i;
int firstDigit = 0;
int lastDigit = strlen(message) - 1;

void setup() {
  // Initialize the display (number of characters, brightness (in 1/31ths)
  vfd.begin(16,31);
  
  // Display a welcome static message
  vfd.print("Playful Tech");
  delay(5000);
  vfd.clear();

  // Go to the first position
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

    // Workaround, "points" and "commas" are in the same digit
    if (digits[i] == ',' || digits[i] == '.')
      vfd.print(" ");
  }

  delay(250);
}
