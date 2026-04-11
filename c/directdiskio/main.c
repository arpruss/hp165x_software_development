#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stddef.h>
#include <hp165x.h>

#define MISC_CONTROL ((volatile uint8_t*)0x202001)
#define DRIVE_STATUS ((volatile uint8_t*)0x20d001)
#define DRIVE_STATUS_RQM 0x80
#define DRIVE_DATA   ((volatile uint8_t*)0x20d003)
#define DRIVE_SEEK       	  0x0F
#define DRIVE_SENSE_INTERRUPT 0x08

static void __attribute__((noinline)) shortPause(void) {
	asm volatile("   move.w #0x0C,%D0\n"		
				 "1: dbf %D0,1b");
}

static volatile uint8_t intDone;
static volatile uint8_t st0=0x11;
static volatile uint8_t trackFound=99;

uint8_t __attribute__((noinline)) waitForRQM(void) {
	for (uint16_t i=0;i<100;i++) {
		shortPause();
		if (*DRIVE_STATUS & DRIVE_STATUS_RQM)
			return 1;
	}
	return 0;
}

static void __attribute__((noinline))_int5_seek(void) {
	waitForRQM();
	*DRIVE_DATA = DRIVE_SENSE_INTERRUPT;
	waitForRQM();
	st0 = *DRIVE_DATA;
	waitForRQM();
	trackFound = *DRIVE_DATA;
	intDone = 1;	
	nopInt(5);
}

static void int5_seek(void) {
	asm volatile("movem.l %A0-%A1/%D0-%D1,-(%sp)");
	_int5_seek();
	asm volatile("movem.l (%sp)+,%A0-%A1/%D0-%D1");
	asm volatile("rte");	
	__builtin_unreachable(); 
}

uint8_t driveSeek(uint8_t track, uint8_t side) {
	*MISC_CONTROL = 0x42;
	intDone = 0;
	
	patchInt(5, int5_seek);

	waitForRQM();
	*DRIVE_DATA = DRIVE_SEEK;
	waitForRQM();
	*DRIVE_DATA = side ? 4 : 0; 
	waitForRQM();
	*DRIVE_DATA = track;

	while (!intDone);

	*MISC_CONTROL = 0x40;

	return (0 == (st0 & (0x80|0x40))) && (trackFound == track);
}

int main(int argc, char** argv) {
	(void)argc;
	(void)argv;
	
	initScreen(0, WRITE_BLACK);
	
	printf("Seeking to track 17\n");
	printf("Status: %d\n", driveSeek(255,1));
	getKey(1);
	reload(); 
	return 0;
}
