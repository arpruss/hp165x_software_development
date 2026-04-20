#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stddef.h>
#include "hp165x.h"
#include "screensize.h"

static uint16_t foreground = WRITE_BLACK;
static uint16_t background = WRITE_WHITE;
static uint8_t scrollBitplanes = 1;

typedef uint8_t byte;

#include "ibm8x14hp.c"

static uint8_t underline=0;
static uint16_t cursorX=0;
static uint16_t cursorY=0;
static char cursorVisible=0;
static uint16_t winX=0;
static uint16_t winY=0;
static uint16_t winRightX=SCREEN_WIDTH/8;
static uint16_t winBottomY=DEFAULT_SCREEN_HEIGHT/14;
static uint16_t currentX=0;
static uint16_t currentY=0;
static uint8_t scrollMode=1;

static uint8_t* font = (uint8_t*)font8x14;
static uint16_t fontHeight = 14;
static uint16_t maxRows = DEFAULT_SCREEN_HEIGHT/14;
static uint16_t maxColumns = SCREEN_WIDTH/8;
static uint8_t reverse = 0;

#define ROR4(x) ((x) << 28 | (x) >> 4)

void setScrollMode(uint8_t m) {
	scrollMode = m;
}

void setTextUnderline(uint8_t u) {
	underline = u;
}

uint8_t getTextUnderline(void) {
	return underline;
}

void getTextWindow(uint16_t* xP,uint16_t *yP,uint16_t *x2P,uint16_t *y2P) {
	if (xP != NULL)
		*xP = winX;
	if (yP != NULL)
		*yP = winY;
	if (x2P != NULL)
		*x2P = winRightX;
	if (y2P != NULL)
		*y2P = winBottomY;
}

/* if bottomRightX==0 or bottomRightY==0, set to maximum possible; if negative,
   add to maximum (i.e., specify margin) */
void setTextWindow(uint16_t topLeftX,uint16_t topLeftY,int16_t bottomRightX,int16_t bottomRightY) {
	maxRows = screenHeight / fontHeight;
	winX = topLeftX;
	winY = topLeftY;
	if (bottomRightX <= 0)
		bottomRightX += maxColumns;
	if (bottomRightY <= 0)
		bottomRightY += maxRows;
	winRightX = bottomRightX;
	winBottomY = bottomRightY;

	if (currentX < winX)
		currentX = winX;
	else if (currentX >= winRightX)
		currentX = winRightX-1;
	if (currentY < winY)
		currentY = winY;
	else if (currentY >= winBottomY)
		currentY = winBottomY-1;
	cursorX = winX;
	cursorY = winY;
}

void setFont(uint8_t* data, uint16_t height) {
	uint16_t pixelY = currentY * fontHeight;
	maxRows = screenHeight / height;
	if (height != fontHeight) {
		winY = 0;
		winBottomY = maxRows-1;
	}
	font = data;
	fontHeight = height;
	currentY = pixelY / height;
	if (currentY >= maxRows)
		currentY = maxRows-1;
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

void setTextReverse(uint8_t _reverse) {
	reverse = _reverse;
}

uint8_t getTextReverse(void) {
	return reverse;
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
	return currentX-winX;
}

uint16_t getTextY(void) {
	return currentY-winY;
}

uint16_t pixelToTextX(uint16_t x) {
	return x/FONT_WIDTH - winX;
}

uint16_t pixelToTextY(uint16_t y) {
	return y/fontHeight - winY;
}

uint16_t textToPixelX(uint16_t x) {
	return (x+winX) * FONT_WIDTH;
}

uint16_t textToPixelY(uint16_t y) {
	return (y+winY) * fontHeight;
}

void setTextXY(uint16_t x, uint16_t y) {
	currentX = winX+x;
	currentY = winY+y;
}

void setTextX(uint16_t x) {
	currentX = winX+x;
}

void setTextY(uint16_t y) {
	currentY = winY+y;
}

void highlightText(uint16_t n, uint8_t highlightState) {
	volatile uint16_t* pos = SCREEN + currentY * (fontHeight*(SCREEN_WIDTH/4)) + currentX*2;

	*SCREEN_MEMORY_CONTROL = highlightState ? WRITE_SET_ATTR : WRITE_CLEAR_ATTR;
	while(n--) {
		if (currentX >= winRightX) {
			currentY++;
			if (currentY >= winBottomY)
				currentY = winBottomY-1;
			currentX = winX;
			pos = SCREEN + currentY * (fontHeight*(SCREEN_WIDTH/4));
		}
		volatile uint16_t* pos2 = pos;
		uint16_t row;
		for (row = 0; row < fontHeight; row++) {
			*(uint32_t*)pos2 = 0x000F000F;
			pos2 += SCREEN_WIDTH/4;			
		}
		pos += 2;
		currentX++;
	}
}

uint16_t putText(const char* s) {
	return putTextN(s, 0xFFFF);
}

/* returns number of lines scrolled */
uint16_t __attribute__((noinline,noclone)) putTextN(const char* s, uint16_t n) {
	volatile uint16_t* pos = SCREEN + currentY * (fontHeight*(SCREEN_WIDTH/4)) + currentX*2;
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
		
		if (c == '\n' || currentX >= winRightX) {
			currentX = winX;
			currentY++;
			if (currentY >= winBottomY) {
				currentY = winBottomY-1;
				if (scrollMode) {
					scrollTextUp(1);
					scrolled++;
				}
			}
			pos = SCREEN + currentY * (fontHeight*(SCREEN_WIDTH/4)) + currentX * 2;
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

			: [x] "=&d" (x),
			  [y] "=&d" (y)
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

			: [x] "=&d" (x),
			  [y] "=&d" (y)
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
		
		if (underline) {
			*SCREEN_MEMORY_CONTROL = fg;
			*(volatile uint32_t*)(pos + (fontHeight-1)*(SCREEN_WIDTH/4)) = 0xF000F;
		}
		
		pos += 2;
		
		currentX++;
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

uint8_t getScrollBitplanes(void) {
	return scrollBitplanes;
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

void setTextCursorXY(uint16_t x, uint16_t y) {
	char v = cursorVisible;
	if (v)
		showTextCursor(0);
	cursorX = x;
	cursorY = y;
	if (v)
		showTextCursor(1);
}

void updateTextCursor(char visible) {
	cursorVisible = visible;
	setTextCursorXY(currentX, currentY);
}

void showTextCursor(char value) {
	volatile uint32_t* pos = (uint32_t*)(SCREEN + cursorY * (fontHeight*(SCREEN_WIDTH/4)) + cursorX*2);
	*SCREEN_MEMORY_CONTROL = value ? WRITE_SET_ATTR : WRITE_CLEAR_ATTR;
	for (uint16_t i=0; i < fontHeight; i++) {
		*pos = 0xFFFFFFFF;
		pos += SCREEN_WIDTH_DWORDS;
	}
	cursorVisible = value;
}

uint8_t isTextCursorVisible(void) {
    return cursorVisible;
}