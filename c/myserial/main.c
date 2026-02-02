#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stddef.h>
#include <hp165x.h>

void simple_serial_init(void);
int16_t simple_serial_getchar(void);
void simple_serial_send(uint32_t size, const void* data);
extern void myHandleSerialInterrupt();

int main(void) {
	*SCREEN_MEMORY_CONTROL = WRITE_BLACK;
	setTextColors(WRITE_WHITE,WRITE_BLACK);
	fillScreen();
	serialSetup(BAUD_9600, PARITY_NONE, STOP_BITS_1, DATA_BITS_8, PROTOCOL_NONE);
	simple_serial_init();
	while (getKey() != KEY_STOP) {
		int16_t c = simple_serial_getchar();
		if (c != -1) {
//			simple_serial_send(3,"ack");
			putChar(c);
		}
	}
	reload();
}

