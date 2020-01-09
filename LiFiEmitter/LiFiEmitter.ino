#include <TimerOne.h>
#include<Wire.h>
#include "emitter.h"

//#define TRANSMIT_SERIAL

#define TRUE 1
#define FALSE 0

#define ACK_CHANNEL 9

// global communication buffer
char com_buffer [32] ;
char com_buffer_nb_bytes = 0 ;
int last_frame_acknowledged = TRUE;


void message_acknowledged(int bytes) {
  unsigned char acknowledged_crc = Wire.read();
  unsigned char last_frame_crc = get_last_frame_crc();
  if(acknowledged_crc == last_frame_crc){
    last_frame_acknowledged = TRUE;
    #ifdef TRANSMIT_SERIAL
    com_buffer_nb_bytes = 0 ;
    #endif
    Serial.print("ACK ");
    Serial.println(acknowledged_crc);
  } else {
    Serial.print("Wrong ACK - received ");
    Serial.print(acknowledged_crc);
    Serial.print(" and sent ");
    Serial.println(last_frame_crc);
  }
}


// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 115200 bits per second:
  Serial.begin(115200);
  init_emitter();
  Timer1.initialize(SYMBOL_PERIOD); //1200 bauds for transmission
  Timer1.attachInterrupt(emit_half_bit);
  Wire.begin(ACK_CHANNEL); 
  Wire.onReceive(message_acknowledged);  
}


void write_static_message_to_buffer(){
  static char * msg = "Hello World" ; 
  static int msg_len = strlen(msg);
  static int i = 0 ;
  memcpy(com_buffer, msg, msg_len);
  com_buffer[msg_len] = i + '0' ;
  if(write(com_buffer, strlen(com_buffer)) < 0){
    delay(10);
  }else if(last_frame_acknowledged == TRUE){
    i ++ ; 
    if(i > 9) i = 0 ;
    last_frame_acknowledged = FALSE;
  }
}


void write_serial_input_to_buffer(){
  static char c;
  if(Serial.available() && transmitter_available()){ //constructing the data frame only if transmitter is ready to transmit
    if(last_frame_acknowledged == TRUE){
      c = Serial.read();
      com_buffer[com_buffer_nb_bytes] = c ;
      com_buffer_nb_bytes ++ ;
    }
    if(com_buffer_nb_bytes >= 32 || c == '\n'){
      last_frame_acknowledged == FALSE;
      if(write(com_buffer, com_buffer_nb_bytes) < 0){
        delay(10);
      } else {
        Serial.println("> Message sent.");
      }
    }
  }
}

// the loop routine runs over and over again forever:
void loop() {
  #ifdef TRANSMIT_SERIAL
  write_serial_input_to_buffer();
  #else
  write_static_message_to_buffer();
  #endif
  delay(10);
}
