#include "utils.h"

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

// TODO: assembly
void drawPixel(uint16_t x, uint16_t y) {
	volatile uint16_t* pos = SCREEN + y * (SCREEN_BUFFER_WIDTH/4) + x/4;
	*pos = 8>>(x%4);
}

// TODO: assembly
void fillScreen(void) {
	for (uint16_t i = 0 ; i < SCREEN_BUFFER_WIDTH/4 * SCREEN_HEIGHT ; i++)
		SCREEN[i] = 0xF;
}