#ifndef _SERIAL_H
#include <hp165x.h>

#define _SERIAL_H

#define SERIAL_COMMAND ((volatile uint8_t*)0x20a007)
#define SERIAL_STATUS  ((volatile uint8_t*)0x20a003)
#define SERIAL_MODE    ((volatile uint8_t*)0x20a005)
#define SERIAL_DATA    ((volatile uint8_t*)0x20a001)
#define SERIAL_BASE    ((volatile uint16_t*)0x20a000)

#define SERIAL_COMMAND_TRANSMIT 1
#define SERIAL_COMMAND_DTR 		2
#define SERIAL_COMMAND_RECEIVE  4

#define STOP_BITS_1          0
#define STOP_BITS_1_AND_HALF 1
#define STOP_BITS_2			 2

#define PARITY_NONE 0
#define PARITY_ODD  1
#define PARITY_EVEN 2

#define DATA_BITS_7 3
#define DATA_BITS_8 4

#define BAUD_110 2
#define BAUD_300 6
#define BAUD_1200 9
#define BAUD_2400 0xC
#define BAUD_4800 0xD
#define BAUD_9600 0xE
#define BAUD_19200 0xF
#define BAUD_MAX   BAUD_19200

#define PROTOCOL_HARDWARE 0
#define PROTOCOL_NONE	  1
#define PROTOCOL_SOFTWARE 2

void _serialInit(void);
void _serialMode(volatile void* base, uint32_t baud, uint32_t parity, uint32_t stopBits, uint32_t dataBits);
void serialSetup(uint8_t baud, uint8_t parity, uint8_t stopBits, uint8_t dataBits, uint8_t protocol);
void serialWrite(unsigned size,char* data);
uint16_t serialReceiveNoWait(uint16_t size, volatile char* p);

#endif