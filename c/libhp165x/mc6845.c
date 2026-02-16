#include <hp165x.h>
#include "screensize.h"

uint16_t ticksPerSecond(void) {
	// This should be close to right for 592x384, 640x392, 640x400, 640x408.
	// For others, need to measure better.
	
	if (screenWidth == 592)
		return 60;
	else
		return 51; /* measure more precisely */
}

static uint8_t mc6845Defaults[] = { 
	0x65, 
	0x4a, 
	0x4e, 
	0xca, 
	0x32, // MC6845_V_TOTAL
	0x00, 
	0x30, 
	0x30, 
	0x00, // 
	0x07, // maximum scan line 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00 };

void _enableScreenAndOtherStuff(void) {
	(*(volatile uint8_t*)0x202001) = 1<<6;
}

void _disableScreenAndOtherStuff(void) {
	(*(volatile uint8_t*)0x202001) = 0;
}

void setScreenHeight(uint16_t height) {
	if (height == 0)
		height = DEFAULT_SCREEN_HEIGHT;
	if (height * (SCREEN_WIDTH/8) > 32768) 
		height = 32768/(SCREEN_WIDTH/8);
	height = height / 8;
	
	if (screenHeight == height * 8)
		return;
	
	screenHeight = height * 8;

	*MC6845_REGISTER_ADDRESS = MC6845_V_TOTAL;
	*MC6845_REGISTER_VALUE = height+2;
	*MC6845_REGISTER_ADDRESS = MC6845_V_DISPLAYED;
	*MC6845_REGISTER_VALUE = height;
	*MC6845_REGISTER_ADDRESS = MC6845_V_SYNC;
	*MC6845_REGISTER_VALUE = height;	
	
	setTextWindow(0,0,0,0);
}

void resetMC6845(void) {
	for (uint8_t i=0; i<sizeof(mc6845Defaults)/sizeof(*mc6845Defaults); i++) {
		*MC6845_REGISTER_ADDRESS = i;
		*MC6845_REGISTER_VALUE = mc6845Defaults[i];
	}
}

void _setScreenWidth(void) {
#if SCREEN_WIDTH != 592
	*MC6845_REGISTER_ADDRESS = MC6845_H_TOTAL;
	*MC6845_REGISTER_VALUE = (808+64+32)/8;  /* TODO: works for 640, adjust for others */
	*MC6845_REGISTER_ADDRESS = MC6845_H_DISPLAYED;
	*MC6845_REGISTER_VALUE = SCREEN_WIDTH/8; 
	*MC6845_REGISTER_ADDRESS = MC6845_H_SYNC;
	*MC6845_REGISTER_VALUE = SCREEN_WIDTH/8+5+2; /* works for 640 */
#endif	
}

