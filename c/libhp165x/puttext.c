#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stddef.h>
#include <hp165x.h>

static uint16_t foreground = WRITE_BLACK;
static uint16_t background = WRITE_WHITE;

typedef uint8_t byte;

#include "ibm8x14.c"

#define FONT_WIDTH 8
#define TEXT_COLUMNS (SCREEN_WIDTH/FONT_WIDTH)


static uint16_t curX=0;
static uint16_t curY=0;

static uint8_t* font = font8x14;
static uint16_t fontHeight = 14;
static uint16_t fontLineHeight = 14;
static uint16_t numRows = (SCREEN_HEIGHT/14);

void setFont(uint8_t* data, uint16_t height, uint16_t lineHeight) {
	uint16_t pixelY = curY * fontLineHeight;
	font = data;
	fontHeight = height;
	fontLineHeight = lineHeight;
	numRows = SCREEN_HEIGHT / lineHeight;
	curY = pixelY / lineHeight;
	if (curY >= numRows)
		curY = numRows;
}

uint16_t getFontLineHeight(void) {
	return fontLineHeight;
}

uint16_t getFontWidth(void) {
	return FONT_WIDTH;
}

uint16_t getTextRows(void) {
	return numRows;
}

uint16_t getTextColumns(void) {
	return TEXT_COLUMNS;
}

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
	volatile uint16_t* pos = SCREEN + curY * (fontLineHeight*(SCREEN_WIDTH/4)) + curX*2;
		
	while(*s) {
		uint16_t c = 0xFF & *s++;
		
		if (c == '\n' || curX >= TEXT_COLUMNS) {
			curY++;
			if (curY >= numRows)
				curY = numRows -1;
			curX = 0;
			pos = SCREEN + curY * (fontLineHeight*(SCREEN_WIDTH/4));
			if (c == '\n')
				continue;
		}
		
		uint8_t* glyph = font + c*fontHeight;
		volatile uint16_t* pos2 = pos;
		uint16_t row;
		for (row = 0; row < fontHeight; row++) {
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
		for (; row < fontLineHeight; row++) {
			*SCREEN_MEMORY_CONTROL = background;
			*pos2 = 0xF;
			pos2[1] = 0xF;
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

#if 0
static uint8_t* romFind(uint32_t value) {
	for (uint32_t* p = (uint32_t*)65536-1 ; p > (uint32_t*)256 ; p--) {
		if (*p == value)
			return (uint8_t*)p;
	}
	return NULL;
}

static void _setFontSystem(uint32_t defaultLocation, uint32_t testLocation, uint32_t testValue) {
	uint8_t* location = 0;
	if (*(uint32_t*)testLocation == testValue) {
		location = (uint8_t*)defaultLocation;
		setFont(location, 16, 16);
	}
	else {
		location = romFind(testValue);
		if (location == 0) {
			setFont(font8x14, 14, 16); 
			return;
		}
		location -= (testLocation-defaultLocation);
		setFont(location, 16, 16);
	}
}

void setFontSystem(uint8_t bold) {
	// search for fonts in ROM in case the user has a different ROM version
	// from mine; default to VGA if not found
	if (bold) {
		_setFontSystem(0xa674,0xaa84,0x63106C80); // start of B
	}
	else {
		_setFontSystem(0x9e74,0xa288,0xF884C448); // start of B
	}
}
#endif