#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stddef.h>
#include <hp165x.h>

#include "locations.h"

#define JSR 0x4EB9

uint32_t originalStart;
uint32_t originalSize;

struct callPatch {
	uint32_t address;
	void (*patch)(void);
};

#define PATCH(hex) \
void _patch##hex(void); \
asm( \
".globl _patch" _QUOTE(hex) "\n" \
"_patch" _QUOTE(hex) ":\n" \
"  movem.l %d0-%d7/%a0-%a6, -(%sp)\n" \
"  pea (4*4+4)(%sp)\n" \
"  jsr patch" _QUOTE(hex) "\n" \
"  addq.l #4,%sp\n" \
"  movem.l (%sp)+,%d0-%d7/%a0-%a6\n" \
"  jmp 0x" _QUOTE(hex) "\n" \
);

PATCH(9e63f6)

void sendString(const char* s) {
	simple_serial_write(s, strlen(s));
}

void sendHex(uint32_t x) {
	char buffer[8];
	
	for (uint16_t i=0; i<8; i++) {
		uint8_t nibble = (x & 0xF0000000) >> 28;
		x <<= 4;
		if (nibble < 10)
			buffer[i] = nibble + '0';
		else
			buffer[i] = nibble + ('A'-10);
	}
	simple_serial_write(buffer, 8);
}


void patch9e63f6(uint32_t* args) {
	sendString("0x9e63f6: ");
	sendHex(args[0]);
}

const struct callPatch patches[] = {
	{ 0x9e63f6, _patch9e63f6 }
};

void patchAll(void) {
	// TODO
}

main(int argc, char** argv) {
	(void)argc;
	(void)argv;
	
	originalStart = *ORIGINAL_START;
	originalSize = *ORIGINAL_SIZE;
	
	simple_serial_init(BAUD_9600);

	patchAll();
	
	asm("jsr 0x984500");
}
