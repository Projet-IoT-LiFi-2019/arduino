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

//#ifndef DEBUG
//#define DEBUG
//#endif

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
  Serial.println("Start of receiver program");
  init_receiver();
  init_emitter();
  Timer1.initialize(SYMBOL_PERIOD/SAMPLE_PER_SYMBOL); //1200 bauds oversampled by factor 4
  Timer1.attachInterrupt(isr);
}

char com_buffer [32] ;
void write_ack_to_buffer(unsigned char crc){
  static char * msg = "A " ; 
  static int msg_len = strlen(msg);
  memcpy(com_buffer, msg, msg_len);
  com_buffer[msg_len] = crc + '0' ;
  if(write(com_buffer, strlen(com_buffer)) < 0){
    delay(10);
  }
}

// the loop routine runs over and over again forever:
void loop() {
  static unsigned char crc_to_acknowledge, previously_acknowledged_crc = 0;
  unsigned char crc, calculated_crc;
  char * data;
  int data_size;

  previously_acknowledged_crc = crc_to_acknowledge;
  if(data_received()){
    if(add_byte_to_frame() > 0){
      data_size = get_data_size();
      crc = get_crc_from_frame();
      data = get_data();
      calculated_crc = calc_crc(data, data_size);
      if(calculated_crc == crc)
        Serial.println(data);
        crc_to_acknowledge = calculated_crc;
    }
  }

  // writing to buffer only if crc has changed, i.e. a new frame has been received
  if(previously_acknowledged_crc != crc_to_acknowledge) {
    #ifdef DEBUG
    Serial.print("Acknowledging ");
    Serial.println(crc_to_acknowledge);
    #endif
    write_ack_to_buffer(crc_to_acknowledge);
  }
}
