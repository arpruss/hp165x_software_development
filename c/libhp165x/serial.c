#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stddef.h>
#include <hp165x.h>

void _serialInit(void);
_WRAP_0(_serialInit,0xeb50);
_WRAP_5(_serialMode,0xeb5c);
_WRAP_2(serialWrite,0xeb44);

void serialSetup(uint8_t baud, uint8_t parity, uint8_t stopBits, uint8_t dataBits, uint8_t protocol) {
	_serialInit();
	_serialMode(SERIAL_BASE,baud,parity,stopBits,dataBits);
	*SERIAL_COMMAND = 0x26; // SERIAL_COMMAND_DTR;
	*(volatile uint8_t*)0x009842d1 = protocol;
}


/*
  9842bc.w: -1 to prepare, 0 when ready
  9842cc.l: address to put data
  9842c0.w: 0
  9842f4.w: 0
  9842e4.w: length wanted?
*/


/* receive up to specified buffer size */
/* TODO: things fall behind if the size is exceeded */
uint16_t serialReceiveNoWait(uint16_t size, volatile char* p) {
	if (size == 0)
		return 0;
	*(volatile uint16_t*)0x9842bc = -1;
	*(volatile uint16_t*)0x9842c0 = size-1;
	*(volatile void**)0x9842cc = p;
//	while(1) {
		*(volatile uint16_t*)0x9842f4 = 0;
		*(volatile uint16_t*)0x9842e4 = 1;
		
		// something about protocol, not sure it's needed
		/* if (*(volatile uint16_t*)0x9842d0 == 0) 
			*(volatile uint16_t *)(0x009842c4 + 6) &= 0x20;
		if ((1&*(volatile uint16_t *)(0x009842c4 + 6)) == 0) 
			*(volatile uint16_t *)(0x009842c4 + 6) |= 1; */
		
//		while (*(volatile uint16_t*)0x9842f4 == 0) { // optional: go until buffer full
			romDelayTicks(0);
//			//romDelayTicks(0);
//			serialReceive(1,1,2,buf2);
//		}
//		if (*(volatile uint16_t*)0x09842bc != 0xFFFF)
//		*(volatile uint16_t*)0x9842e4 = 0;
			return 1+*(volatile uint16_t*)0x09842bc;
//	}
}

/*
void receive1() {
		memset(data,0,sizeof(buffer));
		*(volatile uint16_t*)0x9842bc = -1;
		*(volatile void**)0x9842cc = buffer;
		*(volatile uint16_t*)0x9842c0 = 0; // size-1?
		*(volatile uint16_t*)0x9842f4 = 0;
		*(volatile uint16_t*)0x9842e4 = 1;
		while (*(volatile uint16_t*)0x9842f4 == 0) { // could wait for 9842e4 to be 0
			serialReceive(0);
		}
		printf("[%d]",*(volatile uint16_t*)0x9842f4);
		putText(":");
		putText(data);
} */

void serialWriteNoWait(uint16_t size, char* data) {
	*(volatile uint16_t*)0x9842f8 = 0; // serial write done
	*(volatile uint16_t*)0x9842c2 = size-1;
	*(volatile uint16_t*)0x9842be = 0xFFFF; // serial write index
	*(volatile uint32_t*)0x9842c8 = (uint32_t)data;	
}


