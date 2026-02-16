#ifndef _UTILS_H
#define _UTILS_H

#include <stdint.h>
#include <printf.h>

#define INT_VBL 	1
#define INT_SERIAL  6

#define CURRENT_KEY ((volatile uint16_t*)0x980700)
#define LAST_KEY ((volatile uint16_t*)0x980704)
#define LAST_KEY_DURATION ((volatile uint16_t*)0x98070A)
#define SPINNER_STATE ((volatile uint32_t*)0x98070C)
#define KEY_HOLD_TIME ((volatile uint32_t*)0x980708)
#define HARDWARE_STATUS ((volatile uint16_t*)0x20F000)
#define BEEPER ((volatile uint8_t*)0x203000)
#define BEEPER_ON 0xFF
#define BEEPER_OFF 0xFE

#define HARDWARE_STATUS_NO_DISK  (1<<3)
#define HARDWARE_STATUS_OLD_DISK 1 // disk hasn't been changed


#define __QUOTE(x) #x
#define _QUOTE(x) __QUOTE(x)

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

uint16_t getKey(char wait);
void reload(void);
void _exit(int status);
void exit(int status);

#define OPEN_READ 1
#define OPEN_WRITE 2
#define MAX_FILENAME_LENGTH 10

void romDelayTicks(uint32_t ticks);
void _restore_original_int_handlers(void);
void _final_cleanup(void);
void patchInt(uint16_t level, void (*address)());
void unpatchInt(uint16_t level);

typedef void (*Reload_t)(void);
#define _reload ((Reload_t)0x0000ece2)

/* does not include bottomRight coordinate in rectangle */
void setKeyWait(uint8_t w);
void waitSeconds(uint16_t n); 
uint32_t getVBLCounter(void);
void patchVBL(void);
void unpatchVBL(void);
void setVBLCounter(uint32_t value);
void initialScreen();
char parseKey(uint16_t key);
int loadAndRun(const char* filename);
int refreshDir(void);
int _refreshDir(void);
void delayTicks(uint32_t ticks);
void setKeyClick(uint8_t _click);
void setKeyRepeat(uint16_t delay, uint16_t rate);
uint16_t getKeyBIOS(void);
uint16_t peekKey(void);
#define ERROR_TIMEOUT (-2)
int getTextWithTimeout(char* _buffer, uint16_t maxSize, int timeoutTicks);
int getText(char* _buffer, uint16_t maxSize);
void padFilename(char* paddedName, const char* name);
int strncasecmp(const char* s1, const char* s2, int n);

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
		"move.l (44+5*4)(%sp),-(%sp)\n\t" \
		"jsr " #address "\n\t" \
		"add.l #20,%sp\n\t" \
		"movem.l (%sp)+,%d2-%d7/%a2-%a6\n\t" \
		"rts")


#include "puttext.h"
#include "serial.h"
#include "simple_serial.h"
#include "keyboard.h"
#include "mc6845.h"
#include "hpfile.h"
#include "screen.h"

#endif