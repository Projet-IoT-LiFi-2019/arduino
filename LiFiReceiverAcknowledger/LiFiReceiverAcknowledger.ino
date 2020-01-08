#include <TimerOne.h>
#include "emitter.h"

#define ACK_CHANNEL 9

char com_buffer [32] ;
void write_ack_to_buffer(int bytes){
  unsigned char crc = Wire.read(); 
  com_buffer[0] = crc + '0' ;
  if(write(com_buffer, strlen(com_buffer)) < 0){
    delay(10);
  }
}

void setup() {
  Serial.begin(115200);
  init_emitter();
  Timer1.initialize(SYMBOL_PERIOD); //1200 bauds 
  Timer1.attachInterrupt(emit_half_bit);
  Wire.begin(ACK_CHANNEL); 
  Wire.onReceive(write_ack_to_buffer); 
}

void loop() {}
