#ifndef _UTILS_H
#define _UTILS_H

#include <stdint.h>

#define CURRENT_KEY ((volatile uint16_t*)0x980700)
#define LAST_KEY ((volatile uint16_t*)0x980704)
#define LAST_KEY_DURATION ((volatile uint16_t*)0x98070A)
#define SCREEN_MEMORY_CONTROL ((volatile uint16_t*)0x201000)
//#define WRITE_REVERSE 0x0008
#define WRITE_BLACK 0xFF00
#define WRITE_WHITE 0xFE00
#define SCREEN ((volatile uint16_t*)0x600000)
#define SCREEN_HEIGHT 384
#define SCREEN_WIDTH 592

#define KEY_NONE 0x0000
#define KEY_0 0x0240
#define KEY_DECIMAL 0x0440
#define KEY_CHS 0x0840
#define KEY_1 0x0220
#define KEY_2 0x0420
#define KEY_3 0x0820
#define KEY_4 0x0210
#define KEY_5 0x0410
#define KEY_6 0x0810
#define KEY_CLEAR 0x0120
#define KEY_STOP 0x0104
#define KEY_7 0x0208
#define KEY_8 0x0408
#define KEY_9 0x0808
#define KEY_DONTCARE 0x0110
#define KEY_RUN 0x0102
#define KEY_A 0x0204
#define KEY_B 0x0404
#define KEY_C 0x0804
#define KEY_D 0x0202
#define KEY_E 0x0402
#define KEY_F 0x0802
#define KEY_FORMAT 0x0101
#define KEY_TRACE 0x0201
#define KEY_DISPLAY 0x0401
#define KEY_IO 0x0801
#define KEY_UP_DOWN 0x1040
#define KEY_LEFT_RIGHT 0x2040
#define KEY_SELECT 0x2001
#define KEY_TURN_CCW 0x0082
#define KEY_TURN_CW 0x0081
#define ADJUST_WIDTH(x) ((x)*9/8)


uint16_t getKey(void);
void drawPixel(uint16_t x, uint16_t y);
void fillScreen(void);
void drawBlack(void);
void drawWhite(void);
typedef void (*DrawText_t)(char*);
#define drawText ((DrawText_t)(0x0000eaf6))
typedef void (*SetCoordinates_t)(uint32_t,uint32_t);
#define setCoordinates ((SetCoordinates_t)(0x0000eae4))
typedef void (*Reload_t)(void);
#define reload ((Reload_t)0x0000ece2)
uint16_t getKeyWait(void);
void drawVerticalLine(uint16_t x, uint16_t y1, uint16_t y2);
void drawHorizontalLine(uint16_t x1, uint16_t y, uint16_t x2);


#endif