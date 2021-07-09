// DEFINES
#define ArrayLength(x) (sizeof(x) / sizeof((x)[0]))

// CONSTANTS
// Good explanation here: https://lastminuteengineers.com/74hc595-shift-register-arduino-tutorial/
// Strobe/Latch/RCLK (also sometimes confusingly called "Clock", as in "clocking in") (pin 1) of TPIC6B595
int strobePin = 11;
// Ser/Data/SerIn/MOSI (pin 2) of TPIC6B595
int dataPin = A5;
// Clock/SRCLK Pin connected to Clock (pin 3) of TPIC6B595
int clockPin = 5;
// The predefined light patterns to toggle between
// If, rather than using predefined patterns you'd like to dynamically turn on or off outputs from the shift register,
// look at bitSet(), bitClear(), bitWrite()
const byte patterns[] = {B00111111, B10000110, B01011011, B11001111, B01100110, B11101101, B11111101, B00000111, B01111111, B11101111, B01110111, B01111100, B10111001, B01011110, B11111001, B01110001, B01001001}; 

// GLOBALS
int i = 0;

void setup() {
  //set pins to output because they are addressed in the main loop
  pinMode(strobePin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  // If OE is not tied to GND, then you can manually enable the shift register by writing a LOW value to its pin
  // pinMode(oePin, OUTPUT);
  // digitalWrite(oePin, HIGH);
}

void loop() {
  digitalWrite(strobePin, LOW);
  //set strobe pin low to begin storing bits
  // Then shift in the value of which segment anodes will be lit
  shiftOut(dataPin, clockPin, LSBFIRST, patterns[i]);  
  //set strobe pin high to stop storing bits
  digitalWrite(strobePin, HIGH);
  delay(1000);

  i++;
  if(i>=ArrayLength(patterns)) {
    i=0;
  }
  
}
