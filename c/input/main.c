#include <string.h>
#include "hp165x.h"
#include <stdio.h>

int main() {
	setTextColors(WRITE_WHITE, WRITE_BLACK);
	*SCREEN_MEMORY_CONTROL = WRITE_BLACK;
	fillScreen();
	initKeyboard(1); // use serial
	
	while (1) {
		char buffer[200];
		putText("Input: ");
		*buffer=0;
		int16_t s = getText(buffer,200);
		printf("\nGot %d\n", s);
		if (s<0) 
			break;
		putChar('>');
		putText(buffer);
		putChar('\n');
	}
	
	reload();
	return 0;
}
