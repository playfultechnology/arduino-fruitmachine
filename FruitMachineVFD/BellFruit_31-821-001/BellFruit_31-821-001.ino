/**
 * Code to interface with a VFD display extracted from a Bell Fruit "Snakes and Ladders" fruit machine game
 * and often used in slot machines based on the "Scorpion 4" or "Scorpion 5" MPU.
 * 
 * The display is a Futaba 16-character dot matrix display, marked 31-821-001 ISS 1, as seen at:
 * https://www.fruitmachinesales.co.uk/bell-fruit-16-character-alphanumeric-display
 * 
 * It uses an NEC F780204GF-045 chip: 
 * https://pdf1.alldatasheet.net/datasheet-pdf/view/125501/NEC/D780206GF.html
 * 
 * The display has a 6 pin connector :
 * 1 - +12v
 * 2 - Clock
 * 3 - N/C
 * 4 - Data
 * 5 - Reset
 * 6 - Ground
 * 
 * More info at:
 * https://www.fruit-emu.com/forums/topic/6080-eclipse-cabs-bellfruit/
 * https://www.fruitemu.co.uk/ib/index.php?app=forums&module=forums&controller=topic&id=17419
 */
// DEFINES
#define DISPLAY_OFF 128
#define DISPLAY_ON 129
#define SET_BRIGHTNESS 132 // Followed by ASCII value from 1(brightest) to 6 (dimmest)
#define USER_DEFINED_CHAR_MODE 133 // also turns flash mode off
#define START_ADDRESS_1 144 // 144-151 Set start address (send a number from 144-151 to select which of the 16 digits to start showing characters from
#define WRAP_LTR 160 // Display prints to right (i.e default, characters are printed from left - right and will wrap around back to left if you print more than 16 characters)
#define WRAP_RTL 161 // Display prints to left (i.e. backwards, wraps around to right)
#define SCROLL_LTR 162 // Display scrolls to the left
#define SCROLL_RTL 163 // Display scrolls to the right (can cause decimal point, full stop or commas to be displayed in wrong place, ie before letter instead of after it.)
#define TEST 172 // Multiplexer test -  the display goes through a test pattern to ensure the glass and driving row and coloumn multiplexers are working.
#define CLEAR_SCREEN 177 // Note - does not reset cursor position
#define RESET_CHARSET 188 // Gets rid of any user definitions you made to the character set
#define FLASH_MODE_ON 200 // Only works if command 133 has not been sent. Use this code to allow the flash character (ascii 37) to work.
#define INHIBIT_CLEAR_1 225 // First digit will not be cleared by CLEAR_SCREEN. Use 226-241 to inhibit other characters
#define INHIBIT_RESET 255 // Removes any inhibit conditions set
/*
168+7 values Reprogram character to user defined graphic.  A bit tricky to use I will put a seperate section up for it. Also the 7 numbers after the Command code are not ascii values but are true 8 bit numbers. 
184+1 value Unknown - Tried it every which way doesn't seem to have an effect on characters or display.
220+2 values Unknown- Sometimes causes display reset so mabye something to do with factory testing
245-249 Control of window areas,  I havent experimented with them yet.
254/255 Reset window areas
*/

// CONSTANTS
// We will not be using any hardware SPI interface, so these can be assigned to any GPIO pins you want
const byte dataPin = 3;
const byte clockPin = 4;
const byte resetPin = 2;

// "To send the bits out you place the relevant bit value on pin 2 of the port and then provide it a LO clock pulse . That is a HI-LO. "
void WriteData (byte dat) {
  for (int i=7; i >=0; i--) {
    digitalWrite (dataPin, bitRead (dat, i));
    delayMicroseconds (50);
    digitalWrite (clockPin, LOW);
    delayMicroseconds(50);
    digitalWrite (clockPin, HIGH);
  }
}

void Write(char str[], int len, int delayTime=0) {
  for(int i=0; i<len;i++){
    Write(str[i]);
    delay(delayTime);
  }
}

void Write(char c){
  // Map from ASCII character code to VFD charmap entry
  // Uppercase letters and some symbols [\]^_ to first section of charmap
  if (c >= 64 && c <= 95) {
    WriteData(c - 64);
  }
  // Second section of charmap (numbers and remaining symbols)
  else if (c >= 32 && c <= 63) {
    WriteData(c);
  }
  // Also map lower case letters to first section
  else if (c >= 97 && c <= 122) {
    WriteData(c - 96);
  }
  else if(c == 0x0A || c == 0x0D) { // 0x0A = \n (newline), 0x0D = \r (Carriage Return)
    WriteData(144);
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println(__FILE__ __DATE__);

  pinMode(clockPin, OUTPUT);
  digitalWrite (clockPin, HIGH);
  pinMode(dataPin, OUTPUT);
  pinMode(resetPin, OUTPUT);

  // Reset the display to ensure it wasn't halfway through processing a command
  digitalWrite(resetPin, HIGH);
  delay(100);
  digitalWrite(resetPin, LOW);
  delay(1000);

  // Start printing at first character
  WriteData(144);

  // Scroll To Left
  WriteData(162);

  Write("Ex-Slot Machine", 15);
  delay(100000);
  attract();
}

void attract(){

  WriteData(SCROLL_LTR);
  Write("This is a scrolling message...", 30, 200);
  Write("                ", 16, 200);
  Write("You can scroll slowly...", 24, 400);
  Write("                ", 16, 400);
  Write("Or you can scroll quickly", 25, 75);
  Write("                ", 16, 50);

  WriteData(SCROLL_RTL);
  Write("Or even backwards", 17, 300);
  Write("                ", 16, 300);
  
  delay(1000);
  WriteData(SCROLL_LTR);
  WriteData(FLASH_MODE_ON);
  Write("You can also F%L%A%S%H% letters", 31, 200);
  Write("                ", 16, 200);
  delay(1000);

  WriteData(WRAP_LTR);
  Write("Or Stack letters", 16, 200);
  delay(1000);
  WriteData(CLEAR_SCREEN);
  WriteData(SCROLL_LTR);
  Write("L               ", 16, 100);
  WriteData(INHIBIT_CLEAR_1);
  Write("I               ", 15, 100);
  WriteData(INHIBIT_CLEAR_1+1);
  Write("K               ", 14, 100);
  WriteData(INHIBIT_CLEAR_1+2);
  Write("E               ", 13, 100);
  WriteData(INHIBIT_CLEAR_1+3);
  Write("                ", 12);
  WriteData(INHIBIT_CLEAR_1+4);
  Write("T               ", 11, 100);
  WriteData(INHIBIT_CLEAR_1+5);
  Write("H               ", 10, 100);  
  WriteData(INHIBIT_CLEAR_1+6);
  Write("I               ", 9, 100);
  WriteData(INHIBIT_CLEAR_1+7);
  Write("S               ", 8, 100);

  WriteData(INHIBIT_RESET);

  delay(3000);

  WriteData(CLEAR_SCREEN);
  for(int i=1;i<8;i++){
    WriteData(START_ADDRESS_1);    
    WriteData(SET_BRIGHTNESS);
    WriteData((char)i);
    Write("BRIGHTNESS ", 11);
    char buffer[2];
    itoa(i,buffer,10);    
    Write(buffer,2);
    delay(500);
  }
  for(int i=7;i>=1;i--){
    WriteData(START_ADDRESS_1);    
    WriteData(SET_BRIGHTNESS);
    WriteData((char)i);
    Write("BRIGHTNESS ", 11);
    char buffer [2];
    itoa(i,buffer,10);    
    Write(buffer,2);
    delay(500);
  }

  delay(3000);
  WriteData(CLEAR_SCREEN);
  WriteData(WRAP_LTR);
  WriteData(START_ADDRESS_1);   
  Write("Now, try sending some data over the serial connection", 53, 150);
  WriteData(SCROLL_LTR);
  Write("                    ", 20, 150);
}


void loop() {
  // Pass through any data typed via the serial monitor
  if(Serial.available() > 0) {
    byte c = Serial.read();
    Write(c);    
  }
  delay(50);
}
