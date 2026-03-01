#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stddef.h>
#include <hp165x.h>

#include "locations.h"

#define JSR 0x4EB9

uint32_t originalStart;
uint32_t originalSize;
char initialized = 0;

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
"  pea (4*(8+7)+4)(%sp)\n" \
"  jsr patch" _QUOTE(hex) "  \n" \
"  addq.l #4,%sp\n" \
"  movem.l (%sp)+,%d0-%d7/%a0-%a6\n" \
"  jmp 0x" _QUOTE(hex) "\n" \
);

PATCH(9e63f6)
PATCH(9e7986)
PATCH(9e7842)
PATCH(9e78fa)
PATCH(9e7618)

void sendString(const char* s) {
	simple_serial_write(s, strlen(s));
}

void sendHex(uint32_t x, uint16_t bytes) {
	char buffer[8];
	
	x <<= (4-bytes);
	for (uint16_t i=0; i<2*bytes; i++) {
		uint8_t nibble = (x & 0xF0000000) >> 28;
		x <<= 4;
		if (nibble < 10)
			buffer[i] = nibble + '0';
		else
			buffer[i] = nibble + ('A'-10);
	}
	simple_serial_write(buffer, 8);
}

void init(void) {
	if (! initialized) {
		simple_serial_init(BAUD_9600);
		initialized = 1;
	}
}

void called(uint32_t address, uint16_t argc, uint32_t* argv) {
	init();
	sendString("called ");
	sendHex(address,4);
	sendString(" : ");
	for (uint16_t i=0;i<argc;i++) {
		sendHex(argv[i],4);
		sendString(" ");
	}
	sendString("\n");
}

void patch9e63f6(uint32_t* args) {
	called(0x9e63f6, 1, args);	
}

void value(uint32_t address, uint16_t bytes) {
	sendString("  value at ");
	sendHex(address,4);
	sendString(" = ");
	uint32_t value = 0;
	if (bytes == 1)
		value = *(uint8_t*)(address);
	else if (bytes == 2)
		value = *(uint16_t*)(address);
	else 
		value = *(uint32_t*)(address);
	sendHex(value,bytes);
	sendString("\n");
}

void patch9e7986(uint32_t* args) {
	called(0x9e7986, 1, args);
	value(0x3b405,1);
}

void patch9e7842(uint32_t* args) {
	called(0x9e7842, 1, args);
	value(0x3b412,1);
	value(0x3b407,1);
}

void patch9e78fa(uint32_t* args) {
	called(0x9e78fa, 2, args);
	value(0x00a3b407,1);
}

void patch9e7618(uint32_t* args) {
	called(0x9e7618, 2, args);
	value(0xa3b415,1);
}

const struct callPatch patches[] = {
		{ 0x9e63f6, _patch9e63f6 },
		{ 0x9e7986, _patch9e7986 },
		{ 0x9e7842, _patch9e7842 },
		{ 0x9e78fa, _patch9e78fa },
		{ 0x9e7618, _patch9e7618 }

};

void patchAll(void) {
	uint16_t* p = (uint16_t*)originalStart;
	uint16_t* end = (uint16_t*)(originalStart+originalSize-6);
	sendString("Searching ");
	sendHex((uint32_t)p,4);
	sendString(" : ");
	sendHex((uint32_t)end,4);
	sendString("\n");
	while (p<end) {
		if (*p == JSR) {
			p++;
			uint32_t address = *(uint32_t*)p;
			for (uint16_t i=0; i<sizeof(patches)/sizeof(*patches); i++) {
				if (patches[i].address == address) {
					sendString("patching ");
					sendHex((uint32_t)patches[i].address,4);
					sendString(" -> ");
					sendHex((uint32_t)patches[i].patch,4);
					sendString("\n");
					*(uint32_t*)p = (uint32_t)patches[i].patch;
					p += 2;
					break;
				}
			}
		}
		else {
			p++;
		}
	}
	// TODO
}

main(int argc, char** argv) {
	(void)argc;
	(void)argv;
	
	originalStart = *ORIGINAL_START;
	originalSize = *ORIGINAL_SIZE;
	
	simple_serial_init(BAUD_9600);
	simple_serial_write("hello\n",6);
	simple_serial_write("hello\n",6);
	simple_serial_write("hello\n",6);

	patchAll();
	
	simple_serial_close();

	asm volatile("jmp 0x984500");

	
}
