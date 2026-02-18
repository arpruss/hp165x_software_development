#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stddef.h>
#include <hp165x.h>

//1.12 : 1.17
//1.20 : 1.24
//clock rate formula: ratio of h-displayed 
//640x408: 506
//592x384: 596
//640x384: 509
//592x408: 505
//640x384: 509
//640x400: 507
uint16_t width=640;
uint16_t height=400;//392;
// 517

void set624x392() {
	width = 624;
	height = 392;
	*MC6845_REGISTER_ADDRESS = MC6845_V_DISPLAYED;
	*MC6845_REGISTER_VALUE = height/8; 
	*MC6845_REGISTER_ADDRESS = MC6845_V_SYNC;
	*MC6845_REGISTER_VALUE = height/8;  

	*MC6845_REGISTER_ADDRESS = 0;
	*MC6845_REGISTER_VALUE = (808+48)/8; 
	*MC6845_REGISTER_ADDRESS = MC6845_H_DISPLAYED;
	*MC6845_REGISTER_VALUE = width/8; 
	*MC6845_REGISTER_ADDRESS = MC6845_H_SYNC;
	*MC6845_REGISTER_VALUE = width/8+2; 
}

void set640x392() {
	width = 640;
	height = 392;
	*MC6845_REGISTER_ADDRESS = MC6845_V_DISPLAYED;
	*MC6845_REGISTER_VALUE = height/8; 
	*MC6845_REGISTER_ADDRESS = MC6845_V_SYNC;
	*MC6845_REGISTER_VALUE = height/8;  

	*MC6845_REGISTER_ADDRESS = 0;
	*MC6845_REGISTER_VALUE = (808+64)/8; 
	*MC6845_REGISTER_ADDRESS = MC6845_H_DISPLAYED;
	*MC6845_REGISTER_VALUE = width/8; 
	*MC6845_REGISTER_ADDRESS = MC6845_H_SYNC;
	*MC6845_REGISTER_VALUE = width/8+2; 
}

void drawPixel0(uint16_t x, uint16_t y) {
	volatile uint16_t* pos = SCREEN + y * (width/4) + x/4;
	*pos = 8>>(x%4);
}


/* Increase screen resolution to 592x392 */

#define MC6845_REGISTER_ADDRESS ((volatile uint8_t*)0x0020c001)
#define MC6845_REGISTER_VALUE ((volatile uint8_t*)0x0020c003)
int main(void) {
//	set624x392();

	patchVBL();
	*SCREEN_MEMORY_CONTROL = WRITE_BLACK;
	memset(SCREEN, 0xF, height*width/2);
	setTextColors(WRITE_WHITE,WRITE_GRAY);
	
	*SCREEN_MEMORY_CONTROL = WRITE_WHITE;
	for (int y=0; y<height; y++) {
		drawPixel0(0,y);
		drawPixel0(width/2,y);
		drawPixel0(width-1,y);
	}
	for (int x=0; x<width; x++) {
		drawPixel0(x,0);
		drawPixel0(x,2);
		drawPixel0(x,height/2);
		drawPixel0(x,height-1);
		drawPixel0(x,height-3);
	}

	uint16_t k;
	
	//1,1 okish
	//7,3 shifted up
	int16_t totalDelta = 1;
	int16_t syncDelta = 0;
	int16_t adjust = 0;

	do {
		_disableScreenAndOtherStuff();
		if (height % 8 == 0) {
			*MC6845_REGISTER_ADDRESS = MC6845_MAX_SCANLINE;
			*MC6845_REGISTER_VALUE = 7;
			*MC6845_REGISTER_ADDRESS = MC6845_V_TOTAL;
			*MC6845_REGISTER_VALUE = height/8+totalDelta;
			*MC6845_REGISTER_ADDRESS = MC6845_V_DISPLAYED;
			*MC6845_REGISTER_VALUE = height/8;
			*MC6845_REGISTER_ADDRESS = MC6845_V_SYNC;
			*MC6845_REGISTER_VALUE = height/8 + syncDelta;	
			*MC6845_REGISTER_ADDRESS = MC6845_V_ADJUST;
			*MC6845_REGISTER_VALUE = adjust;	
		}
		else if (height % 14 == 0) {
			*MC6845_REGISTER_ADDRESS = MC6845_MAX_SCANLINE;
			*MC6845_REGISTER_VALUE = 13;
			*MC6845_REGISTER_ADDRESS = MC6845_V_TOTAL;
			*MC6845_REGISTER_VALUE = height/14+totalDelta;
			*MC6845_REGISTER_ADDRESS = MC6845_V_DISPLAYED;
			*MC6845_REGISTER_VALUE = height/14;
			*MC6845_REGISTER_ADDRESS = MC6845_V_SYNC;
			*MC6845_REGISTER_VALUE = height/14 + syncDelta;	
			*MC6845_REGISTER_ADDRESS = MC6845_V_ADJUST;
			*MC6845_REGISTER_VALUE = adjust;	
		}

		*MC6845_REGISTER_ADDRESS = MC6845_H_TOTAL;
		*MC6845_REGISTER_VALUE = width != 592 ? 113 : 101; //101*width/592;
	//	(808+64+32)/8;  /* TODO: works for 640, adjust for others */
		*MC6845_REGISTER_ADDRESS = MC6845_H_DISPLAYED;
		*MC6845_REGISTER_VALUE = width/8; 
		*MC6845_REGISTER_ADDRESS = MC6845_H_SYNC;
		*MC6845_REGISTER_VALUE = width/8+5+2; 
		_enableScreenAndOtherStuff();
		
		k = getKey(1);
		switch(k) {
			case KEY_0:
				totalDelta--;
				break;
			case KEY_DECIMAL:
				totalDelta++;
				break;
			case KEY_1:
				syncDelta--;
				break;
			case KEY_2:
				syncDelta++;
				break;
			case KEY_4:
				adjust--;
				adjust &= 31;
				break;
			case KEY_5:
				adjust++;
				adjust &= 31;
				break;
			case KEY_RUN:
				syncDelta = 2;
				totalDelta = 1;
				adjust = 0;
				break;
		}
	} while ( k != KEY_STOP );

	uint32_t t=getVBLCounter();

	waitSeconds(10);

	t = getVBLCounter() - t;

	resetMC6845();
	
	printf("totalDelta:%d syncDelta:%d adjust=%d\n", totalDelta, syncDelta, adjust);
	printf("time:%d", t);
	getKey(1);

	reload();
	
}
 