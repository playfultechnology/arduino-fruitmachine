
//ccTalk tutorial simple messaging example
// www.cctalktutorial.wordpress.com 
#define ALTSOFTCOMM
#ifdef ALTSOFTCOMM
  #include <AltSoftSerial.h>
#endif
#include "cctcom.h"
#include "headers.h"
// some garbage , without it my compiler gives a fatal error , you can skip this if the schetch compiles. 
long weqwerqr;long xxxx[1];long sdasdasd[1];long sdasdasa[1];long sdasdase[1];
///
unsigned char deviceID = 3;

void printASCIIdata(){// prints the ascii data field
    for (int i = 4 ; i<RX_buffer[1]+4;i++){
      Serial.write(RX_buffer[i]);
    }
    Serial.println();
}
#define MAXDATALENGTH 58
void printBINdata(){// prints the bin data field
  char get_data[MAXDATALENGTH];
  snprintf(get_data, MAXDATALENGTH, "%d ", RX_buffer);
      
      Serial.println(get_data);
}
void printHEXdata(){// prints the bin data field
    for (int i=0 ; i<RX_buffer[1]+4;i++){
      Serial.print(RX_buffer[i], HEX);
      Serial.print(",");
    }
    Serial.println();
}

void setup() {
  Serial.begin(9600);
  ccTalk_comm_init();
  while (RX_state != RXidle) {
    ccTalkReceive();
  }
}
void loop() {
  cctsend(simple_poll,deviceID,0);
  delay(100);
  while (RX_state < RXcomplete) {
    ccTalkReceive();
  } // wait for an answer or error
  if (RX_state!=RXcomplete) {
    Serial.println(RX_msg[RX_state]); // Show the error
    while(1);// stop
  }
  Serial.println("ccTalk device present"); // simple pool was ok 
  delay(100);
  
  cctsend(request_manufacturer_id,deviceID,0);
  delay(100);
  while (RX_state < RXcomplete) {
    ccTalkReceive();
  }
  if (RX_state != RXcomplete){
    Serial.println(RX_msg[RX_state]);// Show the error if any
   // while(1);// stop
  }
  Serial.print("Manufacturer ID:");
  printASCIIdata(); // Print the manufacturer ID should be Money Controls
  
  cctsend(request_equipment_category_id,deviceID,0);
  delay(100);
  while (RX_state < RXcomplete) {ccTalkReceive();}
  if (RX_state!=RXcomplete){
    Serial.println(RX_msg[RX_state]);// Show the error if any
   // while(1);// stop
  }
  Serial.print("Equipment Category:");
  printASCIIdata(); // Print the equipment category ID  shuold be Payout
  
  cctsend(request_product_code,deviceID,0);
  delay(100);
  while (RX_state < RXcomplete) {ccTalkReceive();}
  if (RX_state!=RXcomplete){
    Serial.println(RX_msg[RX_state]);// Show the error if any
   // while(1);// stop
  }
  Serial.print("Product Code:");
  printASCIIdata(); // Print the product code  should be SCH2
  
  cctsend(request_software_revision,deviceID,0);
  delay(100);
  while (RX_state < RXcomplete) {ccTalkReceive();}
  if (RX_state!=RXcomplete){
    Serial.println(RX_msg[RX_state]);// Show the error if any
   // while(1);// stop
  }
  Serial.print("Software Revision:");
  printASCIIdata(); // Print the Software revision should be SCH2-V2.4


// See "Summary dispense coins example" on https://oem.suzohapp.com/wp-content/uploads/2019/07/18-USE_SERNR-ID_man.pdf
 // Page 26

// Also https://cctalktutorial.wordpress.com/2015/11/13/cctalk-tutorial-testing-some-headers-with-a-coin-hopper-part-2/

  // Send TEST command
  cctsend(test_hopper, deviceID, 0);
  delay(100);
  while (RX_state < RXcomplete) {ccTalkReceive();}
  if (RX_state!=RXcomplete){
    Serial.println(RX_msg[RX_state]);// Show the error if any
   // while(1);// stop
  }
  Serial.print("Test Hopper:");
  printHEXdata(); // C0 indicates that a power up is detected and the hopper is disabled. (p20 https://oem.suzohapp.com/wp-content/uploads/2019/07/18-USE_SERNR-ID_man.pdf)


  // Send ENABLE command
  cctsend(enable_hopper, deviceID, 165);
  delay(100);
  while (RX_state < RXcomplete) {ccTalkReceive();}
  if (RX_state!=RXcomplete){
    Serial.println(RX_msg[RX_state]);// Show the error if any
   // while(1);// stop
  }
  Serial.print("Enable Hopper:");
  printASCIIdata(); 
  
  // Send TEST command AGAIN
  cctsend(test_hopper, deviceID, 0);
  delay(100);
  while (RX_state < RXcomplete) {ccTalkReceive();}
  if (RX_state!=RXcomplete){
    Serial.println(RX_msg[RX_state]);// Show the error if any
   // while(1);// stop
  }
  Serial.print("Test Hopper:");
  printHEXdata(); // C0 indicates that a power up is detected and the hopper is disabled. (p20 https://oem.suzohapp.com/wp-content/uploads/2019/07/18-USE_SERNR-ID_man.pdf)

  cctsend(request_serial_number, deviceID, 0);
  delay(100);
  while (RX_state < RXcomplete) {ccTalkReceive();}
  if (RX_state!=RXcomplete){
    Serial.println(RX_msg[RX_state]);// Show the error if any
   // while(1);// stop
  }
  Serial.print("Serial Number:");
  printHEXdata(); // Print the Software revision should be SCH2-V2.4

  cctsend(request_cipher_key, deviceID, 0);
  delay(200);
  while (RX_state < RXcomplete) {ccTalkReceive();}
  if (RX_state!=RXcomplete){
    Serial.println(RX_msg[RX_state]);// Show the error if any
   // while(1);// stop
  }
  Serial.print("Cipher key:");
  printHEXdata();
/*
// https://oem.suzohapp.com/wp-content/uploads/2019/07/18-USE_SERNR-ID_man.pdf   // 2A 13 30?
unsigned char requestSerialNumber[] = {0x03, 0x00, 0x01, 0xF2, 0x0A };
  for(int i=0; i<5; i++){
    altSerial.write(requestSerialNumber[i]);
  }
  while(altSerial.available() == 0) { delay(10); }
  Serial.print(F("Serial Num:"));
  while (altSerial.available()) {
    char c = altSerial.read();
    Serial.print(c, HEX);
  }
Serial.println("");


  unsigned char test[] = {0x03, 0x00, 0x01, 0xA3, 0x59};
  for(int i=0; i<5; i++){
    altSerial.write(test[i]);
  }
  while(altSerial.available() == 0) { delay(10); }
  while (altSerial.available()) {
    char c = altSerial.read();
    Serial.print(c, HEX);
  }
*/
/*
  cctsend(enable_hopper, deviceID, 165);
  delay(200);
  while (RX_state < RXcomplete) {ccTalkReceive();}
  if (RX_state!=RXcomplete){
    Serial.println(RX_msg[RX_state]);// Show the error if any
   // while(1);// stop
  }
  Serial.print("Enable Hopper:");
  printASCIIdata();


*/


  

/*
  cctsend(166,4,0);
  delay(200);
  while (RX_state < RXcomplete) {ccTalkReceive();}
  if (RX_state!=RXcomplete){
    Serial.println(RX_msg[RX_state]);// Show the error if any
   // while(1);// stop
  }
  Serial.print("Enable Hopper:");
  printASCIIdata(); // Print
  
  
  cctsend(test_hopper,4,0);
  delay(100);
  while (RX_state < RXcomplete) {ccTalkReceive();}
  if (RX_state!=RXcomplete){
    Serial.println(RX_msg[RX_state]);// Show the error if any
   // while(1);// stop
  }
  Serial.print("Test Hopper:");
  printASCIIdata(); // Print the Software revision


  cctsend(request_master_inhibit_status,4,0);
  delay(100);
  while (RX_state < RXcomplete) {ccTalkReceive();}
  if (RX_state!=RXcomplete){
    Serial.println(RX_msg[RX_state]);// Show the error if any
   // while(1);// stop
  }
  Serial.print("Inhibit Status:");
  printASCIIdata(); // Print the Software revision

*/





   
  Serial.println();
  while(1);// stop
}
