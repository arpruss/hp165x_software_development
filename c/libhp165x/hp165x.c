#include <stdlib.h>
#include <string.h>
#include "hp165x.h"
#define INITIAL_MARGIN 8

asm(
"_vbl_counter_code:\n"
"  addq.l #1,vblCounterValue\n"
"  jmp _original_int1_handler\n"); 

volatile uint32_t vblCounterValue = 0;
extern void _vbl_counter_code(void);
extern void _vbl_counter_jmp(void);
extern void _original_int1_handler(void);

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
	patchInt(INT_VBL, _vbl_counter_code);
}

void drawBlack(void) {
	*SCREEN_MEMORY_CONTROL = WRITE_BLACK;
}

void drawWhite(void) {
	*SCREEN_MEMORY_CONTROL = WRITE_WHITE;
}

void initialScreen() {
	volatile uint32_t* pos;
	volatile uint32_t* pos2;

	*SCREEN_MEMORY_CONTROL = WRITE_WHITE;
	fillScreen();

	*SCREEN_MEMORY_CONTROL = WRITE_BLACK;
	pos = (uint32_t*)SCREEN;
	pos2 = pos + (SCREEN_HEIGHT-INITIAL_MARGIN) * (SCREEN_WIDTH/8);
	for (int16_t i = (SCREEN_WIDTH/8) * INITIAL_MARGIN ; i >= 0 ; i--) {
		*pos2++ = *pos++ = 0x000F000F;
	}
	pos = (uint32_t*)SCREEN + INITIAL_MARGIN * (SCREEN_WIDTH/8);
	pos2 = pos + (SCREEN_WIDTH/8) - 1;
	for (int16_t y = SCREEN_HEIGHT - 2 * INITIAL_MARGIN ; y >= 0 ; y--) {
		*pos = 0x000F000F;
		*pos2 = 0x000F000F;
		pos += SCREEN_WIDTH/8;
		pos2 += SCREEN_WIDTH/8;
	}
}

void reload(void) {
	_final_cleanup();
	initialScreen();
	_reload();
}

// TODO: assembly
void drawPixel(uint16_t x, uint16_t y) {
	volatile uint16_t* pos = SCREEN + y * (SCREEN_WIDTH/4) + x/4;
	*pos = 8>>(x%4);
}

// TODO: assembly
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
    "  move.l #(0x600000+(592*384/2)), %%a0\n" // ; 592*384/2 is divisible by 64
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
		
