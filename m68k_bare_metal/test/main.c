#include <stdint.h>

#define SCREEN_MEMORY_CONTROL ((volatile uint16_t*)0x201000)
#define WRITE_WHITE 0x0002
#define WRITE_BLACK 0xFF00
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

extern uint16_t getkey(); 

main() {
	SCREEN_MEMORY_CONTROL[0] = WRITE_WHITE;
	for (int i=0; i<592*376/2; i++)
		SCREEN[i] = 0xF;
	DrawText("Hello\n world");
	SetCoordinates(10,30);
	DrawText(alpha);
	while (getkey() != 0x2001);
	Reload();
}