#include <stdlib.h>
#include <string.h>
#include "hp165x.h"
#define INITIAL_MARGIN 8

static const struct {
	uint16_t key;
	char	 ascii;
} keyToASCII[] = {
	{ KEY_0, '0' },
	{ KEY_1, '1' },
	{ KEY_2, '2' },
	{ KEY_3, '3' },
	{ KEY_4, '4' },
	{ KEY_5, '5' },
	{ KEY_6, '6' },
	{ KEY_7, '7' },
	{ KEY_8, '8' },
	{ KEY_9, '9' },
	{ KEY_A, 'A' },
	{ KEY_B, 'B' },
	{ KEY_C, 'C' },
	{ KEY_D, 'D' },
	{ KEY_E, 'E' },
	{ KEY_F, 'F' },
};

asm(
"_vbl_counter_code:\n"
"  add.l #1,vblCounterValue\n"
"_vbl_counter_jmp:\n"
"  jmp 0xDEADBEEF\n");

volatile uint32_t vblCounterValue = 0;
extern void _vbl_counter_code(void);
extern void _vbl_counter_jmp(void);

uint32_t getVBLCounter(void) {
	return vblCounterValue;
}

void setVBLCounter(uint32_t value) {
	vblCounterValue = value;
}

void unpatchVBL() {
	if (*(volatile uint16_t*)0x980000 == 0x4EF9 && *(volatile uint32_t*)0x980002 == (uint32_t)_vbl_counter_code) {
		*(volatile uint32_t*)0x980002 = *(volatile uint32_t*)((volatile uint8_t*)_vbl_counter_jmp+2);
	}
}

void patchVBL() {
	static char registeredAtExit = 0;

	if (!registeredAtExit) {
		atexit(unpatchVBL);
		registeredAtExit = 1;
	}
	
	if (*(volatile uint16_t*)0x980000 == 0x4EF9) {
		if (*(volatile uint32_t*)0x980002 == (uint32_t)_vbl_counter_code) // already patched
			return;
		*(volatile uint32_t*)((volatile uint8_t*)_vbl_counter_jmp+2) = *(volatile uint32_t*)0x980002;
		asm("move.l #_vbl_counter_code,0x980002");
//		*(volatile uint32_t*)0x980002 = (uint32_t)_vbl_counter_code;
	}
}

char parseKey(uint16_t k) {
	for (unsigned i=0; i<sizeof(keyToASCII)/sizeof(*keyToASCII); i++) {
		if (k == keyToASCII[i].key)
			return keyToASCII[i].ascii;
	}
	return 0;
}

uint16_t getKey(void) {
	uint16_t k = *LAST_KEY;
	*LAST_KEY = 0;
	return k;
}

void drawBlack(void) {
	*SCREEN_MEMORY_CONTROL = WRITE_BLACK;
}

void drawWhite(void) {
	*SCREEN_MEMORY_CONTROL = WRITE_WHITE;
}

void initialScreen() {
	volatile uint16_t* pos = SCREEN;
	for (unsigned y=0;y<SCREEN_HEIGHT;y++) {
		if (y<INITIAL_MARGIN || SCREEN_HEIGHT-INITIAL_MARGIN <= y) {
			*SCREEN_MEMORY_CONTROL = WRITE_BLACK;
			for (unsigned x4=0; x4<SCREEN_WIDTH/4; x4++)
				*pos++ = 0x0F;
		}
		else {
			*SCREEN_MEMORY_CONTROL = WRITE_BLACK;
			for (unsigned x4=0; x4<INITIAL_MARGIN/4; x4++)
				*pos++ = 0x0F;
			*SCREEN_MEMORY_CONTROL = WRITE_WHITE;
			for (unsigned x4=0; x4<(SCREEN_WIDTH-2*INITIAL_MARGIN)/4; x4++)
				*pos++ = 0x0F;
			*SCREEN_MEMORY_CONTROL = WRITE_BLACK;
			for (unsigned x4=0; x4<INITIAL_MARGIN/4; x4++)
				*pos++ = 0x0F;
		}
	}		
}

void reload(void) {
	unpatchVBL();
	initialScreen();
	_reload();
}

// TODO: assembly
void drawPixel(uint16_t x, uint16_t y) {
	volatile uint16_t* pos = SCREEN + y * (SCREEN_WIDTH/4) + x/4;
	*pos = 8>>(x%4);
}

// TODO: assembly
void fillScreen(void) {
	for (uint16_t i = 0 ; i < SCREEN_WIDTH/4 * SCREEN_HEIGHT ; i++)
		SCREEN[i] = 0xF;
}

void drawVerticalLine(uint16_t x, uint16_t y1, uint16_t y2) {
	uint16_t yCount;
	volatile uint16_t* pos;
	uint16_t mask = 8>>(x%4);
	if (y1 <= y2) {
		pos = SCREEN + (SCREEN_WIDTH/4) * y1 + (x/4);
		yCount = y2 - y1 + 1;
	}
	else {
		pos = SCREEN + (SCREEN_WIDTH/4) * y2 + (x/4);
		yCount = y1 - y2 + 1;
	}
	for (uint16_t i = 0 ; i < yCount ; i++) {
		*pos = mask;
		pos += SCREEN_WIDTH/4;
	}
}

void drawHorizontalLine(uint16_t x1, uint16_t y, uint16_t x2) {
	uint16_t xCount;
	volatile uint16_t* pos = SCREEN + (SCREEN_WIDTH/4) * y;
	uint16_t mask;
	
	if (x1 <= x2) {
		mask = 8>>(x1%4);
		pos += (x1/4);
		xCount = x2 - x1 + 1;
	}
	else {
		mask = 8>>(x2%4);
		pos += (x2/4);
		xCount = x1 - x2 + 1;
	}
	uint8_t value = 0;
	for (uint16_t i = 0 ; i < xCount ; i++) {
		value |= mask;
		mask >>= 1;
		if (mask == 0) {
			*pos++ |= value;
			mask = 8;
			value = 0;
		}
	}
	if (value)
		*pos = value;
}

uint16_t getKeyWait() {
	*LAST_KEY = 0;
	do {
		uint16_t k = *LAST_KEY;
		if (k) {
			*LAST_KEY = 0;
			return k;
		}
	} while(1);
}

void waitSeconds(uint16_t n) {
	for (int i=0;i<2*n;i++) {
		asm("  movem.l %d0-%d1/%a0,-(%sp)\n"
			"  move.w #60606,%d0\n"
			"  move.w %d0,%d1\n"
			".wait1:\n"         
			"  add.w %d1,%d1\n"   // 8 cycles
			"  add.w %d1,%d1\n"   // 8 cycles
			"  add.w %d1,%d1\n"   // 8 cycles
			"  add.w %d1,%d1\n"   // 8 cycles
			"  add.w %d1,%d1\n"   // 8 cycles
			"  add.w %d1,%d1\n"   // 8 cycles
			"  add.w %d1,%d1\n"   // 8 cycles
			"  add.w %d1,%d1\n"   // 8 cycles
			"  add.w %d1,%d1\n"   // 8 cycles
			"  add.w %d1,%d1\n"   // 8 cycles
			"  add.w %d1,%d1\n"   // 8 cycles
			"  add.w %d1,%d1\n"   // 8 cycles
			"  add.w %d1,%d1\n"   // 8 cycles
			"  add.w %d1,%d1\n"   // 8 cycles
			"  add.w %d1,%d1\n"   // 8 cycles
			"  add.w %d1,%d1\n"   // 8 cycles
			"  dbra %d0,.wait1\n" // 4 cycles
		    "  movem.l (%sp)+,%d0-%d1/%a0");
	}
}


_WRAP_1(drawText,0xeaf6);
_WRAP_1(setTextMode,0xeb08);
_WRAP_2(setCoordinates,0xeae4);
_WRAP_3(_openFile,0xeb74);
_WRAP_1(closeFile,0xeb7a);
_WRAP_3(readFile,0xeb86);
_WRAP_3(writeFile,0xeb80);
_WRAP_5(findDirEntry,0xeb98);
_WRAP_2(getDirEntry,0xebce);
_WRAP_0(_ebb0, 0xebb0);
_WRAP_1(_eb62, 0x227c);
_WRAP_0_RET_D1(getKeyClick,0xeb38);

void _eb62(int x);
int _ebb0(void);

int refreshDir(void) {
	_eb62(0);
	return _ebb0();
}

int openFile(const char* name, uint32_t fileType, uint32_t mode) {
	char paddedName[MAX_FILENAME_LENGTH] = "          "; // 10
	int length = strlen(name);
	if (length > MAX_FILENAME_LENGTH)
		return ERROR_FILE_NOT_FOUND;
	memcpy(paddedName, name, strlen(name));
	return _openFile(paddedName, fileType, mode);
}

