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
#define WRITE_FILE 2
#define READ_FILE  1

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
#define ADJUST_WIDTH(x) ((x)*57/50) // measured at 1.14
#define TEXT_MODE_NORMAL 0

uint16_t getKey(void);
void drawPixel(uint16_t x, uint16_t y);
void fillScreen(void);
void drawBlack(void);
void drawWhite(void);
void reload(void);
typedef void (*SetTextMode_t)(uint16_t);
#define setTextMode ((SetTextMode_t)0xeb08)
typedef void (*DrawText_t)(char*);
#define drawText ((DrawText_t)(0x0000eaf6))
typedef void (*SetCoordinates_t)(uint32_t,uint32_t);
#define setCoordinates ((SetCoordinates_t)(0x0000eae4))
typedef void (*Reload_t)(void);
#define _reload ((Reload_t)0x0000ece2)
typedef void (*SetCoordinates_t)(uint32_t,uint32_t);
typedef int16_t (*OpenFile_t)(const char* filename, uint16_t type, uint16_t mode);
#define openFile ((OpenFile_t)0x0000eb74)
typedef void (*CloseFile_t)(int16_t fd);
#define closeFile ((CloseFile_t)0x0000eb7a)
typedef int (*ReadFile_t)(int16_t fd, void* data, int32_t size);
#define readFile ((ReadFile_t)0x0000eb86)
typedef int (*WriteFile_t)(int16_t fd, const void* data, int32_t size);
#define writeFile ((WriteFile_t)0x0000eb80)
uint16_t getKeyWait(void);
void drawVerticalLine(uint16_t x, uint16_t y1, uint16_t y2);
void drawHorizontalLine(uint16_t x1, uint16_t y, uint16_t x2);
void waitSeconds(uint16_t n); 
uint32_t getVBLCounter(void);
void patchVBL(void);
void unpatchVBL(void);
void setVBLCounter(uint32_t value);

#endif