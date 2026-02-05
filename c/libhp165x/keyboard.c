#include "hp165x.h"

static char useSerial = 0;

void initKeyboard(char s) {
	useSerial = s;
	if (useSerial)
		simple_serial_init(BAUD_9600);
	else
		simple_serial_close();
}

char kbhit(void) {
	return (0 <= simple_serial_peek()) || peekKey();
}

char getch(void) {
	while(1) {	
		uint16_t k = getKey(0);
		
		if (k != 0)
			return parseKey(k);
		
		if (0 <= simple_serial_peek()) 
			return simple_serial_getchar();
	}
}
