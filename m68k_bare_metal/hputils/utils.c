#include "utils.h"
#define INITIAL_MARGIN 8

uint16_t getKey(void) {
	uint16_t k = *LAST_KEY;
	*LAST_KEY = 0;
	return k;
}

void drawBlack(void) {
	*SCREEN_MEMORY_CONTROL = WRITE_BLACK;
}

void drawWhite(void) {
	*SCREEN_MEMORY_CONTROL = WRITE_WHITE;
}

void initialScreen() {
	volatile uint16_t* pos = SCREEN;
	for (unsigned y=0;y<SCREEN_HEIGHT;y++) {
		if (y<INITIAL_MARGIN || SCREEN_HEIGHT-INITIAL_MARGIN <= y) {
			*SCREEN_MEMORY_CONTROL = WRITE_BLACK;
			for (unsigned x4=0; x4<SCREEN_WIDTH/4; x4++)
				*pos++ = 0x0F;
		}
		else {
			*SCREEN_MEMORY_CONTROL = WRITE_BLACK;
			for (unsigned x4=0; x4<INITIAL_MARGIN/4; x4++)
				*pos++ = 0x0F;
			*SCREEN_MEMORY_CONTROL = WRITE_WHITE;
			for (unsigned x4=0; x4<(SCREEN_WIDTH-2*INITIAL_MARGIN)/4; x4++)
				*pos++ = 0x0F;
			*SCREEN_MEMORY_CONTROL = WRITE_BLACK;
			for (unsigned x4=0; x4<INITIAL_MARGIN/4; x4++)
				*pos++ = 0x0F;
		}
	}		
}

void reload(void) {
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
	for (uint16_t i = 0 ; i < SCREEN_WIDTH/4 * SCREEN_HEIGHT ; i++)
		SCREEN[i] = 0xF;
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

uint16_t getKeyWait() {
	*LAST_KEY = 0;
	do {
		uint16_t k = *LAST_KEY;
		if (k) {
			*LAST_KEY = 0;
			return k;
		}
	} while(1);
}

void waitSeconds(uint16_t n) {
	for (int i=0;i<2*n;i++) {
		asm("  movem.l %d0-%d1/%a0,-(%sp)\n"
			"  move.w #60606,%d0\n"
			"  move.w %d0,%d1\n"
			".wait1:\n"         
			"  add.w %d1,%d1\n"   // 8 cycles
			"  add.w %d1,%d1\n"   // 8 cycles
			"  add.w %d1,%d1\n"   // 8 cycles
			"  add.w %d1,%d1\n"   // 8 cycles
			"  add.w %d1,%d1\n"   // 8 cycles
			"  add.w %d1,%d1\n"   // 8 cycles
			"  add.w %d1,%d1\n"   // 8 cycles
			"  add.w %d1,%d1\n"   // 8 cycles
			"  add.w %d1,%d1\n"   // 8 cycles
			"  add.w %d1,%d1\n"   // 8 cycles
			"  add.w %d1,%d1\n"   // 8 cycles
			"  add.w %d1,%d1\n"   // 8 cycles
			"  add.w %d1,%d1\n"   // 8 cycles
			"  add.w %d1,%d1\n"   // 8 cycles
			"  add.w %d1,%d1\n"   // 8 cycles
			"  add.w %d1,%d1\n"   // 8 cycles
			"  dbra %d0,.wait1\n" // 4 cycles
		    "  movem.l (%sp)+,%d0-%d1/%a0");
	}
}
