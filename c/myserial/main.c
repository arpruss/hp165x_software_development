#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stddef.h>
#include <hp165x.h>

int main(void) {
	*SCREEN_MEMORY_CONTROL = WRITE_BLACK;
	setTextColors(WRITE_WHITE,WRITE_BLACK);
	fillScreen();
	simple_serial_init(BAUD_9600);
	while (getKey() != KEY_STOP) {
		int16_t c = simple_serial_getchar();
		if (c != -1) {
			char ch=c;
			simple_serial_write(&ch, 1); 
			putChar(ch);
		}
	}
	reload();
}

