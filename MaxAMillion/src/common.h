#ifndef FRUITMACHINE_COMMON_H
#define FRUITMACHINE_COMMON_H

// DEFINES
// Helper function to get length of an arbitrary array
#define ArrayLength(x) (sizeof(x) / sizeof((x)[0]))

/**
 * Returns modulo, and deals correctly with negative numbers (unlike builtin %)
 */
int mod(int x, int y ){
  return x<0 ? ((x+1)%y)+y-1 : x%y;
}

// See https://forum.arduino.cc/t/rounding-an-integer-up-only/350784/3
long roundUp(long val, long factor) {
  long rem = val % factor;
  val -= rem;
  if (rem > 0) val += factor;
  return val;
}

/**
 * Displays a PROGMEM string on LCD screen and via serial connection
 */
void logMessage(const __FlashStringHelper* message){
/*
  for(int i=0; i<3; i++){
    strncpy(displayBuffer[i], displayBuffer[i+1], sizeof(displayBuffer[i]));
  }
  // Note use of right-padding to fill whole line
  snprintf_P(displayBuffer[3], sizeof(displayBuffer[3]), PSTR("%-20S") , message);
*/	
  Serial.println(message);
}

#endif