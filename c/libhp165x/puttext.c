#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stddef.h>
#include <hp165x.h>

static uint16_t foreground = WRITE_BLACK;
static uint16_t background = WRITE_WHITE;
static uint8_t scrollBitplanes = 1;

typedef uint8_t byte;

#include "ibm8x14hp.c"

static uint16_t winX=0;
static uint16_t winY=0;
static uint16_t winRightX=MAX_TEXT_COLUMNS;
static uint16_t winBottomY=DEFAULT_SCREEN_HEIGHT/14;
static uint16_t curX=0;
static uint16_t curY=0;
static uint8_t scrollMode=1;

static uint8_t* font = (uint8_t*)font8x14;
static uint16_t fontHeight = 14;
static uint16_t maxRows = DEFAULT_SCREEN_HEIGHT/14;
static uint8_t reverse = 0;

#define ROR4(x) ((x) << 28 | (x) >> 4)

void setScrollMode(uint8_t m) {
	scrollMode = m;
}

void getTextWindow(uint16_t* xP,uint16_t *yP,uint16_t *x2P,uint16_t *y2P) {
	*xP = winX;
	*yP = winX;
	*x2P = winRightX;
	*y2P = winBottomY;
}

/* if bottomRightX==0 or bottomRightY==0, set to maximum possible; if negative,
   add to maximum (i.e., specify margin) */
void setTextWindow(uint16_t topLeftX,uint16_t topLeftY,int16_t bottomRightX,int16_t bottomRightY) {
	maxRows = screenHeight / fontHeight;
	winX = topLeftX;
	winY = topLeftY;
	if (bottomRightX <= 0)
		bottomRightX += MAX_TEXT_COLUMNS;
	if (bottomRightY <= 0)
		bottomRightY += maxRows;
	winRightX = bottomRightX;
	winBottomY = bottomRightY;

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

void setTextScrollBitplanes(uint8_t b) {
	scrollBitplanes = b;
}

void setTextColors(uint16_t f, uint16_t b) {
	foreground = f;
	background = b;

	scrollBitplanes = 0;
	for (uint8_t mask=1; mask != 0x10 ; mask <<= 1) {
		if (mask & ~foreground) {
			// bitplane active on foreground
			if ((mask & ~background) == 0) {
				// but not on background, so it might make a difference
				scrollBitplanes |= mask;
			}
			else {
				if ( ( (mask<<8) & foreground) != ( (mask<<8) & background ) )
					scrollBitplanes |= mask;
			}
		}
		else {
			if ((mask & ~background) != 0) {
				scrollBitplanes |= mask;
			}
		}
	}
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
uint16_t putTextN(const char* s, uint16_t n) {
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
		
	while(*s && n) {
		uint16_t c = 0xFF & *s++;
		n--;
		
		if (c == '\n' || curX >= winRightX) {
			curX = winX;
			curY++;
			if (curY >= winBottomY) {
				curY = winBottomY-1;
				if (scrollMode) {
					scrollTextUp(1);
					scrolled++;
				}
			}
			pos = SCREEN + curY * (fontHeight*(SCREEN_WIDTH/4)) + curX * 2;
			if (c == '\n')
				continue;
		}
		
		volatile uint32_t* pos2 = (volatile uint32_t*)pos;
		
		if ((uint32_t)font < 0x10000) {
			if (c & 0x80)
				c = '?';
			uint32_t* glyph = (uint32_t*)(font + c*16);
	
			uint32_t x;
			uint32_t y;
			
			asm volatile(
			"  move.w %[fg], 0x201000\n"
			"  move.l (%[glyph])+, %[x]\n"
			"  move.l %[x], %[y]\n"
			"  move.l %[x], (%[pos2])\n"
			"  ror.l #4, %[x]\n"
			"  move.l %[x], (" _QUOTE(SCREEN_WIDTH) "/2)(%[pos2])\n"
			"  ror.l #4, %[x]\n"
			"  move.l %[x], (2*" _QUOTE(SCREEN_WIDTH) "/2)(%[pos2])\n"
			"  ror.l #4, %[x]\n"
			"  move.l %[x], (3*" _QUOTE(SCREEN_WIDTH) "/2)(%[pos2])\n"
			
			"  move.w %[bg], 0x201000\n"
			"  not.l  %[y]\n"
			"  move.l %[y], (%[pos2])\n"
			"  ror.l #4, %[y]\n"
			"  move.l %[y], (" _QUOTE(SCREEN_WIDTH) "/2)(%[pos2])\n"
			"  ror.l #4, %[y]\n"
			"  move.l %[y], (2*" _QUOTE(SCREEN_WIDTH) "/2)(%[pos2])\n"
			"  ror.l #4, %[y]\n"
			"  move.l %[y], (3*" _QUOTE(SCREEN_WIDTH) "/2)(%[pos2])\n"
			
			"  move.l (%[glyph])+, %[x]\n"
			"  move.l %[x], %[y]\n"
			"  not.l  %[y]\n"
			"  move.l %[y], (4*" _QUOTE(SCREEN_WIDTH) "/2)(%[pos2])\n"
			"  ror.l #4, %[y]\n"
			"  move.l %[y], (5*" _QUOTE(SCREEN_WIDTH) "/2)(%[pos2])\n"
			"  ror.l #4, %[y]\n"
			"  move.l %[y], (6*" _QUOTE(SCREEN_WIDTH) "/2)(%[pos2])\n"
			"  ror.l #4, %[y]\n"
			"  move.l %[y], (7*" _QUOTE(SCREEN_WIDTH) "/2)(%[pos2])\n"

			"  move.w %[fg], 0x201000\n"
			"  move.l %[x], (4*" _QUOTE(SCREEN_WIDTH) "/2)(%[pos2])\n"
			"  ror.l #4, %[x]\n"
			"  move.l %[x], (5*" _QUOTE(SCREEN_WIDTH) "/2)(%[pos2])\n"
			"  ror.l #4, %[x]\n"
			"  move.l %[x], (6*" _QUOTE(SCREEN_WIDTH) "/2)(%[pos2])\n"
			"  ror.l #4, %[x]\n"
			"  move.l %[x], (7*" _QUOTE(SCREEN_WIDTH) "/2)(%[pos2])\n"

			"  move.l (%[glyph])+, %[x]\n"
			"  move.l %[x], %[y]\n"
			"  move.l %[x], (8*" _QUOTE(SCREEN_WIDTH) "/2)(%[pos2])\n"
			"  ror.l #4, %[x]\n"
			"  move.l %[x], (9*" _QUOTE(SCREEN_WIDTH) "/2)(%[pos2])\n"
			"  ror.l #4, %[x]\n"
			"  move.l %[x], (10*" _QUOTE(SCREEN_WIDTH) "/2)(%[pos2])\n"
			"  ror.l #4, %[x]\n"
			"  move.l %[x], (11*" _QUOTE(SCREEN_WIDTH) "/2)(%[pos2])\n"

			"  move.w %[bg], 0x201000\n"
			"  not.l  %[y]\n"
			"  move.l %[y], (8*" _QUOTE(SCREEN_WIDTH) "/2)(%[pos2])\n"
			"  ror.l #4, %[y]\n"
			"  move.l %[y], (9*" _QUOTE(SCREEN_WIDTH) "/2)(%[pos2])\n"
			"  ror.l #4, %[y]\n"
			"  move.l %[y], (10*" _QUOTE(SCREEN_WIDTH) "/2)(%[pos2])\n"
			"  ror.l #4, %[y]\n"
			"  move.l %[y], (11*" _QUOTE(SCREEN_WIDTH) "/2)(%[pos2])\n"

			"  move.l (%[glyph]), %[x]\n"
			"  move.l %[x], %[y]\n"
			"  not.l  %[y]\n"
			"  move.l %[y], (12*" _QUOTE(SCREEN_WIDTH) "/2)(%[pos2])\n"
			"  ror.l #4, %[y]\n"
			"  move.l %[y], (13*" _QUOTE(SCREEN_WIDTH) "/2)(%[pos2])\n"
			"  ror.l #4, %[y]\n"
			"  move.l %[y], (14*" _QUOTE(SCREEN_WIDTH) "/2)(%[pos2])\n"

			"  move.w %[fg], 0x201000\n"
			"  move.l %[x], (12*" _QUOTE(SCREEN_WIDTH) "/2)(%[pos2])\n"
			"  ror.l #4, %[x]\n"
			"  move.l %[x], (13*" _QUOTE(SCREEN_WIDTH) "/2)(%[pos2])\n"
			"  ror.l #4, %[x]\n"
			"  move.l %[x], (14*" _QUOTE(SCREEN_WIDTH) "/2)(%[pos2])\n"

			: [x] "=&r" (x),
			  [y] "=&r" (y)
			: [fg] "r" (fg), 
			  [bg] "r" (bg),
			  [pos2] "a" (pos2),
			  [glyph] "a" (glyph));
		}
		else if (font == (uint8_t*)font8x14) {
			uint32_t* glyph = (uint32_t*)(font + c*16);
	
			uint32_t x;
			uint32_t y;
			
			asm volatile(
			"  move.w %[fg], 0x201000\n"
			"  move.l (%[glyph])+, %[x]\n"
			"  move.l %[x], %[y]\n"
			"  move.l %[x], (%[pos2])\n"
			"  ror.l #4, %[x]\n"
			"  move.l %[x], (" _QUOTE(SCREEN_WIDTH) "/2)(%[pos2])\n"
			"  ror.l #4, %[x]\n"
			"  move.l %[x], (2*" _QUOTE(SCREEN_WIDTH) "/2)(%[pos2])\n"
			"  ror.l #4, %[x]\n"
			"  move.l %[x], (3*" _QUOTE(SCREEN_WIDTH) "/2)(%[pos2])\n"
			
			"  move.w %[bg], 0x201000\n"
			"  not.l  %[y]\n"
			"  move.l %[y], (%[pos2])\n"
			"  ror.l #4, %[y]\n"
			"  move.l %[y], (" _QUOTE(SCREEN_WIDTH) "/2)(%[pos2])\n"
			"  ror.l #4, %[y]\n"
			"  move.l %[y], (2*" _QUOTE(SCREEN_WIDTH) "/2)(%[pos2])\n"
			"  ror.l #4, %[y]\n"
			"  move.l %[y], (3*" _QUOTE(SCREEN_WIDTH) "/2)(%[pos2])\n"
			
			"  move.l (%[glyph])+, %[x]\n"
			"  move.l %[x], %[y]\n"
			"  not.l  %[y]\n"
			"  move.l %[y], (4*" _QUOTE(SCREEN_WIDTH) "/2)(%[pos2])\n"
			"  ror.l #4, %[y]\n"
			"  move.l %[y], (5*" _QUOTE(SCREEN_WIDTH) "/2)(%[pos2])\n"
			"  ror.l #4, %[y]\n"
			"  move.l %[y], (6*" _QUOTE(SCREEN_WIDTH) "/2)(%[pos2])\n"
			"  ror.l #4, %[y]\n"
			"  move.l %[y], (7*" _QUOTE(SCREEN_WIDTH) "/2)(%[pos2])\n"

			"  move.w %[fg], 0x201000\n"
			"  move.l %[x], (4*" _QUOTE(SCREEN_WIDTH) "/2)(%[pos2])\n"
			"  ror.l #4, %[x]\n"
			"  move.l %[x], (5*" _QUOTE(SCREEN_WIDTH) "/2)(%[pos2])\n"
			"  ror.l #4, %[x]\n"
			"  move.l %[x], (6*" _QUOTE(SCREEN_WIDTH) "/2)(%[pos2])\n"
			"  ror.l #4, %[x]\n"
			"  move.l %[x], (7*" _QUOTE(SCREEN_WIDTH) "/2)(%[pos2])\n"

			"  move.l (%[glyph])+, %[x]\n"
			"  move.l %[x], %[y]\n"
			"  move.l %[x], (8*" _QUOTE(SCREEN_WIDTH) "/2)(%[pos2])\n"
			"  ror.l #4, %[x]\n"
			"  move.l %[x], (9*" _QUOTE(SCREEN_WIDTH) "/2)(%[pos2])\n"
			"  ror.l #4, %[x]\n"
			"  move.l %[x], (10*" _QUOTE(SCREEN_WIDTH) "/2)(%[pos2])\n"
			"  ror.l #4, %[x]\n"
			"  move.l %[x], (11*" _QUOTE(SCREEN_WIDTH) "/2)(%[pos2])\n"

			"  move.w %[bg], 0x201000\n"
			"  not.l  %[y]\n"
			"  move.l %[y], (8*" _QUOTE(SCREEN_WIDTH) "/2)(%[pos2])\n"
			"  ror.l #4, %[y]\n"
			"  move.l %[y], (9*" _QUOTE(SCREEN_WIDTH) "/2)(%[pos2])\n"
			"  ror.l #4, %[y]\n"
			"  move.l %[y], (10*" _QUOTE(SCREEN_WIDTH) "/2)(%[pos2])\n"
			"  ror.l #4, %[y]\n"
			"  move.l %[y], (11*" _QUOTE(SCREEN_WIDTH) "/2)(%[pos2])\n"

			"  move.l (%[glyph]), %[x]\n"
			"  move.l %[x], %[y]\n"
			"  not.l  %[y]\n"
			"  move.l %[y], (12*" _QUOTE(SCREEN_WIDTH) "/2)(%[pos2])\n"
			"  ror.l #4, %[y]\n"
			"  move.l %[y], (13*" _QUOTE(SCREEN_WIDTH) "/2)(%[pos2])\n"

			"  move.w %[fg], 0x201000\n"
			"  move.l %[x], (12*" _QUOTE(SCREEN_WIDTH) "/2)(%[pos2])\n"
			"  ror.l #4, %[x]\n"
			"  move.l %[x], (13*" _QUOTE(SCREEN_WIDTH) "/2)(%[pos2])\n"

			: [x] "=&r" (x),
			  [y] "=&r" (y)
			: [fg] "r" (fg), 
			  [bg] "r" (bg),
			  [pos2] "a" (pos2),
			  [glyph] "a" (glyph));
		}
		else {
			uint8_t* glyph = font + c*fontHeight;
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
	putTextN(&c,1);
}

void putchar_(int c) {
	/* ASSUME BIG ENDIAN */
	putTextN(3+(char*)&c,1);
}

void scrollTextUp(uint16_t rows) {
	scrollUp(rows*fontHeight, winX*FONT_WIDTH, winY*fontHeight, winRightX*FONT_WIDTH, winBottomY*fontHeight, 
		background, scrollBitplanes);
}

void scrollTextDown(uint16_t rows) {
	scrollDown(rows*fontHeight, winX*FONT_WIDTH, winY*fontHeight, winRightX*FONT_WIDTH, winBottomY*fontHeight, 
		background, scrollBitplanes);
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
		setFont((uint8_t*)defaultLocation, 15);
	}
	else {
		uint8_t* location = romFind(testValue);
		if (location == 0) {
			setFont(font8x14, 15); 
			return;
		}
		location -= (testLocation-defaultLocation);
		setFont(location, 15);
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

