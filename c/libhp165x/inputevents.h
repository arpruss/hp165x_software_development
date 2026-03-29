#ifndef _KEYBOARD_H

#define _KEYBOARD_H
#include <hp165x.h>

#define _CTRL(x) ((x)-'a'+1)
#define KEYBOARD_CTRL_LEFT _CTRL('l')
#define KEYBOARD_CTRL_RIGHT _CTRL('r')
#define KEYBOARD_LEFT '\x02' // ctrl-b
#define KEYBOARD_RIGHT '\x06' // ctrl-f
#define KEYBOARD_UP '\x10' // ctrl-p
#define KEYBOARD_DOWN '\x0E' // ctrl-n
#define KEYBOARD_BREAK '\x03' // ctrl-c
#define KEYBOARD_DELETE '\x07' // ctrl-g
#define KEYBOARD_HOME 0xD2
#define KEYBOARD_END  0xD5
#define KEYBOARD_F1   0xC2
#define KEYBOARD_ALT_ALPHA(x) ((x)-'a'+0x88)
#define KEYBOARD_MOUSE 0xFF
#define KEYBOARD_INSERT      0xD1
//#define KEYBOARD_DELETE      0xD4
#define KEYBOARD_PAGE_UP     0xD3
#define KEYBOARD_PAGE_DOWN   0xD6

#define INPUT_MOUSE    0x01
#define INPUT_KEY      0x02
#define MOUSE_DATA     0xF0

// if you're using kbhit() or getch(), mouse events will get skipped
char kbhit(void);
char getch(void);

typedef struct {
	uint8_t type;
	union {
		struct {
			uint16_t character;
			uint16_t nativeKey;
		} key;
		struct {
			uint16_t x;
			uint16_t y;
			uint8_t buttons;
			uint8_t buttonDifference;
		} mouse;
	} data;
} InputEvent_t;

#define MOUSE_BUTTON_LEFT 		 0x01
#define MOUSE_DOUBLE_CLICK 		 0x80

uint8_t getInputEvent(InputEvent_t* e);
void initInputEvents(uint8_t useSerial);
void mouseArrow(uint16_t x, uint16_t y);
#define initKeyboard initInputEvents
typedef void (*ImageDrawer_t)(uint16_t x, uint16_t y);
void setMouseCursor(ImageDrawer_t drawer, uint16_t drawMode, uint16_t eraseMode, uint32_t timeoutSeconds);
void clearMouseCursor(void);
void drawMouseCursor(void);

#endif
