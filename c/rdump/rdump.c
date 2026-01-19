#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stddef.h>
#include <hp165x.h>


int main(void) {
	setTextBlackOnWhite(0);
	
	drawBlack();
	fillScreen();
	
	while (1) {
		char line[80];
		uint32_t* z = (uint32_t*)0x980700;
		sprintf(line, "%08x %08x %08x %08x", z[0], z[1], z[2], z[3]);
		setTextXY(0,0);
		putText(line);
	}
}
