#include "Arduino.h"
#include "cctcom.h"
#include "headers.h"
// 
void cctsend(unsigned char command, unsigned char  dest, unsigned char msglength, unsigned char D00,unsigned char D01,unsigned char D02,unsigned char D03, unsigned char D04,unsigned char D05,unsigned char D06,unsigned char D07) {
  unsigned char temp = 0;
  TX_buffer[0] = dest;
  TX_buffer[1] = msglength;
  TX_buffer[2] = 1; // implicit source , only for 8 bit checksum
  TX_buffer[3] = command;
  TX_buffer[4] = D00;TX_buffer[5] = D01;TX_buffer[6] = D02;TX_buffer[7] = D03;
  TX_buffer[8] = D04;TX_buffer[9] = D05;TX_buffer[10] = D06;TX_buffer[11] = D07;
  TX_buffer[msglength+4]=0;
  for (temp=0 ; temp<msglength+4; temp++) { // a simple checksum and send the message
    TX_buffer[msglength+4]-=TX_buffer[temp];
    swrite(TX_buffer[temp]);
  }
  swrite(TX_buffer[msglength+4]); // and the simple checksum
  startrx(); // Start the receiving the answer 
}

// The millistimer class used for delays
void milistimer::startt(int tdelay) {// start the timer witth delay in miliseconds. 
  unsigned long temp;
  temp = millis();
  target = temp + tdelay;
};
bool milistimer::isready(void) {//return true if timer is expired 
  unsigned long temp;
  temp = millis();
  if (target - temp > 86400000) // one day
    return true;
  else
    return false;
};
milistimer comt; // the timer needed for RX process delays

void ccTalk_comm_init(){// init the serial port and flush RX buffer
  sbegin(9600);
  breakrx();
}

void breakrx(){ // stop transmission right away and flush the buffer
                // the transmission buffer might still send the remaining bytes                
                // cctalkreceive job will put the RX_state to RX_idle when ready
  RX_state = RXflush;
  comt.startt(answertimeout);
}

void clearrxerror(){
  RX_state = RXflush;// does not set the timer since it was allready set when the state was changed to error.
}

void startrx(){// put RX to wait for an answer . To use after cctsend only
          RX_state = RXloop;
          comt.startt(interbytetimeout);
          RX_bytecount = 0;
}

void ccTalkReceive() {// the main process , should be called often enough. 
  unsigned char temprx;
  do {// at least one pass even there is no char in the buffer
    // as many passes as bytes in the receive buffer
    switch (RX_state) {
      case RXidle: { // If a character was received then change the status to flush;
          if (savailable()) {
            RX_state = RXflush;
            comt.startt(answertimeout);//set the flush timer
            temprx = sread();//blind read the received byte 
          }
          break;
        }
      case RXloop: {
          if (savailable()) { // ignore the timeout if there is a char on queue
            temprx = sread();
            RX_buffer[RX_bytecount] = temprx;
            
            if (RX_buffer[RX_bytecount] != TX_buffer[RX_bytecount]) { //error , wrong loopback
              RX_state = RXerr_wrong_loopback;
              comt.startt(answertimeout);
              break;
            } else {                                               // no loopback error
            
              RX_bytecount++;
              if (RX_bytecount > (RX_buffer[1] + 4)) { //loopback ready, prepare for the answer
                RX_bytecount = 0;// reset the byte counter
                RX_state = RXansw;//
                comt.startt(answertimeout);// set the timeout for the answer
              } else {                             //loopback in progress
                comt.startt(interbytetimeout);//set the timeout for the next byte
              }
            }
          } else {
            if (comt.isready()) { // timeout
              RX_state = RXerr_no_loopback;// Error
              comt.startt(answertimeout);
            }
          }
          break;
        }
      case RXansw: {
          if (savailable()) { // ignore the timeout if there is a char on queue
            temprx = sread();
            RX_buffer[RX_bytecount] = temprx;
            // skip source and destination check leaving them for later
            if ((RX_bytecount >= 1) && (RX_buffer[1] > maxDataLength)) { // check message legth
              RX_state = RXerr_msg_length;
              comt.startt(answertimeout);
              break;
            }
            RX_bytecount++;
            if (RX_bytecount > (RX_buffer[1] + 4)) { //message complete
              unsigned char checksum = 0;
              // do checksum
              for (RX_bytecount = 0; RX_bytecount++ ; RX_bytecount <= RX_buffer[1] + 4) {
                checksum = RX_buffer[RX_bytecount];
              }
              if (checksum != 0) { //checksum error
                RX_state = RXerr_checksum_failed;
                comt.startt(answertimeout);
                break;
              } else {// successfull
                RX_state = RXcomplete;// no timeout needed , a message can be sent right away
                break;
              }
            };
          } else {
            if (comt.isready()) { // timeout
              RX_state = RXerr_answer_timeout;// Error
              if (RX_bytecount > 0){//no reason to set the timeout if allready was a timeout
                comt.startt(answertimeout); 
              }
            }
          }
          break;
        }
      case RXcomplete: {
          //do nothing, stay here until the main program does something
          // just ignore all other bytes received if a complete correct answer was received
          // they will be handled when going to Idle state
          break;
        }
      case RXflush: {
          if (savailable()) { //read all bytes until answer timeout
            temprx = sread();
            comt.startt(answertimeout);
          }
          if (comt.isready()) { // timeout
            RX_state = RXidle;// Error
          }
          break;
        }
      default: {// here are all errors
        // nothing can be done here we only keep an eye on the break length
          if (savailable()) { //read all bytes, the timer is set where the error is found.
            temprx = sread();// 
            comt.startt(answertimeout);//restart the timer if a character is received
          }
        }
    }
  } while (savailable());
}
