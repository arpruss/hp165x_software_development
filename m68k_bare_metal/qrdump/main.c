#include <stdint.h>
#include "utils.h"

char alpha[] = {'a','b',0};
int beta[4];

int main() {
	drawBlack();
	fillScreen();
	drawWhite();
	fillScreen();
	drawBlack();
	setCoordinates(0,0);
	drawText("Hello\n world");
	setCoordinates(10,30);
	drawText(alpha);
	while (getKey() != 0x2001);
	for (int i=0; i<30; i++) {
		volatile uint16_t* pos = SCREEN + i*SCREEN_BUFFER_WIDTH/4 + 10; //y * (SCREEN_BUFFER_WIDTH/4) + x/4;
		drawPixel(10+i,i);
	}
//	fillScreen();
//	for (int i=0; i<SCREEN_BUFFER_WIDTH; i++) {
//		volatile uint16_t* pos = SCREEN + SCREEN_BUFFER_WIDTH/4 + 10; //y * (SCREEN_BUFFER_WIDTH/4) + x/4;
//		*pos = 1;
//	*pos = 1<<(8-(x%4));
//		drawPixel(i,i/8);
//		drawPixel(SCREEN_BUFFER_WIDTH-1-i,i/8);
//	}
	while (getKey() != 0x2001);
	reload();
	return 0;
}