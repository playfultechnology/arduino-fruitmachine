#ifndef cctcom_h
  #define cctcom_h
#include "Arduino.h"
#ifdef ALTSOFTCOMM
  AltSoftSerial altSerial;
  #define sbegin(baud) altSerial.begin(baud)
  #define srend() altSerial.end()
  #define savailable() (altSerial.available() > 0)
  #define sread() altSerial.read()
  #define swrite(dta) altSerial.write(dta)
#else
  #define sbegin(baud) Serial.begin(baud)
  #define srend() Serial.end()
  #define savailable() (Serial.available() > 0)
  #define sread()Serial.read()
  #define swrite(dta) Serial.write(dta)
#endif

unsigned char RX_buffer[128];// the receive buffer 
unsigned char TX_buffer[128];// the transmit buffer , not really because is used only to format the message and to compare with loopback
unsigned char RX_bytecount = 0; // used to count the bytes received

const unsigned char maxDataLength = 124; // the maximum data field length
const unsigned long interbytetimeout = 150;// the timeout for next byte in the same message 
const unsigned long answertimeout = 3000;// the answer timeout , it might be longer for some pools like eeprom writing

class milistimer
{
  public:
    void startt(int tdelay); // start the countdown with tdelay in miliseconds
    bool isready(void);// return true if timer expired
  private:
    unsigned long target; 
};
void ccTalkReceive(); //the main RX process to be called in loop 
// cctalk receive process functions , the main process should be called until RX_state = RX_idle
void ccTalk_comm_init();// opens the serial port and flush the RX buffer 
void breakrx();// flush the RX buffer - Usable from any RX state 
void clearrxerror();// flush the buffer without restarting the timer, usable only from RX error states


void startrx();// init the RX process to wait for an answer , use after sending a pool with cctsend, 
// the main process should be called until RX_state >= RXcomplete


// Send a pool command = cctalk pool byte , see headers.h
// dest = destination field 40 for bill acceptor, 2 for coin acceptor etc
// D00..D07 = data field , you dont need to fill all if the data field is shorter
// if you need longer data field then fill the next bytes in the TX_buffer[12] and higher
void cctsend(unsigned char command,unsigned char  dest,unsigned char msglength,\
unsigned char D00=0,unsigned char D01=0,unsigned char D02=0,unsigned char D03=0,
unsigned char D04=0,unsigned char D05=0,unsigned char D06=0,unsigned char D07=0);
  
// RX state machines coding
enum RX_State_ {RXidle,RXloop,RXansw,RXcomplete,RXflush,RXerr_unexpected_byte_in_idle,RXerr_no_loopback,RXerr_wrong_loopback,RXerr_answer_timeout,\
      RXerr_msg_length,RXerr_checksum_failed} RX_state;
// Text definitions for RX states usefull for debugging 
const char* RX_msg[] = {"RXidle","RXloop","RXansw","RXcomplete","RXflush","RXerr_unexpected_byte_in_idle","RXerr_no_loopback","RXerr_wrong_loopback","RXerr_answer_timeout",\
      "RXerr_msg_length","RXerr_checksum_failed"};
#endif
