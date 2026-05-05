#ifndef _KEYBOARD_H

#define _KEYBOARD_H
#include <hp165x.h>

#define KEYBOARD_CTRL_LEFT  0xA3
#define KEYBOARD_CTRL_RIGHT 0xA4

#define _CTRL(x) ((x)-'a'+1)
#define KEYBOARD_UP      0xDA
#define KEYBOARD_DOWN    0xD9
#define KEYBOARD_LEFT    0xD8
#define KEYBOARD_RIGHT   0xD7
#define KEYBOARD_BREAK   '\x03' // ctrl-c
#define KEYBOARD_DELETE  127 
#define KEYBOARD_HOME    0xD2
#define KEYBOARD_END     0xD5
#define KEYBOARD_F1      0xC2
#define KEYBOARD_Fn(x)   ((KEYBOARD_F1 - 1) + (x))
#define KEYBOARD_ALT_ALPHA(x) ((x)-'a'+0x88)
//#define KEYBOARD_MOUSE       0xFF
#define KEYBOARD_INSERT      0xD1
#define KEYBOARD_PAGE_UP     0xD3
#define KEYBOARD_PAGE_DOWN   0xD6

#define _KEYBOARD_HID_OFFSET      0x88
#define KEYBOARD_KP_DOT          (_KEYBOARD_HID_OFFSET+0x63)
#define KEYBOARD_KP_ASTERISK     (_KEYBOARD_HID_OFFSET+0x55)
#define KEYBOARD_KP_PLUS         (_KEYBOARD_HID_OFFSET+0x57)
#define KEYBOARD_KP_SLASH        (_KEYBOARD_HID_OFFSET+0x54)
#define KEYBOARD_KP_ENTER        (_KEYBOARD_HID_OFFSET+0x58)
#define KEYBOARD_KP_MINUS        (_KEYBOARD_HID_OFFSET+0x56)
#define KEYBOARD_KP_0            (_KEYBOARD_HID_OFFSET+0x62)
#define KEYBOARD_KP_1            (_KEYBOARD_HID_OFFSET+0x59)
#define KEYBOARD_KP_2            (_KEYBOARD_HID_OFFSET+0x5a)
#define KEYBOARD_KP_3            (_KEYBOARD_HID_OFFSET+0x5b)
#define KEYBOARD_KP_4            (_KEYBOARD_HID_OFFSET+0x5c)
#define KEYBOARD_KP_5            (_KEYBOARD_HID_OFFSET+0x5d)
#define KEYBOARD_KP_6            (_KEYBOARD_HID_OFFSET+0x5e)
#define KEYBOARD_KP_7            (_KEYBOARD_HID_OFFSET+0x5f)
#define KEYBOARD_KP_8            (_KEYBOARD_HID_OFFSET+0x60)
#define KEYBOARD_KP_9            (_KEYBOARD_HID_OFFSET+0x61)

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

typedef void (*ImageDrawer_t)(uint16_t x, uint16_t y);

typedef struct {
	ImageDrawer_t drawer;
	uint16_t drawMode;
	uint16_t eraseMode;
	uint32_t timeoutTicks;
	uint8_t visible;
} MouseCursorData_t;

void flushInputEvents(void);
uint8_t isMouseCursorVisible(void);
void saveMouseCursor(MouseCursorData_t* m); 
void restoreMouseCursor(const MouseCursorData_t* m);
uint8_t getInputEvent(InputEvent_t* e);
void initInputEvents(uint8_t useSerial);
void mouseArrow(uint16_t x, uint16_t y);
#define initKeyboard initInputEvents
void setMouseCursor(ImageDrawer_t drawer, uint16_t drawMode, uint16_t eraseMode, uint32_t timeoutSeconds);
void clearMouseCursor(void);
void drawMouseCursor(void);
uint8_t isInputSerialActive(void);
char haveInputEvent(void);

#endif
