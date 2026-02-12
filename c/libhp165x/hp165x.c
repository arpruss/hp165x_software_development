#include <stdlib.h>
#include <string.h>
#include "hp165x.h"
#define INITIAL_MARGIN 8

asm(
"_vbl_counter_code:\n"
"  addq.l #1,vblCounterValue\n"
"  jmp _original_int1_handler\n"); 

volatile uint32_t vblCounterValue = -1;
extern void _vbl_counter_code(void);
extern void _vbl_counter_jmp(void);
extern void _original_int1_handler(void);

uint16_t screenHeight = 384;

uint16_t getScreenHeight() {
	return screenHeight;
}

uint16_t getScreenWidth() {
	return SCREEN_WIDTH;
}

uint32_t getVBLCounter(void) {
	return vblCounterValue;
}

void setVBLCounter(uint32_t value) {
	vblCounterValue = value;
}

void patchInt(uint16_t level, void (*address)()) {
	volatile uint8_t* ptr = ((volatile uint8_t*)0x980000)+6*(level-1);
	*(volatile uint32_t*)(ptr+2) = (uint32_t)address;
	*(volatile uint16_t*)(ptr) = 0x4EF9;
}

void unpatchInt(uint16_t level) {
	volatile uint8_t* current = ((volatile uint8_t*)0x980000)+6*(level-1);
	volatile uint8_t* orig = ((volatile uint8_t*)_original_int1_handler)+6*(level-1);
	*(volatile uint32_t*)(current+2) = *(volatile uint32_t*)(orig+2);
	*(volatile uint16_t*)(current) = *(volatile uint16_t*)(orig);
}

void patchVBL() {
	vblCounterValue = 0;
	patchInt(INT_VBL, _vbl_counter_code);
}

void drawBlack(void) {
	*SCREEN_MEMORY_CONTROL = WRITE_BLACK;
}

void drawWhite(void) {
	*SCREEN_MEMORY_CONTROL = WRITE_WHITE;
}

void initialScreen() {
	*SCREEN_MEMORY_CONTROL = WRITE_WHITE;
	fillScreen();

	volatile uint32_t* pos;
	volatile uint32_t* pos2;

	*SCREEN_MEMORY_CONTROL = WRITE_GRAY;
	pos = (uint32_t*)SCREEN;
	pos2 = pos + (ROM_SCREEN_HEIGHT-INITIAL_MARGIN) * (ROM_SCREEN_WIDTH/8);
	for (int16_t i = (ROM_SCREEN_WIDTH/8) * INITIAL_MARGIN ; i >= 0 ; i--) {
		*pos2++ = *pos++ = 0x000F000F;
	}
	pos = (uint32_t*)SCREEN + INITIAL_MARGIN * (ROM_SCREEN_WIDTH/8);
	pos2 = pos + (592/8) - 1;
	for (int16_t y = ROM_SCREEN_HEIGHT - 2 * INITIAL_MARGIN ; y >= 0 ; y--) {
		*pos = 0x000F000F;
		*pos2 = 0x000F000F;
		pos += ROM_SCREEN_WIDTH/8;
		pos2 += ROM_SCREEN_WIDTH/8;
	}
}

void reload(void) {
//	setScreenHeight(0);
	_final_cleanup();
	initialScreen();
	asm("move.l 0x00A7FFFE, %sp"); /* switch to original stack location */
	_reload();
}

// TODO: assembly
void drawPixel(uint16_t x, uint16_t y) {
	volatile uint16_t* pos = SCREEN + y * (SCREEN_WIDTH/4) + x/4;
	*pos = 8>>(x%4);
}

// fills screen, up to 392 lines as needed
// todo: only do the number of lines that are actually
// needed
void fillScreen(void) {
asm(
	"  move.l #0x000F000F, %%d0\n"
    "  move.l %%d0,%%d1\n"
    "  move.l %%d0,%%d2\n"
    "  move.l %%d0,%%d3\n"
    "  move.l %%d0,%%d4\n"
    "  move.l %%d0,%%d5\n"
    "  move.l %%d0,%%d6\n"
    "  move.l %%d0,%%d7\n"
    "  move.l #(0x600000+(" _QUOTE(SCREEN_WIDTH*MAX_SCREEN_HEIGHT/2) ")), %%a0\n" // ; 592*392/2 is divisible by 64
    "  move.l #0x600000, %%a1\n"
	"1:\n"
	"  movem.l %%d0-%%d7,-(%%a0)\n" // ; clear 16 display words at once, decrementing A0 by 32
    "  cmp.l %%a1,%%a0\n"
	"  bge 1b\n" : : : "d2", "d3", "d4", "d5", "d6", "d7" );
}

void drawVerticalLine(uint16_t x, uint16_t y1, uint16_t y2) {
	uint16_t yCount;
	volatile uint16_t* pos;
	uint16_t mask = 8>>(x%4);
	if (y1 <= y2) {
		pos = SCREEN + (SCREEN_WIDTH/4) * y1 + (x/4);
		yCount = y2 - y1 + 1;
	}
	else {
		pos = SCREEN + (SCREEN_WIDTH/4) * y2 + (x/4);
		yCount = y1 - y2 + 1;
	}
	for (uint16_t i = 0 ; i < yCount ; i++) {
		*pos = mask;
		pos += SCREEN_WIDTH/4;
	}
}

void drawHorizontalLine(uint16_t x1, uint16_t y, uint16_t x2) {
	uint16_t xCount;
	volatile uint16_t* pos = SCREEN + (SCREEN_WIDTH/4) * y;
	uint16_t mask;
	
	if (x1 <= x2) {
		mask = 8>>(x1%4);
		pos += (x1/4);
		xCount = x2 - x1 + 1;
	}
	else {
		mask = 8>>(x2%4);
		pos += (x2/4);
		xCount = x1 - x2 + 1;
	}
	uint8_t value = 0;
	for (uint16_t i = 0 ; i < xCount ; i++) {
		value |= mask;
		mask >>= 1;
		if (mask == 0) {
			*pos++ |= value;
			mask = 8;
			value = 0;
		}
	}
	if (value)
		*pos = value;
}

		// https://stackoverflow.com/questions/66586687/delay-loop-in-68k-assembly

// the processor is supposed to be 10MHz, but my timing shows more like 8.5MHz
// 8854 per tick
static void waitSecond(void) {
	asm volatile("  move.l #(8500000/16),%%d0\n" 
		"  moveq.l #1,%%d1\n"
		"1:\n"
		"  sub.l %%d1,%%d0\n"
		"  bne.s 1b\n" : : : "d0", "d1");
}

void waitSeconds(uint16_t n) {
	for (int i=0;i<n;i++) {
		waitSecond();
	}
}

_WRAP_1(romDelayTicks,0xea06);
_WRAP_1(drawText,0xeaf6);
_WRAP_1(setTextMode,0xeb08);
_WRAP_2(setCoordinates,0xeae4);

asm(".globl delayTicks\n" 
	"delayTicks:\n\t" 
	"movem.l %d2-%d7/%a2-%a6,-(%sp)\n\t" \
	"move.l (44+4)(%sp),%d0\n\t" \
	"move.l #0x1C,%d7\n\t" \
	"trap #0\n\t" \
	"movem.l (%sp)+,%d2-%d7/%a2-%a6\n\t" \
	"rts");
		
int strncasecmp(const char* s1, const char* s2, int n) {
	while (*s1 && *s2 && n>0) {
		int c1 = *s1;
		int c2 = *s2;
		if (c1 != c2) {
			if ('a' <= c1 && c1 <= 'z') 
				c1 += 'A'-'a';
			if ('a' <= c2 && c2 <= 'z') 
				c2 += 'A'-'a';
			if (c1 != c2)
				return c1-c2;
		}
		s1++;
		s2++;
		n--;
	}
	if (n > 0)
		return *s1 - *s2; 
	return 0;
}

static uint8_t defaultLookupTable[16] = 
{
	0,2,1,1,0,
	0,2,2,2,0,
	0,0,2,2,0,0 
};

/* 
If you use this, be careful not to damage the
screen, as it can also change the mapping outside
the visible area and cause distortion. Also reset
on exit. 
*/

void setScreenLookupTable(uint8_t* table) {
/*  Write 1 to bit 7 of 202001.w
 Write the 16 entries to 204001.b, 204003.b, etc.
 Write 0 to bit 7 of 202000.w
 Keep 1 in bit 6.
*/	
	if (table == NULL)
		table = defaultLookupTable;
	*(volatile uint8_t*)0x202001 = 0x80;
	volatile uint8_t* p = (volatile uint8_t*)0x204001;
	for (uint16_t i=0; i<16; i++) {
		*p = table[i];
		p += 2;
	}
	*(volatile uint8_t*)0x202001 = 0x40;		
}

_WRAP_0_RET_D1(getKeyBIOS,0xeb38);
