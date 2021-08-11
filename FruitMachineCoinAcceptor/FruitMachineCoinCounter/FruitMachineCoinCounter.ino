/**
 * Arduino Fruit Machine Coin Counter
 * 
 * This is a simple coin counter that uses an "IR obstacle avoidance sensor", with 
 * the IR LED and photodiode placed on opposite sides of the Y-chute, so that the
 * beam is broken and counted each time a coin is inserted.
 */

// CONSTANTS
// GPIO pin must support external interrupts (i.e. Pin 2/3 on Arduino)
const byte interruptPin = 2;

// GLOBALS
// Number of times the trigger has fired
volatile unsigned int coinCount = 0;
volatile bool hasChanged = false;

void setup() {

  Serial.begin(115200);
  Serial.println(__FILE__ __DATE__);

  pinMode(interruptPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(interruptPin), coinTrigger, FALLING);
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
    // put your main code here, to run repeatedly:
    Serial.println(coinCount);
    hasChanged = false;
  }
}
