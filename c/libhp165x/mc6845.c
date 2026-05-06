#include <hp165x.h>
#include "screensize.h"

static uint16_t cellHeight = 8;
static uint8_t hTotal = 0x65;
static uint8_t vTotal = 0x32;
static uint8_t vAdjust = 0;
static const uint32_t clockPer1000s = 2424000*1000;//2386000*1000;
uint32_t ticksPer1000Sec = 60000;
uint16_t ticksPerSec = 60;
uint16_t msPerTick = 17;
uint16_t usPerTick = 16667;

static uint32_t calculateVerticalFrequency(void) {
    ticksPer1000Sec = clockPer1000s / ((uint32_t)hTotal * ((uint16_t)vTotal*8 + vAdjust));
    ticksPerSec = (ticksPer1000Sec + 500) / 1000;
    msPerTick = (1000 + ticksPerSec/2) / ticksPerSec;
    usPerTick = 1000000000 / ticksPer1000Sec;
}

//fclk = 2.5e6 or 2.386e6
//fv = fclk / (R0*(R4*(R9+1)+R5))
// fclk/(H_TOTAL*(V_TOTAL*8+V_ADJUST))
// V_TOTAL
uint16_t ticksPerSecond(void) {
#if 0    
	// This should be close to right for 592x384, 640x392, 640x400.
	// For others, need to measure better.
	
	if (screenWidth == 592)
		return 60;
	else if (screenHeight >= 400)
		return 52;
	else 
		return 53; /* measure more precisely */
#endif
    return ticksPerSec;
}

static uint8_t mc6845Defaults[] = { 
	0x65, // R0: H_TOTAL
	0x4a, // R1: H_DISPLAYED
	0x4e, // R2: MC6845_H_SYNC
	0xca, // R3: MC6845_SYNC_WIDTH
	0x32, // R4: MC6845_V_TOTAL
	0x00, // R5: MC6845_V_ADJUST
	0x30, // R6: V_DISPLAYED
	0x30, // R7: V_SYNC
	0x00, // R8: INTERLACE
	0x07, // R9: maximum scan line 
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
	static char initialized = 0;
	
	if (height == 0)
		height = DEFAULT_SCREEN_HEIGHT;
	if (height * (SCREEN_WIDTH/8) > 32768) 
		height = 32768/(SCREEN_WIDTH/8);
	height = height / 8;
	
	if (initialized && screenHeight == height * 8)
		return;
	if (!initialized && screenHeight == ROM_SCREEN_HEIGHT)
		return;
	initialized = 1;
	
	screenHeight = height * 8;
	
	*MC6845_REGISTER_ADDRESS = MC6845_V_TOTAL;
    vTotal = height <= 384/8 ? height + 2 : height + 1; // okish: height+2
	*MC6845_REGISTER_VALUE = vTotal; 
    // vAdjust??
	*MC6845_REGISTER_ADDRESS = MC6845_V_DISPLAYED;
	*MC6845_REGISTER_VALUE = height;
	*MC6845_REGISTER_ADDRESS = MC6845_V_SYNC;
	*MC6845_REGISTER_VALUE = height; //okish: height+1

    calculateVerticalFrequency();
	
	setTextWindow(0,0,0,0);
}

__attribute__((optimize("Os")))
void resetMC6845(void) {
	for (uint8_t i=0; i<sizeof(mc6845Defaults)/sizeof(*mc6845Defaults); i++) {
		*MC6845_REGISTER_ADDRESS = i;
		*MC6845_REGISTER_VALUE = mc6845Defaults[i];
	}
    hTotal = mc6845Defaults[MC6845_H_TOTAL];
    vTotal = mc6845Defaults[MC6845_V_TOTAL];
    vAdjust = mc6845Defaults[MC6845_V_ADJUST];
    calculateVerticalFrequency();
}

void _setScreenWidth(void) {
#if SCREEN_WIDTH != 592
    hTotal = (808+64+32)/8; /* TODO: works for 640, adjust for others */
	*MC6845_REGISTER_ADDRESS = MC6845_H_TOTAL; // R0 = 113
	*MC6845_REGISTER_VALUE = hTotal;   
	*MC6845_REGISTER_ADDRESS = MC6845_H_DISPLAYED; 
	*MC6845_REGISTER_VALUE = SCREEN_WIDTH/8; 
	*MC6845_REGISTER_ADDRESS = MC6845_H_SYNC;
	*MC6845_REGISTER_VALUE = SCREEN_WIDTH/8+5+2; /* works for 640 */
    calculateVerticalFrequency();
#endif	
}

