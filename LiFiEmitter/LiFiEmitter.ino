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

// global communication buffer
char com_buffer [32] ;
char com_buffer_nb_bytes = 0 ;


void isr(){
  static int i = 0;
  sample_signal_edge();
  if(i == 0) emit_half_bit();
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


void write_static_message_to_buffer(){
  static char * msg = "Hello World" ; 
  static int msg_len = strlen(msg);
  static int i = 0 ;
  memcpy(com_buffer, msg, msg_len);
  com_buffer[msg_len] = i + '0' ;
  if(write(com_buffer, strlen(com_buffer)) < 0){
    delay(10);
  }else{
    i ++ ; 
    if(i > 9) i = 0 ;
  }
}


void write_serial_input_to_buffer(){
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
}

// the loop routine runs over and over again forever:
void loop() {
  static int last_frame_acknowledged = 1;
  char * ack_data;

  if(last_frame_acknowledged == 1){
    #ifdef TRANSMIT_SERIAL
    write_serial_input_to_buffer();
    #else
    write_static_message_to_buffer();
    #endif
    last_frame_acknowledged = 0;
  }
  else{
    resend_frame();
  }
  
  if(data_received()){
    if(add_byte_to_frame() > 0){
      ack_data = get_data();
      Serial.println(ack_data);
      last_frame_acknowledged = 1;
    }
  }
}
