#ifndef _UTILS_H
#define _UTILS_H

#include <stdint.h>

#define CURRENT_KEY ((volatile uint16_t*)0x980700)
#define LAST_KEY ((volatile uint16_t*)0x980704)
#define LAST_KEY_DURATION ((volatile uint16_t*)0x98070A)
#define SPINNER_STATE ((volatile uint32_t*)0x98070C)
#define KEY_HOLD_TIME ((volatile uint32_t*)0x980708)
#define SCREEN_MEMORY_CONTROL ((volatile uint16_t*)0x201000)
#define HARDWARE_STATUS ((volatile uint16_t*)0x20F000)
#define BEEPER ((volatile uint8_t*)0x203000)
#define BEEPER_ON 0xFF
#define BEEPER_OFF 0xFE


#define HARDWARE_STATUS_NO_DISC  (1<<3)
#define HARDWARE_STATUS_OLD_DISK 1 // disk hasn't been changed

// reading won't be very useful in these modes
#define WRITE_BLACK   0xF00 // clears all other planes than data, draws black on data
#define WRITE_WHITE   0xE00 // clears all other planes than data, draws white on data
// the following 4 modes allow reading
#define WRITE_SET_ATTR   0x007  // leaves data unchanged
#define WRITE_CLEAR_ATTR 0x807  // leaves data unchanged
#define WRITE_SET_DATA   0x00E  // leaves data unchanged
#define WRITE_CLEAR_DATA 0x10E  // leaves data unchanged
#define WRITE_NOP        0x00F

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
#define KEY_DONT_CARE 0x0110
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
void _exit(int status);
void exit(int status);

#define OPEN_READ 1
#define OPEN_WRITE 2
#define MAX_FILENAME_LENGTH 10

void setTextMode(uint32_t mode);
void drawText(const char* p);
void setCoordinates(int32_t x, int32_t y);
int _openFile(const char* filename, uint32_t fileType, uint32_t mode);
int openFile(const char* filename, uint32_t fileType, uint32_t mode);
int writeFile(int32_t fd, const void* data, int32_t size);
int readFile(int32_t fd, void* data, int32_t size);
void closeFile(int32_t fd);

typedef void (*Reload_t)(void);
#define _reload ((Reload_t)0x0000ece2)

typedef struct {
	char name[10];
	uint16_t type;
	uint32_t startBlock;
	uint32_t numBlocks;
	uint8_t dateAndTime[6];
	uint8_t misc[6];
} DirEntry_t;

int findDirEntry(const char*filename, uint32_t type, DirEntry_t* dirEntry,uint32_t startIndex, uint32_t nameLength);
int getDirEntry(int index, DirEntry_t* dirEntry);

#define ERROR_FILE_NOT_FOUND (-5)

void setKeyWait(uint8_t w);
void drawVerticalLine(uint16_t x, uint16_t y1, uint16_t y2);
void drawHorizontalLine(uint16_t x1, uint16_t y, uint16_t x2);
void waitSeconds(uint16_t n); 
uint32_t getVBLCounter(void);
void patchVBL(void);
void unpatchVBL(void);
void setVBLCounter(uint32_t value);
void initialScreen();
char parseKey(uint16_t key);
int loadAndRun(const char* filename);
int refreshDir(void);
void delayTicks(uint32_t ticks);
void setKeyClick(uint8_t _click);
void setKeyRepeat(uint16_t delay, uint16_t rate);
uint16_t getKey(void);
uint16_t getKeyBIOS(void);

/* 
   I don't know which registers are clobbered by the OS routines, so to
   be safe, we save all the ones that gcc requires to callee to be 
   responsible for saving: D2-D7 and A2-A6, for a total of 44 bytes. 
   */
#define _WRAP_0(name,address) \
	asm(".globl " #name "\n" \
		#name ":\n\t" \
		"movem.l %d2-%d7/%a2-%a6,-(%sp)\n\t" \
		"jsr " #address "\n\t" \
		"movem.l (%sp)+,%d2-%d7/%a2-%a6\n\t" \
		"rts")
		
#define _WRAP_0_RET_D1(name,address) \
	asm(".globl " #name "\n" \
		#name ":\n\t" \
		"movem.l %d2-%d7/%a2-%a6,-(%sp)\n\t" \
		"jsr " #address "\n\t" \
		"movem.l (%sp)+,%d2-%d7/%a2-%a6\n\t" \
		"move %d1,%d0\n\t" \
		"rts")
		
#define _WRAP_1(name,address) \
	asm(".globl " #name "\n" \
		#name ":\n\t" \
		"movem.l %d2-%d7/%a2-%a6,-(%sp)\n\t" \
		"move.l (44+4)(%sp),-(%sp)\n\t" \
		"jsr " #address "\n\t" \
		"addq #4,%sp\n\t" \
		"movem.l (%sp)+,%d2-%d7/%a2-%a6\n\t" \
		"rts")
		
#define _WRAP_2(name,address) \
	asm(".globl " #name "\n" \
		#name ":\n\t" \
		"movem.l %d2-%d7/%a2-%a6,-(%sp)\n\t" \
		"move.l (44+2*4)(%sp),-(%sp)\n\t" \
		"move.l (44+2*4)(%sp),-(%sp)\n\t" \
		"jsr " #address "\n\t" \
		"addq #8,%sp\n\t" \
		"movem.l (%sp)+,%d2-%d7/%a2-%a6\n\t" \
		"rts")
		
#define _WRAP_3(name,address) \
	asm(".globl " #name "\n" \
		#name ":\n\t" \
		"movem.l %d2-%d7/%a2-%a6,-(%sp)\n\t" \
		"move.l (44+3*4)(%sp),-(%sp)\n\t" \
		"move.l (44+3*4)(%sp),-(%sp)\n\t" \
		"move.l (44+3*4)(%sp),-(%sp)\n\t" \
		"jsr " #address "\n\t" \
		"add.l #12,%sp\n\t" \
		"movem.l (%sp)+,%d2-%d7/%a2-%a6\n\t" \
		"rts")

#define _WRAP_4(name,address) \
	asm(".globl " #name "\n" \
		#name ":\n\t" \
		"movem.l %d2-%d7/%a2-%a6,-(%sp)\n\t" \
		"move.l (44+4*4)(%sp),-(%sp)\n\t" \
		"move.l (44+4*4)(%sp),-(%sp)\n\t" \
		"move.l (44+4*4)(%sp),-(%sp)\n\t" \
		"move.l (44+4*4)(%sp),-(%sp)\n\t" \
		"jsr " #address "\n\t" \
		"add.l #16,%sp\n\t" \
		"movem.l (%sp)+,%d2-%d7/%a2-%a6\n\t" \
		"rts")

#define _WRAP_5(name,address) \
	asm(".globl " #name "\n" \
		#name ":\n\t" \
		"movem.l %d2-%d7/%a2-%a6,-(%sp)\n\t" \
		"move.l (44+5*4)(%sp),-(%sp)\n\t" \
		"move.l (44+5*4)(%sp),-(%sp)\n\t" \
		"move.l (44+5*4)(%sp),-(%sp)\n\t" \
		"move.l (44+5*4)(%sp),-(%sp)\n\t" \
		"jsr " #address "\n\t" \
		"add.l #20,%sp\n\t" \
		"movem.l (%sp)+,%d2-%d7/%a2-%a6\n\t" \
		"rts")


#include "puttext.h"

#endif