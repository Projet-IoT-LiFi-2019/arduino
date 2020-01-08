// change to alter communication speed, 
// will lower values will result in faster communication
// the receiver must be tuned to the same value
#define SYMBOL_PERIOD 500 /* Defined a symbol period in us*/
#define SAMPLE_PER_SYMBOL 4

#define WORD_LENGTH 10 /* Each byte is encoded on 10-bit with start, byte, stop */
#define SYNC_SYMBOL 0xD5 /* Synchronization symbol to send after a preamble, before data communication happens */
#define ETX 0x03
#define STX 0x02
