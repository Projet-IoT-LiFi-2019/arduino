#ifndef TIMERONE_H
#define TIMERONE_H
#include <TimerOne.h>
#endif

#ifndef EMITTER_H
#define EMITTER_H
#include "emitter.h"
#endif

#ifndef RECEIVER_H
#define RECEIVER_H
#include "receiver.h"
#endif

//#define TRANSMIT_SERIAL

void isr(){
  static int i = 0;
  if(i == 0) emit_half_bit();
  else sample_signal_edge();
  i = ++i % SAMPLE_PER_SYMBOL;
}

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 115200 bits per second:
  Serial.begin(115200);
  init_emitter();
  init_receiver();
  Timer1.initialize(SYMBOL_PERIOD/SAMPLE_PER_SYMBOL); //1200 bauds for transmission, else we listen for ACK
  Timer1.attachInterrupt(isr); 
}

// the loop routine runs over and over again forever:
char * msg = "Hello World" ;
char com_buffer [32] ;
char com_buffer_nb_bytes = 0 ;
void loop() {
  #ifdef TRANSMIT_SERIAL
  if(Serial.available() && transmitter_available()){ //constructing the data frame only if transmitter is ready to transmit
    char c = Serial.read();
    com_buffer[com_buffer_nb_bytes] = c ;
    com_buffer_nb_bytes ++ ;
    if(com_buffer_nb_bytes >= 32 || c == '\n'){
      if(write(com_buffer, com_buffer_nb_bytes) < 0){
        Serial.println("Transmitter is busy");
      }else{
        com_buffer_nb_bytes = 0 ;
      }
    }
  }
  delay(10);
  #else
    static int i = 0 ;
    memcpy(com_buffer, msg, 11);
    com_buffer[11] = i + '0' ;
    if(write(com_buffer, 12) < 0){
      delay(10);
    }else{
      i ++ ; 
      if(i > 9) i = 0 ;
    }
  #endif
}
