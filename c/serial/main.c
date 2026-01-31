#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stddef.h>
#include <hp165x.h>

void serialWrite(int size,char* data);
void serialMode(uint32_t regAddr,unsigned baud,unsigned parity,unsigned stopBits,unsigned bitsOptionPlus3);
void serialReceive(uint32_t unknown);
_WRAP_2(serialWrite,0xeb44);
_WRAP_5(serialMode,0xeb5c);
_WRAP_1(serialReceive,0xea06);
//In PVTEST_: func_0x0000eb5c(0x20a000,0xe,0,1,4);

void setup() {
  *(volatile uint32_t*)0x009842c8 = 0;
  *(volatile uint16_t*)0x009842ba = 0;
  *(volatile uint16_t*)0x009842da = 1;
  *(volatile uint16_t*)0x009842dc = 0;
  *(volatile uint16_t*)0x009842de = 0;
  *(volatile uint16_t*)0x009842e0 = 0;
  *(volatile uint16_t*)0x009842e2 = 0;
  *(volatile uint16_t*)0x009842e4 = 0;
  *(volatile uint16_t*)0x009842e6 = 0;
  *(volatile uint16_t*)0x009842e8 = 0xffff;
  *(volatile uint16_t*)0x009842ec = 0;
  *(volatile uint16_t*)0x009842ee = 0;
  *(volatile uint16_t*)0x009842f0 = 0;
  *(volatile uint16_t*)0x009842f2 = 0;
  *(volatile uint16_t*)0x009842fe = 0;
  *(volatile uint16_t*)0x00984302 = 0;
  *(volatile uint16_t*)0x00984300 = 0;
  *(volatile uint16_t*)0x009842d2 = 4;
  *(volatile uint16_t*)0x009842d6 = 0;
  *(volatile uint16_t*)0x009842d4 = 1;
  *(volatile uint16_t*)0x009842d8 = 0xe;
  *(volatile uint16_t*)0x009842d0 = 0; // no protocol
  *(volatile uint16_t*)0x009842fa = 1;
  *(volatile uint16_t*)0x009842fc = 1;
  *(volatile uint32_t*)0x009842c4 = 0x20a000;
}

/*
  9842bc.w: -1 to prepare, 0 when ready
  9842cc.l: address to put data
  9842c0.w: 0
  9842f4.w: 0
  9842e4.w: length wanted?
*/

char data[80]="";

int main(void) {
	setup();
	serialMode(0x20a000,0xf,0,1,4); // 0xf=192000
	while(getKey() != KEY_STOP) {
		*(volatile uint16_t*)0x9842bc = -1;
		*(volatile void**)0x9842cc = data;
		*(volatile uint16_t*)0x9842c0 = 0;
		*(volatile uint16_t*)0x9842f4 = 0;
		*(volatile uint16_t*)0x9842e4 = 16;
		memset(data,0,sizeof(data));
		*(volatile uint16_t*)0x9842bc = 0;
		serialReceive(1);
		putText(".");
		putText(data);		
	}
	reload();
}
