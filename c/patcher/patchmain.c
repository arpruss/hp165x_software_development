#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stddef.h>
#include <hp165x.h>

#include "locations.h"

#define JSR 0x4EB9

void myHandleSerialInterrupt(void);

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
PATCH(9e682a)
PATCH(9e734a)
PATCH(9e7940)
PATCH(9e7d30)
PATCH(9e7eb2)
PATCH(9e7e7c)
PATCH(9e7ee2)
PATCH(9e765e)
PATCH(9e6ab2)

void sendString(const char* s) {
	simple_serial_write(s, strlen(s));
}

void sendHex(uint32_t x) {
	static char buffer[8];
	
	for (uint16_t i=0; i<8; i++) {
		uint8_t nibble = (x & 0xF0000000) >> 28;
		x <<= 4;
		if (nibble < 10)
			buffer[i] = nibble + '0';
		else
			buffer[i] = nibble + ('A'-10);
	}
	simple_serial_write(buffer, 8);
	simple_serial_flush();
}

void init(void) {
	if (! initialized) {
		simple_serial_init(BAUD_19200);
		initialized = 1;
	}
	patchInt(INT_SERIAL,myHandleSerialInterrupt);
}

void called(uint32_t address, uint16_t argc, uint32_t* argv) {
	init();
	sendString("called ");
	sendHex(address);
	sendString(" from ");
	sendHex(argv[-1]);
	sendString(" : ");
	for (uint16_t i=0;i<argc;i++) {
		sendHex(argv[i]);
		sendString(" ");
	}
	sendString("\n");
}

void patch9e63f6(uint32_t* args) {
	called(0x9e63f6, 1, args);	
}

void value(uint32_t address, uint16_t bytes) {
	sendString("  value at ");
	sendHex(address);
	sendString(" = ");
	uint32_t v = 0;
	if (bytes == 1)
		v = *(uint8_t*)(address);	 
	else if (bytes == 2)
		v = *(uint16_t*)(address);
	else 
		v = *(uint32_t*)(address);
	sendHex(v);
	sendString("\n");
}

void patch9e7986(uint32_t* args) {
	called(0x9e7986, 1, args);
	value(0xa3b405,1);
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
	if (!initialized)
		return;
	called(0x9e7618, 2, args); // affects OSC(0x25)
	value(0xa3b415,1);
}

void patch9e7ee2(uint32_t* args) {
	called(0x9e7ee2, 2, args);
	sendString("\n");
}

void patch9e682a(uint32_t* args) {
	called(0x9e682a, 0, args);
	sendString("\n");
}

void patch9e734a(uint32_t* args) {
	called(0x9e734a, 0, args);
}

void patch9e7940(uint32_t* args) {
	called(0x9e7940, 2, args); // affects OSC(9)
}

void patch9e7d30(uint32_t* args) {
	called(0x9e7d30, 1, args); // affects OSC(0x2d)
}

void patch9e7eb2(uint32_t* args) {
	called(0x9e7eb2, 1, args); // affects OSC(21);
	value(0xa3b41e, 1);
	value(0xa3b41f, 1);
}

void patch9e7e7c(uint32_t* args) {
	called(0x9e7e7c, 1, args); // affects OSC(21);
	value(0xa3b41e, 1);
	value(0xa3b41f, 1);
}

void patch9e6ab2(uint32_t* args) {
	called(0x9e6ab2, 0, args); // affects relays
	value(0xa331f6, 4);
	value(0xa331f6 + 4, 4);
	value(0xa331fe, 2);
	value(0xa331fe + 2, 2);
	value(0xa3b3f2, 4);
	value(0xa3b3f2 +4, 4);
	value(0xa3b3fa, 2);
	value(0xa3b3fa +2, 2);
	value(0xa3b3fe, 2);
	value(0xa3b3fe + 2, 2);
}

void patch9e765e(uint32_t* args) {
	called(0x9e765e, 1, args); // affects OSC(0x13) and OSC(0x15), stores param in 00a3b40a
}

const struct callPatch patches[] = {
		{ 0x9e63f6, _patch9e63f6 },
		{ 0x9e7986, _patch9e7986 },
		{ 0x9e7842, _patch9e7842 },
		{ 0x9e78fa, _patch9e78fa },
		//{ 0x9e7618, _patch9e7618 } // called A LOT
		{ 0x9e682a, _patch9e682a },
		{ 0x9e734a, _patch9e734a },
		{ 0x9e7940, _patch9e7940 },
		{ 0x9e7d30, _patch9e7d30 },
		{ 0x9e7e7c, _patch9e7e7c },
		{ 0x9e7ee2, _patch9e7ee2 },
		{ 0x9e765e, _patch9e765e },
		{ 0x9e6ab2, _patch9e6ab2 },
};

void patchAll(void) {
	uint16_t* p = (uint16_t*)originalStart;
	uint16_t* end = (uint16_t*)(originalStart+originalSize-6);
	sendString("Searching ");
	sendHex((uint32_t)p);
	sendString(" : ");
	sendHex((uint32_t)end);
	sendString("\n");
	while (p<end) {
		if (*p == JSR) {
			p++;
			uint32_t address = *(uint32_t*)p;
			for (uint16_t i=0; i<sizeof(patches)/sizeof(*patches); i++) {
				if (patches[i].address == address) {
					sendString("patching ");
					sendHex((uint32_t)patches[i].address);
					sendString(" -> ");
					sendHex((uint32_t)patches[i].patch);
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
	
	simple_serial_init(BAUD_19200);
	simple_serial_write("hello\n",6);
	simple_serial_write("hello\n",6);
	simple_serial_write("hello\n",6);

	patchAll();
	
	simple_serial_close();

	asm volatile("jmp 0x984500");

	
}
