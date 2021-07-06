// INCLUDES
#include <AltSoftSerial.h>
// GLOBALS
AltSoftSerial altSerial;
long xxxx[0];
char simplePoll[] = {4, 0, 1, 254, 255};
char count = 0 ;

void setup() {
  Serial.begin(9600);
  altSerial.begin(9600);
}

void loop() {

  char c;
  if (count < 5) {
    altSerial.write(simplePoll[count]);
    count++;
  }

  if (altSerial.available()) {
    c = altSerial.read();
    Serial.print(c);
  }

  
  

if(Serial.available()) {
  c =Serial.read();
  altSerial.print(c);
  
  Serial.print(c);
}
  
}
