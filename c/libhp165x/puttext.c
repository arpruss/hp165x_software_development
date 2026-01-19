#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stddef.h>
#include <hp165x.h>

static uint16_t foreground = WRITE_BLACK;
static uint16_t background = WRITE_WHITE;

typedef uint8_t byte;

#include "ibm8x14.c"
#define FONT_HEIGHT 14
#define FONT_WIDTH 8

#define NUM_ROWS (SCREEN_HEIGHT/FONT_HEIGHT)
#define NUM_COLS (SCREEN_WIDTH/FONT_WIDTH)

static uint16_t curX=0;
static uint16_t curY=0;

void setTextBlackOnWhite(char value) {
	if (value) {
		foreground = WRITE_BLACK;
		background = WRITE_WHITE;
	}
	else {
		background = WRITE_BLACK;
		foreground = WRITE_WHITE;
	}
}

uint16_t getTextX(void) {
	return curX;
}

uint16_t getTextY(void) {
	return curY;
}

void setTextXY(uint16_t x, uint16_t y) {
	curX = x;
	curY = y;
}

void setTextX(uint16_t x) {
	curX = x;
}

void setTextY(uint16_t x) {
	curX = x;
}

void putText(char* s) {
	volatile uint16_t* pos = SCREEN + curY * (14*(SCREEN_WIDTH/4)) + curX*2;
		
	while(*s) {
		uint16_t c = 0xFF & *s++;
		
		if (c == '\n' || curX >= NUM_COLS) {
			curY++;
			if (curY >= NUM_ROWS)
				curY = NUM_ROWS -1;
			curX = 0;
			pos = SCREEN + curY * (14*(SCREEN_WIDTH/4));
			if (c == '\n')
				continue;
		}
		
		uint8_t* glyph = font8x14 + c*14;
		volatile uint16_t* pos2 = pos;
		for (uint16_t row = 0; row < 14; row++) {
			uint8_t g = *glyph;
			*SCREEN_MEMORY_CONTROL = background;
			*pos2 = (~g)>>4;
			pos2[1] = (~g)&0xF;
			*SCREEN_MEMORY_CONTROL = foreground;
			*pos2 = g>>4;
			pos2[1] = g&0xF;
			glyph++;
			pos2 += SCREEN_WIDTH/4;			
		}
		pos += 2;
		curX++;
	}
}

void putchar_(int c) {
	char s[2];
	s[1] = 0;
	s[0] = c;
	putText(s);
}
