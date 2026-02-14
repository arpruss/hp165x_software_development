#include <stdlib.h>
#include <string.h>
#include "hp165x.h"
#define INITIAL_MARGIN 8


_WRAP_1(drawText,0xeaf6);
_WRAP_1(setTextMode,0xeb08);
_WRAP_2(setCoordinates,0xeae4);

uint16_t screenHeight = 384;

uint16_t getScreenHeight() {
	return screenHeight;
}

uint16_t getScreenWidth() {
	return SCREEN_WIDTH;
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
	pos2 = pos + (ROM_SCREEN_WIDTH/8) - 1;
	for (int16_t y = ROM_SCREEN_HEIGHT - 2 * INITIAL_MARGIN ; y >= 0 ; y--) {
		*pos = 0x000F000F;
		*pos2 = 0x000F000F;
		pos += ROM_SCREEN_WIDTH/8;
		pos2 += ROM_SCREEN_WIDTH/8;
	}
}

// TODO: assembly
void drawPixel(uint16_t x, uint16_t y) {
	volatile uint16_t* pos = SCREEN + y * (SCREEN_WIDTH/4) + x/4;
	*pos = 8>>(x%4);
}

// fills screen, up to 392 lines as needed
// todo: maybe only do the number of lines that are actually
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

/* TODO: very inefficient */
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

/* draws a frame around a fillRectangle() */
void frameRectangle(uint16_t topLeftX, uint16_t topLeftY, uint16_t bottomRightX, uint16_t bottomRightY, uint16_t thickness){
	int16_t left = topLeftX - thickness;
	if (left < 0)
		left = 0;
	int16_t top = topLeftY - thickness;
	if (top < 0)
		top = 0;
	int16_t right = bottomRightX + thickness;
	if (right > SCREEN_WIDTH)
		right = SCREEN_WIDTH;
	int16_t bottom = bottomRightY + thickness;
	if (bottom > screenHeight)
		bottom = screenHeight;
	fillRectangle(left,top,topLeftX,bottom);
	fillRectangle(bottomRightX,top,right,bottom);
	fillRectangle(topLeftX,top,bottomRightX,topLeftY);
	fillRectangle(topLeftX,bottomRightY,bottomRightX,bottom);
}

/* does not include bottomRight coordinate in rectangle */
void fillRectangle(uint16_t topLeftX, uint16_t topLeftY, uint16_t bottomRightX, uint16_t bottomRightY) {
	if (bottomRightY <= topLeftY || bottomRightX <= topLeftX)
		return;

	uint16_t height = bottomRightY - topLeftY;

	if (topLeftX == 0 && bottomRightX == SCREEN_WIDTH) {
		/* case 0: full rows */

		/* case 0a: full screen */
		if (topLeftX == 0 && bottomRightY == screenHeight) {
			/* use optimized assembly code */
			fillScreen();
			return;
		}

		/* case 0b: partial screen */
		volatile uint32_t* pos = (volatile uint32_t*)(SCREEN + topLeftY * SCREEN_WIDTH_WORDS);
		uint16_t n = SCREEN_WIDTH_DWORDS * height;
		while (n--)
			*pos++ = 0xF000F;
		return;
	}

	uint16_t width = bottomRightX - topLeftX;
	if (width == 0)
		return;


	uint16_t start_4 = topLeftX / 4;
	volatile uint16_t* pos = SCREEN + topLeftY * SCREEN_WIDTH_WORDS + start_4;	
	uint16_t end_4 = (bottomRightX - 1) / 4 - start_4;
	uint8_t startMod4 = topLeftX % 4;
	uint8_t endMod4 = (bottomRightX-1) % 4;
	
	if (end_4 == 0) {
		/* case 1: single 4 bit column */
		uint16_t writeMask = ((16>>startMod4)-1)-((8>>endMod4)-1);
		for (uint16_t i = 0 ; i < height ; i++) {
			*pos = writeMask;
			pos += SCREEN_WIDTH_WORDS;
		}
	}
	else if (end_4 == 1) {
		/* case 2: two 4 bit columns */
		uint32_t writeMask = (uint32_t)((16>>startMod4)-1) << 16 | (15-((8>>endMod4)-1));
		volatile uint32_t* pos2 = (volatile uint32_t*)pos;
		for (uint16_t i = 0 ; i < height ; i++) {
			*pos2 = writeMask;
			pos2 += SCREEN_WIDTH_DWORDS;
		}
	}
	else {
		/* case 3: more than two 4 bit columns */
		uint32_t writeMask = (uint32_t)((16>>startMod4)-1) << 16 | 0xF;
		volatile uint32_t* pos2 = (volatile uint32_t*)pos;
		for (uint16_t i = 0 ; i < height ; i++) {
			*pos2 = writeMask;
			pos2 += SCREEN_WIDTH_DWORDS;
		}
		pos += 2;
		width -= (4-startMod4)+4;
		while (width >= 8) {
			volatile uint32_t* pos2 = (volatile uint32_t*)pos;
			for (uint16_t i = 0 ; i < height ; i++) {
				*pos2 = 0xF000F;
				pos2 += SCREEN_WIDTH_DWORDS;
			}
			pos += 2;
			width -= 8;
		}
		if (width > 4) {
			volatile uint32_t* pos2 = (volatile uint32_t*)pos;			
			writeMask = 0xF0000 | (15-((8>>endMod4)-1));
			for (uint16_t i = 0 ; i < height ; i++) {
				*pos2 = writeMask;
				pos2 += SCREEN_WIDTH_DWORDS;
			}
		}
		else if (width > 0) {
			uint16_t endMask = 15-((8>>endMod4)-1);
			for (uint16_t i = 0 ; i < height ; i++) {
				*pos = endMask;
				pos += SCREEN_WIDTH_WORDS;
			}
		}
	}	
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

