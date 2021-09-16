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
/*
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
*/
/*
168+7 values Reprogram character to user defined graphic.  A bit tricky to use I will put a seperate section up for it. Also the 7 numbers after the Command code are not ascii values but are true 8 bit numbers. 
184+1 value Unknown - Tried it every which way doesn't seem to have an effect on characters or display.
220+2 values Unknown- Sometimes causes display reset so mabye something to do with factory testing
245-249 Control of window areas,  I havent experimented with them yet.
254/255 Reset window areas
*/
#define REVERSE_SCROLL 0xa3
#define FORWARDS_SCROLL 0xa2
#define REVERSE_NOSCROLL 0xa1
#define FORWARDS_NOSCROLL 0xa0
#define CLEAR_SCREEN 0xb1 // b3
#define FLASH_SCREEN 0xd1 // contents stay flashing new chars dont
#define FLASH_OFF 0xd2
#define FLASH_LASTCHAR 0xd8 // make last char sent flash
#define SCREEN_OFF 0x80 // Doesn't clear contents
#define SCREEN_ON 0x81 // contents re-display
#define BRIGHTNESS 0x84 // followed by brightness byte (0==full 7=lowest) needs extended commands
#define ENABLE_EXTENDED 0x85 // turn on extra commands
#define ENABLE_LOWERCASE 0xbc // turn on extended font
#define USER_DEFINED_SUMMIT 0xa8 //  (for making user defined char)
#define SCROLL_FROM_0 0xe0
#define SCROLL_FROM_1 0xe1
#define SCROLL_FROM_2 0xe2
#define SCROLL_FROM_3 0xe3
#define SCROLL_FROM_4 0xe4
#define SCROLL_FROM_5 0xe5
#define SCROLL_FROM_6 0xe6
#define SCROLL_FROM_7 0xe7
#define SCROLL_FROM_8 0xe8
#define SCROLL_FROM_9 0xe9
#define SCROLL_FROM_10 0xea
#define SCROLL_FROM_11 0xeb
#define SCROLL_FROM_12 0xec
#define SCROLL_FROM_13 0xed
#define SCROLL_FROM_14 0xee
#define SCROLL_FROM_15 0xef
#define POS_1 0x90
#define POS_2 0x91
#define POS_3 0x92
#define POS_4 0x93
#define POS_5 0x94
#define POS_6 0x95
#define POS_7 0x96
#define POS_8 0x97
#define POS_9 0x98
#define POS_10 0x99
#define POS_11 0x9a
#define POS_12 0x9b
#define POS_13 0x9c
#define POS_14 0x9d
#define POS_15 0x9e
#define POS_16 0x9f



// CONSTANTS
// We will not be using any hardware SPI interface, so these can be assigned to any GPIO pins you want
const byte dataPin = 11;
const byte clockPin = 13;
const byte resetPin = 10;

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

  Reset_VFD();
  WriteData(SCREEN_ON);

WriteData(ENABLE_EXTENDED);
WriteData(BRIGHTNESS);
WriteData(0);
  
  Write("ALastair", 8);
}


// https://hackaday.io/project/1546-vfd-ntp-clock/log/6009-code-snippets
void sendbyte(uint8_t d_chr) {
  unsigned char bit_cnt;
  for(bit_cnt=0; bit_cnt<8; bit_cnt++) {
    if(d_chr & 0x80) digitalWrite(dataPin,HIGH);
    else digitalWrite(dataPin,LOW);
    d_chr <<= 1;
    digitalWrite(clockPin,LOW);
    digitalWrite(clockPin,HIGH);
  }
}

void Reset_VFD(void) {
  digitalWrite ( resetPin, HIGH );
  delay(10);
  digitalWrite ( clockPin, HIGH);
  digitalWrite ( resetPin, LOW );
  delay(50);
}

void loop() {
  // Pass through any data typed via the serial monitor
  if(Serial.available() > 0) {
    byte c = Serial.read();
    Write(c);    
  }
  delay(50);
}
