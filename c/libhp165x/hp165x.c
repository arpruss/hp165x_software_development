#include <stdlib.h>
#include <string.h>
#include "hp165x.h"
#define INITIAL_MARGIN 8

asm(
"_vbl_counter_code:\n"
"  addq.l #1,vblCounterValue\n"
"  jmp _original_int1_handler\n"); 

volatile uint32_t vblCounterValue = -1;
extern void _vbl_counter_code(void);
extern void _vbl_counter_jmp(void);
extern void _original_int1_handler(void);
void _final_cleanup_with_atexit_support(void);

uint32_t getVBLCounter(void) {
	return vblCounterValue;
}

void setVBLCounter(uint32_t value) {
	vblCounterValue = value;
}

void patchInt(uint16_t level, void (*address)()) {
	volatile uint8_t* ptr = ((volatile uint8_t*)0x980000)+6*(level-1);
	*(volatile uint32_t*)(ptr+2) = (uint32_t)address;
	*(volatile uint16_t*)(ptr) = 0x4EF9;
}

void unpatchInt(uint16_t level) {
	volatile uint8_t* current = ((volatile uint8_t*)0x980000)+6*(level-1);
	volatile uint8_t* orig = ((volatile uint8_t*)_original_int1_handler)+6*(level-1);
	*(volatile uint32_t*)(current+2) = *(volatile uint32_t*)(orig+2);
	*(volatile uint16_t*)(current) = *(volatile uint16_t*)(orig);
}

void patchVBL() {
	vblCounterValue = 0;
	patchInt(INT_VBL, _vbl_counter_code);
}

void reload(void) {
	initialScreen();
	_final_cleanup_with_atexit_support();
	//asm volatile("move.l 0x00A7FFFE, %sp"); 
	_reload();
	__builtin_unreachable(); 
}

// https://stackoverflow.com/questions/66586687/delay-loop-in-68k-assembly

// the processor is supposed to be 10MHz, but my timing shows more like 8.5MHz
// per tick. Some of that may be due to VBL stealing CPU cycles.
static void waitSecond(void) {
	asm volatile("  move.l #(8500000/16),%%d0\n" 
		"  moveq.l #1,%%d1\n"
		"1:\n"
		"  sub.l %%d1,%%d0\n"
		"  bne.s 1b\n" : : : "d0", "d1");
}

void waitSeconds(uint16_t n) {
	for (int i=0;i<n;i++) {
		waitSecond();
	}
}

_WRAP_1(romDelayTicks,0xea06);

asm(".globl delayTicks\n" 
	"delayTicks:\n\t" 
	"movem.l %d2-%d7/%a2-%a6,-(%sp)\n\t" \
	"move.l (44+4)(%sp),%d0\n\t" \
	"move.l #0x1C,%d7\n\t" \
	"trap #0\n\t" \
	"movem.l (%sp)+,%d2-%d7/%a2-%a6\n\t" \
	"rts");
		

_WRAP_0_RET_D1(getKeyBIOS,0xeb38);
