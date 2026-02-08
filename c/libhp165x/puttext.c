#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stddef.h>
#include <hp165x.h>

static uint16_t foreground = WRITE_BLACK;
static uint16_t background = WRITE_WHITE;

typedef uint8_t byte;

#include "ibm8x14hp.c"

static uint16_t winX=0;
static uint16_t winY=0;
static uint16_t winRightX=MAX_TEXT_COLUMNS;
static uint16_t winBottomY=SCREEN_HEIGHT/14;
static uint16_t curX=0;
static uint16_t curY=0;
static uint8_t scrollMode=1;

static uint8_t* font = (uint8_t*)font8x14;
static uint16_t fontHeight = 14;
static uint16_t maxRows = SCREEN_HEIGHT/14;
static uint8_t reverse = 0;

#define ROR4(x) ((x) << 28 | (x) >> 4)

void setScrollMode(uint8_t m) {
	scrollMode = m;
}

void getTextWindow(uint16_t* xP,uint16_t *yP,uint16_t *widthP,uint16_t *heightP) {
	*xP = winX;
	*yP = winX;
	*widthP = winRightX-winX;
	*heightP = winBottomY-winY;
}

/* if width==0 or height==0, set to maximum possible;
   if width or height is negative, use as margin */
void setTextWindow(uint16_t x,uint16_t y,int16_t width,int16_t height) {
	winX = x;
	winY = y;
	if (width > 0) {
		winRightX = x + width;
		if (winRightX > MAX_TEXT_COLUMNS)
			winRightX = MAX_TEXT_COLUMNS;
	}
	else {
		winRightX = MAX_TEXT_COLUMNS - 1 + width;
	}
	if (height > 0) {
		winBottomY = y + height;
		if (winBottomY > maxRows)
			winBottomY = maxRows;
	}
	else {
		winBottomY = maxRows - 1 + width;
	}
	if (curX < winX)
		curX = winX;
	else if (curX >= winRightX)
		curX = winRightX-1;
	if (curY < winY)
		curY = winY;
	else if (curY >= winBottomY)
		curY = winBottomY-1;
}

void setFont(uint8_t* data, uint16_t height) {
	uint16_t pixelY = curY * fontHeight;
	maxRows = SCREEN_HEIGHT / height;
	if (height != fontHeight) {
		winY = 0;
		winBottomY = maxRows-1;
	}
	font = data;
	fontHeight = height;
	curY = pixelY / height;
	if (curY >= maxRows)
		curY = maxRows-1;
}

uint16_t getFontHeight(void) {
	return fontHeight;
}

uint16_t getFontWidth(void) {
	return FONT_WIDTH;
}

uint16_t getTextRows(void) {
	return winBottomY-winY;
}

uint16_t getTextColumns(void) {
	return winRightX-winX;
}

uint16_t getTextMaxRows(void) {
	return maxRows;
}

uint16_t getTextMaxColumns(void) {
	return MAX_TEXT_COLUMNS;
}

void setTextReverse(char _reverse) {
	reverse = _reverse;
}

void setTextColors(uint16_t f, uint16_t b) {
	foreground = f;
	background = b;
}

uint16_t getTextForeground(void) {
	return foreground;
}

uint16_t getTextBackground(void) {
	return background;
}

uint16_t getTextX(void) {
	return curX-winX;
}

uint16_t getTextY(void) {
	return curY-winY;
}

void setTextXY(uint16_t x, uint16_t y) {
	curX = winX+x;
	curY = winY+y;
}

void setTextX(uint16_t x) {
	curX = winX+x;
}

void setTextY(uint16_t y) {
	curY = winY+y;
}

void highlightText(uint16_t n, uint8_t highlightState) {
	volatile uint16_t* pos = SCREEN + curY * (fontHeight*(SCREEN_WIDTH/4)) + curX*2;

	*SCREEN_MEMORY_CONTROL = highlightState ? WRITE_SET_ATTR : WRITE_CLEAR_ATTR;
	while(n--) {
		if (curX >= winRightX) {
			curY++;
			if (curY >= winBottomY)
				curY = winBottomY-1;
			curX = winX;
			pos = SCREEN + curY * (fontHeight*(SCREEN_WIDTH/4));
		}
		volatile uint16_t* pos2 = pos;
		uint16_t row;
		for (row = 0; row < fontHeight; row++) {
			*(uint32_t*)pos2 = 0x000F000F;
			pos2 += SCREEN_WIDTH/4;			
		}
		pos += 2;
		curX++;
	}
}

/* returns number of lines scrolled */
uint16_t putText(const char* s) {
	volatile uint16_t* pos = SCREEN + curY * (fontHeight*(SCREEN_WIDTH/4)) + curX*2;
	uint16_t bg;
	uint16_t fg;
	uint16_t scrolled = 0;
	
	if (reverse) {
		bg = foreground;
		fg = background;
	}
	else {
		bg = background;
		fg = foreground;
	}
		
	while(*s) {
		uint16_t c = 0xFF & *s++;
		
		if (c == '\n' || curX >= winRightX) {
			curX = winX;
			curY++;
			if (curY >= winBottomY) {
				curY = winBottomY-1;
				if (scrollMode) {
					scrollText(1);
					scrolled++;
				}
			}
			pos = SCREEN + curY * (fontHeight*(SCREEN_WIDTH/4));
			if (c == '\n')
				continue;
		}
		
		if ((uint32_t)font < 0x10000) {
			if (c & 0x80)
				c = '?';
			uint32_t* glyph = (uint32_t*)(font + c*16);
			volatile uint32_t* pos2 = (uint32_t*)pos;
			uint32_t x = *glyph++; 
			uint32_t y = x;
			
			*SCREEN_MEMORY_CONTROL = fg;
			*pos2 = x;
			x = ROR4(x);
			pos2[SCREEN_WIDTH/8] = x;
			x = ROR4(x);
			pos2[2*SCREEN_WIDTH/8] = x;
			x = ROR4(x);
			pos2[3*SCREEN_WIDTH/8] = x;

			*SCREEN_MEMORY_CONTROL = bg;
			x = ~y;
			*pos2 = x;
			x = ROR4(x);
			pos2[SCREEN_WIDTH/8] = x;
			x = ROR4(x);
			pos2[2*SCREEN_WIDTH/8] = x;
			x = ROR4(x);
			pos2[3*SCREEN_WIDTH/8] = x; 

			*SCREEN_MEMORY_CONTROL = fg;
			y = x = *glyph++;
			pos2[4*SCREEN_WIDTH/8] = x;
			x = ROR4(x);
			pos2[5*SCREEN_WIDTH/8] = x;
			x = ROR4(x);
			pos2[6*SCREEN_WIDTH/8] = x;
			x = ROR4(x);
			pos2[7*SCREEN_WIDTH/8] = x;

			*SCREEN_MEMORY_CONTROL = bg;
			x = ~y;
			pos2[4*SCREEN_WIDTH/8] = x;
			x = ROR4(x);
			pos2[5*SCREEN_WIDTH/8] = x;
			x = ROR4(x);
			pos2[6*SCREEN_WIDTH/8] = x;
			x = ROR4(x);
			pos2[7*SCREEN_WIDTH/8] = x; 

			*SCREEN_MEMORY_CONTROL = fg;
			y = x = *glyph++;
			pos2[8*SCREEN_WIDTH/8] = x;
			x = ROR4(x);
			pos2[9*SCREEN_WIDTH/8] = x;
			x = ROR4(x);
			pos2[10*SCREEN_WIDTH/8] = x;
			x = ROR4(x);
			pos2[11*SCREEN_WIDTH/8] = x;

			*SCREEN_MEMORY_CONTROL = bg;
			x = ~y;
			pos2[8*SCREEN_WIDTH/8] = x;
			x = ROR4(x);
			pos2[9*SCREEN_WIDTH/8] = x;
			x = ROR4(x);
			pos2[10*SCREEN_WIDTH/8] = x;
			x = ROR4(x);
			pos2[11*SCREEN_WIDTH/8] = x; 

			*SCREEN_MEMORY_CONTROL = fg;
			y = x = *glyph++;
			pos2[12*SCREEN_WIDTH/8] = x;
			x = ROR4(x);
			pos2[13*SCREEN_WIDTH/8] = x;
			x = ROR4(x);
			pos2[14*SCREEN_WIDTH/8] = x;
			x = ROR4(x);
			pos2[15*SCREEN_WIDTH/8] = x;

			*SCREEN_MEMORY_CONTROL = bg;
			x = ~y;
			pos2[12*SCREEN_WIDTH/8] = x;
			x = ROR4(x);
			pos2[13*SCREEN_WIDTH/8] = x;
			x = ROR4(x);
			pos2[14*SCREEN_WIDTH/8] = x;
			x = ROR4(x);
			pos2[15*SCREEN_WIDTH/8] = x; 
		}
		else if (font == (uint8_t*)font8x14) {
			uint32_t* glyph = (uint32_t*)(font + c*16);
			volatile uint32_t* pos2 = (uint32_t*)pos;
			uint32_t x = *glyph++; 
			uint32_t y = x;
			
			*SCREEN_MEMORY_CONTROL = fg;
			*pos2 = x;
			x = ROR4(x);
			pos2[SCREEN_WIDTH/8] = x;
			x = ROR4(x);
			pos2[2*SCREEN_WIDTH/8] = x;
			x = ROR4(x);
			pos2[3*SCREEN_WIDTH/8] = x;

			*SCREEN_MEMORY_CONTROL = bg;
			x = ~y;
			*pos2 = x;
			x = ROR4(x);
			pos2[SCREEN_WIDTH/8] = x;
			x = ROR4(x);
			pos2[2*SCREEN_WIDTH/8] = x;
			x = ROR4(x);
			pos2[3*SCREEN_WIDTH/8] = x; 

			*SCREEN_MEMORY_CONTROL = fg;
			y = x = *glyph++;
			pos2[4*SCREEN_WIDTH/8] = x;
			x = ROR4(x);
			pos2[5*SCREEN_WIDTH/8] = x;
			x = ROR4(x);
			pos2[6*SCREEN_WIDTH/8] = x;
			x = ROR4(x);
			pos2[7*SCREEN_WIDTH/8] = x;

			*SCREEN_MEMORY_CONTROL = bg;
			x = ~y;
			pos2[4*SCREEN_WIDTH/8] = x;
			x = ROR4(x);
			pos2[5*SCREEN_WIDTH/8] = x;
			x = ROR4(x);
			pos2[6*SCREEN_WIDTH/8] = x;
			x = ROR4(x);
			pos2[7*SCREEN_WIDTH/8] = x; 

			*SCREEN_MEMORY_CONTROL = fg;
			y = x = *glyph++;
			pos2[8*SCREEN_WIDTH/8] = x;
			x = ROR4(x);
			pos2[9*SCREEN_WIDTH/8] = x;
			x = ROR4(x);
			pos2[10*SCREEN_WIDTH/8] = x;
			x = ROR4(x);
			pos2[11*SCREEN_WIDTH/8] = x;

			*SCREEN_MEMORY_CONTROL = bg;
			x = ~y;
			pos2[8*SCREEN_WIDTH/8] = x;
			x = ROR4(x);
			pos2[9*SCREEN_WIDTH/8] = x;
			x = ROR4(x);
			pos2[10*SCREEN_WIDTH/8] = x;
			x = ROR4(x);
			pos2[11*SCREEN_WIDTH/8] = x; 

			*SCREEN_MEMORY_CONTROL = fg;
			y = x = *glyph++;
			pos2[12*SCREEN_WIDTH/8] = x;
			x = ROR4(x);
			pos2[13*SCREEN_WIDTH/8] = x;

			*SCREEN_MEMORY_CONTROL = bg;
			x = ~y;
			pos2[12*SCREEN_WIDTH/8] = x;
			x = ROR4(x);
			pos2[13*SCREEN_WIDTH/8] = x;
		}
		else {
			uint8_t* glyph = font + c*fontHeight;
			volatile uint32_t* pos2 = (volatile uint32_t*)pos;
			uint16_t row;
			for (row = 0; row < fontHeight; row++) {
				uint8_t g = *(uint8_t*)glyph++;
				*SCREEN_MEMORY_CONTROL = bg;
				uint32_t v = (uint32_t)g << 12 | g;
				*pos2 = ~v;
				*SCREEN_MEMORY_CONTROL = fg;
				*pos2 = v;
				pos2 += SCREEN_WIDTH/8;			
			}
		}
		
		pos += 2;
		curX++;
	}
	
	return scrolled;
}

void putChar(char c) {
	char s[2];
	s[1] = 0;
	s[0] = c;
	putText(s);
}

void putchar_(int c) {
	char s[2];
	s[1] = 0;
	s[0] = c;
	putText(s);
}

void scrollText(uint16_t rows) {
	uint8_t bitplanes = 0;
	for (uint8_t mask=1; mask != 0x10 ; mask <<= 1) {
		if (mask & ~foreground) {
			// bitplane active on foreground
			if ((mask & ~background) == 0) {
				// but not on background, so it might make a difference
				bitplanes |= mask;
			}
			else {
				if ( ( (mask<<8) & foreground) != ( (mask<<8) & background ) )
					bitplanes |= mask;
			}
		}
		else {
			if ((mask & ~background) != 0) {
				bitplanes |= mask;
			}
		}
	}
	scrollUp(rows*fontHeight, winX*FONT_WIDTH, winY*fontHeight, winRightX*FONT_WIDTH, winBottomY*fontHeight, 
		background, bitplanes);
}

static uint8_t* romFind(uint32_t value) {
	for (uint32_t* p = (uint32_t*)65536-1 ; p > (uint32_t*)256 ; p = (uint32_t*)((uint8_t*)p - 2)) {
		if (*p == value)
			return (uint8_t*)p;
	}
	return NULL;
}

/* TODO: adjust window */
static void _setFontSystem(uint32_t defaultLocation, uint32_t testLocation, uint32_t testValue) {
	if (*(uint32_t*)testLocation == testValue) {
		setFont((uint8_t*)defaultLocation, 16);
	}
	else {
		uint8_t* location = romFind(testValue);
		if (location == 0) {
			setFont(font8x14, 14); 
			return;
		}
		location -= (testLocation-defaultLocation);
		setFont(location, 16);
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

