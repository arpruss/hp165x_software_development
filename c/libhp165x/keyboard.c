#include "hp165x.h"

static uint8_t useSerial = 0;
static uint8_t mouseEventPos = 0;
static int8_t mouseData[2];

void initKeyboard(uint8_t s) {
	useSerial = s;
	if (useSerial)
		simple_serial_init(BAUD_19200);
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
		
		if (0 <= simple_serial_peek()) {
			return simple_serial_getchar();
		}
	}
}

uint8_t getInputEvent(InputEvent_t* e) {
	int16_t serialChar = simple_serial_peek();
	if (serialChar >= 0) {
		serialChar = simple_serial_getchar();
		if (mouseEventPos == 2) {
			e->type = INPUT_MOUSE;
			e->data.mouse.buttons = mouseData[0] & ~MOUSE_DATA;
			e->data.mouse.x = mouseData[1] & 0x7F;
			if (mouseData[1] & 0x40)
				e->data.mouse.x |= 0x80;
			e->data.mouse.y = serialChar & 0x7F;
			if (serialChar & 0x40)
				e->data.mouse.y |= 0x80;
				
			e->data.mouse.doubleClick = (serialChar & 0x80) ? 1 : 0;
			mouseEventPos = 0;
			return 1;
		}
		else if (mouseEventPos == 1) {
			mouseData[1] = serialChar;
			mouseEventPos = 2;
		}
		else if ((serialChar & MOUSE_DATA) == MOUSE_DATA) {
			mouseData[0] = serialChar;
			mouseEventPos = 1;
		}
		else {
			e->type = INPUT_KEY;
			e->data.key.character = (uint8_t)serialChar;
			e->data.key.nativeKey = 0;
			return 1;
		}
	}
	else {
		uint16_t k = getKey(0);
		if (k != 0) {
			e->type = INPUT_KEY;
			e->data.key.character = parseKey(k);
			e->data.key.nativeKey = k;
			return 1;
		}
	}
	return 0;
}

