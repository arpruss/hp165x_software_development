#include "hp165x.h"

static uint8_t useSerial = 0;
static uint8_t mouseEventPos = 0;
static uint8_t oldMouseButtons = 0;
static int8_t mouseData[2];
static uint8_t flushedMouseEvent = 0; // we requested a flush when the mouse event wasn't finished
static int16_t mouseX = 0;
static int16_t mouseY = 0;
static uint16_t mouseDrawMode;
static uint16_t mouseEraseMode;
static ImageDrawer_t drawMouse = NULL;
static uint32_t mouseCursorTimeoutTicks = 0;
static uint32_t lastMouseCursorTicks;
#define NO_CURSOR -1000
static int16_t mouseCursorX=NO_CURSOR;
static int16_t mouseCursorY;

#define IS_MOUSE(c) ( mouseEventPos > 0 || ( ((c) & MOUSE_DATA) == MOUSE_DATA ) )

uint8_t isInputSerialActive(void) {
	return useSerial;
}

void clearMouseCursor(void) {
	if (! useSerial )
		return;

	if (mouseCursorX != NO_CURSOR) {
		if (drawMouse != NULL) {
			*SCREEN_MEMORY_CONTROL = mouseEraseMode;
			drawMouse(mouseCursorX, mouseCursorY);
		}
		mouseCursorX = NO_CURSOR;
	}
}

void drawMouseCursor(void) {
	if (! useSerial )
		return;

	lastMouseCursorTicks = getVBLCounter();

	if (mouseCursorX == mouseX && mouseCursorY == mouseY) 
		return;
	
	clearMouseCursor();
	
	if (drawMouse != NULL) {
		mouseCursorX = mouseX;
		mouseCursorY = mouseY;
		*SCREEN_MEMORY_CONTROL = mouseDrawMode;
		drawMouse(mouseCursorX, mouseCursorY);
	}
}

static void mouseTimeoutCheck(void) {
	if (! useSerial )
		return;

	if (mouseCursorTimeoutTicks != 0 && getVBLCounter() - lastMouseCursorTicks > mouseCursorTimeoutTicks) 
		clearMouseCursor();
}

void restoreMouseCursor(const MouseCursorData_t* m) {
	if (! useSerial )
		return;
	
	clearMouseCursor();
	drawMouse = m->drawer;
	mouseDrawMode = m->drawMode;
	mouseEraseMode = m->eraseMode;
	mouseCursorTimeoutTicks = m->timeoutTicks;
	if (m->visible)
		drawMouseCursor();
}

uint8_t isMouseCursorVisible(void) {
	return mouseCursorX != NO_CURSOR;
}

void saveMouseCursor(MouseCursorData_t* m) {
	if (! useSerial) {
		m->visible = 0;
		m->drawer = NULL;
		return;
	}
	m->drawer = drawMouse;
	m->drawMode = mouseDrawMode;
	m->eraseMode = mouseEraseMode;
	m->timeoutTicks = mouseCursorTimeoutTicks;
	m->visible = mouseX != NO_CURSOR;
}

void setMouseCursor(ImageDrawer_t drawer, uint16_t drawMode, uint16_t eraseMode, uint32_t timeoutSeconds) {
	if (! useSerial )
		return;

	clearMouseCursor();
	drawMouse = drawer;
	mouseDrawMode = drawMode;
	mouseEraseMode = eraseMode;
	mouseCursorTimeoutTicks = timeoutSeconds * ticksPerSecond();
}

void initInputEvents(uint8_t s) {
	useSerial = s;
	if (useSerial) {
		simple_serial_init(BAUD_19200);
		mouseX = 0;
		mouseY = 0;
	}
	else
		simple_serial_close();
	oldMouseButtons = 0;
	drawMouse = NULL;
}

static uint8_t updateMouse(uint8_t serialChar, InputEvent_t* e) {
	if (! useSerial )
		return 0;
	if (mouseEventPos == 2) {
		int16_t dx = mouseData[1] & 0x7F;
		if (mouseData[1] & 0x40)
			dx |= 0xFF80;
		int16_t dy = serialChar & 0x7F;
		if (dy & 0x40)
			dy |= 0xFF80;
		uint8_t changed = dx != 0 || dy != 0; // register as a movement even if off-screen
		mouseX += dx;
		mouseY += dy;
		if (mouseX < 0)
			mouseX = 0;
		else if (mouseX >= screenWidth)
			mouseX = screenWidth-1;
		if (mouseY < 0)
			mouseY = 0;
		else if (mouseY >= screenHeight)
			mouseY = screenHeight-1;

		mouseEventPos = 0;
		
		uint8_t buttons = mouseData[0] & ~MOUSE_DATA;
		uint8_t buttonDifference = 0;

		if (serialChar & 0x80) 
			buttons |= MOUSE_DOUBLE_CLICK;
		if (buttons != oldMouseButtons) {
			changed = 1;
			buttonDifference = buttons ^ oldMouseButtons;
		}

		oldMouseButtons = buttons;

		if (e != NULL) {
			e->type = INPUT_MOUSE;
			e->data.mouse.buttons = buttons;
			e->data.mouse.buttonDifference = buttonDifference;
			e->data.mouse.x = mouseX;
			e->data.mouse.y = mouseY;
		}

		if (changed) 
			drawMouseCursor();
		
        if (flushedMouseEvent) {
            flushedMouseEvent = 0;
            return 0;
        }
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
	return 0;
}

char kbhit(void) {
	mouseTimeoutCheck();
	
	if (peekKey())
		return 1;
	if (useSerial) {
		while(1) {
			int16_t c = simple_serial_peek();
			if (c < 0)
				return 0;
			if ( IS_MOUSE(c) ) {
				updateMouse(simple_serial_getchar(), NULL);
			}
			else {
				return 1;
			}
		}
	}
	else {
		return 0;
	}
}

char haveInputEvent(void) {
	if (peekKey())
		return 1;
	if (useSerial) {
		while(1) {
			int16_t c = simple_serial_peek();
			if (c < 0)
				return 0;
			if ( IS_MOUSE(c) ) {
                if (mouseEventPos < 2) {
                    updateMouse(simple_serial_getchar(), NULL);
                }
                else {
                    if (!flushedMouseEvent)
                        return 1;
                    updateMouse(simple_serial_getchar(), NULL);
                }
			}
			else {
				return 1;
			}
		}
	}
	else {
		return 0;
	}
}

char getch(void) {
	while(1) {	
		mouseTimeoutCheck();

		uint16_t k = getKey(0);
		
		if (k != 0)
			return parseKey(k);
		
		if (useSerial && 0 <= simple_serial_peek()) {
			uint8_t c = simple_serial_getchar();
			
			if ( IS_MOUSE(c) ) {
				updateMouse(c, NULL);
			}
			else {
				return c;
			}
		}
	}
}

void flushInputEvents(void) {
    mouseTimeoutCheck();
    getKey(0);
    if (useSerial) {
        while ( simple_serial_peek() >= 0 ) {
            int16_t serialChar = simple_serial_getchar();
            
            if (IS_MOUSE(serialChar)) {
                updateMouse(serialChar, NULL);
            }
        }
        if (mouseEventPos > 0)
            flushedMouseEvent = 1;
    }
}

uint8_t getInputEvent(InputEvent_t* e) {
	mouseTimeoutCheck();

	uint16_t k = getKey(0);
	
	if (k != 0) {
		e->type = INPUT_KEY;
		e->data.key.character = parseKey(k);
		e->data.key.nativeKey = k;
		return 1;
	}
	if (useSerial) {
		int16_t serialChar = simple_serial_peek();
		if (serialChar >= 0) {
			serialChar = simple_serial_getchar();
			
			if (IS_MOUSE(serialChar)) {
				if (updateMouse(serialChar, e))
					return 1;
			}
			else {
				e->type = INPUT_KEY;
				e->data.key.character = (uint8_t)serialChar;
				e->data.key.nativeKey = 0;
				return 1;
			}
		}
	}
	return 0;
}

