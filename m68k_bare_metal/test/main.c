#include <stdint.h>

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

char alpha[] = {'a','b',0};
int beta[4];

__asm__(
".global getkey\n"
"getkey:\n"
"  jsr 0xeb38\n"
"  movew %d1,%d0\n"
"  rts");

#include "utils.h"

main() {
	SCREEN_MEMORY_CONTROL[0] = WRITE_BLACK;
	for (int i=0; i<SCREEN_WIDTH*SCREEN_HEIGHT/4; i++)
		SCREEN[i] = 0xF;
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
	*LAST_KEY=0;
	while (*LAST_KEY != KEY_SELECT);
	Reload();
}