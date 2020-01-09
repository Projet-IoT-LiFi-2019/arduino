#include <TimerOne.h>
#include "receiver.h"
#include<Wire.h>

#define ACK_CHANNEL 9

//#define DEBUG

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 115200 bits per second:
  Serial.begin(115200);
  Serial.println("Start of receiver program");
  init_receiver();
  Wire.begin(); 
  Timer1.initialize(SYMBOL_PERIOD/SAMPLE_PER_SYMBOL); //1200 bauds oversampled by factor 4
  Timer1.attachInterrupt(sample_signal_edge);
}

// the loop routine runs over and over again forever:
void loop() {
  unsigned char crc, calculated_crc, acknowledged_crc;
  char * data;
  int data_size;

  if(data_received()){
    if(add_byte_to_frame() > 0){
      data_size = get_data_size();
      crc = get_crc_from_frame();
      data = get_data();
      calculated_crc = calc_crc(data, data_size);
      if(calculated_crc == crc){
        acknowledged_crc = atoi(data);
        #ifdef DEBUG
        Serial.println(acknowledged_crc);
        #endif
        Wire.beginTransmission(ACK_CHANNEL); // transmit to device ACK_CHANNEL
        Wire.write(acknowledged_crc);
        Wire.endTransmission();
      }  
    }
  }
}
