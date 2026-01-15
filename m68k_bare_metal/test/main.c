#include <stdint.h>
#include <stdio.h>

#define SCREEN_MEMORY_CONTROL ((volatile uint16_t*)0x201000)
#define SCREEN ((volatile uint16_t*)0x600000)
typedef void (*DrawText_t)(char*);
#define DrawText ((DrawText_t)(0x0000eaf6))
typedef void (*SetCoordinates_t)(uint32_t,uint32_t);
#define SetCoordinates ((SetCoordinates_t)(0x0000eae4))
//typedef uint16_t (*GetKey_t)(void);
//#define GetKey ((GetKey_t)0x0000eb38)
typedef void (*Reload_t)(void);
#define Reload ((Reload_t)0x0000ece2)

void putchar_(int c){}

char alpha[] = {'a','b',0};
int beta[4];

__asm__(
".global getkey\n"
"getkey:\n"
"  jsr 0xeb38\n"
"  movew %d1,%d0\n"
"  rts");

#include "utils.h"

typedef void (*function_one_arg32_t)(uint32_t);

void modes(uint16_t m) {
	((function_one_arg32_t)0x0000eb08)(m);
}


test() {
	for (uint16_t i=0;i<256;i++) {
		SCREEN_MEMORY_CONTROL[0]=WRITE_WHITE;
		uint16_t x = (i/16)*16*2;
		uint16_t y = (i%16)*14;
		((function_one_arg32_t)0x0000eb08)(i);
		char z[9];
		sprintf(z,"%02x",(unsigned int)i);
		setCoordinates(x,y);
		drawText(z);
	}
		
}

main() {
	SCREEN_MEMORY_CONTROL[0] = WRITE_BLACK;
	for (int i=0; i<SCREEN_WIDTH*SCREEN_HEIGHT/4; i++)
		SCREEN[i] = 0xF;
	//test();
#if 1
	SCREEN_MEMORY_CONTROL[0]=WRITE_WHITE;
	for (int x=0;x<256;x++) {
		drawPixel(x,x);
		drawPixel(255-x,x);
	}
	SCREEN_MEMORY_CONTROL[0] = WRITE_BLACK;
	for (int i=SCREEN_WIDTH/4*8; i<SCREEN_WIDTH/4*16; i++)
		SCREEN[i] = 0xF;
	SCREEN_MEMORY_CONTROL[0] = WRITE_WHITE;
	for (int x=0;x<256;x++) {
		drawPixel(x,64);
		drawPixel(x,256-64);
	}
#endif	
	*LAST_KEY=0;
	while (*LAST_KEY != KEY_SELECT);
	Reload();
}