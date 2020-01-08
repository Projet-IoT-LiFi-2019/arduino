#include <TimerOne.h>
#include<Wire.h>
#include "emitter.h"

#define ACK_CHANNEL 9


char com_buffer [32] ;
void write_ack_to_buffer(unsigned char crc){
  com_buffer[0] = crc + '0' ;
  if(write(com_buffer, strlen(com_buffer)) < 0){
    delay(10);
  }
}

int crc_to_acknowledge = 0;
void acknowledge_message(int bytes) {
  crc_to_acknowledge = Wire.read();
  Serial.println(crc_to_acknowledge);
}

void setup() {
  Serial.begin(115200);
  init_emitter();
  Timer1.initialize(SYMBOL_PERIOD); //1200 bauds 
  Timer1.attachInterrupt(emit_half_bit);
  Wire.begin(ACK_CHANNEL); 
  Wire.onReceive(acknowledge_message); 
}

void loop() {
  write_ack_to_buffer(crc_to_acknowledge);
  delay(10);
}
