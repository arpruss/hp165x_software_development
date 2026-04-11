#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stddef.h>
#include <hp165x.h>

#define MISC_CONTROL ((volatile uint8_t*)0x202001)
#define DRIVE_STATUS ((volatile uint8_t*)0x20d001)
#define DRIVE_STATUS_RQM 0x80
#define DRIVE_STATUS_EXM 0x20
#define DRIVE_DATA   ((volatile uint8_t*)0x20d003)
#define DRIVE_SEEK       	  0x0F
#define DRIVE_READ			  0x46
#define DRIVE_WRITE			  0x45
#define DRIVE_SENSE_INTERRUPT 0x08
#define DISK_SECTORS_PER_TRACK     5
#define DISK_SECTOR_SIZE_SELECT			  3
#define DISK_GPL		0x32
#define DISK_SECTOR_SIZE      1024

#define IO_ERROR  ((int16_t)-32768)

static void __attribute__((noinline)) shortPause(void) {
	asm volatile("   move.w #0x0C,%D0\n"		
				 "1: dbf %D0,1b");
}

static volatile char* volatile buffer;
static volatile uint8_t intDone;
static volatile uint8_t intError;
static uint8_t st0;
static uint8_t st1;
static uint8_t st2;
static uint8_t finalTrack;
static uint8_t finalSide;
static uint8_t finalSector;
static uint8_t finalSectorSize;

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
	finalTrack = *DRIVE_DATA;
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

static void __attribute__((noinline,used))ioStatus(void) {
	waitForRQM();
	st0 = *DRIVE_DATA;
	waitForRQM();
	st1 = *DRIVE_DATA;
	waitForRQM();
	st2 = *DRIVE_DATA;
	waitForRQM();
	finalTrack = *DRIVE_DATA;
	waitForRQM();
	finalSide = *DRIVE_DATA;
	waitForRQM();
	finalSector = *DRIVE_DATA;
	waitForRQM();
	finalSectorSize = *DRIVE_DATA;
}

char topByte;

static void int5_read(void) {
	asm volatile(
		"i5r:   move.b 0x20d003,topByte\n"
		"	move.l %a0,-(%sp)\n"
		"	move.l buffer,%a0\n"
		"	move.b topByte,(%a0)+\n"
		"1:\n"
		"	cmp.b #0xF0,0x20d001\n"
		"	beq 2f\n"
		"	cmp.b #0xF0,0x20d001\n"
		"	beq 2f\n"
		"	cmp.b #0xF0,0x20d001\n"
		"	beq 2f\n"
		"	cmp.b #0xF0,0x20d001\n"
		"	beq 2f\n"
		"	cmp.b #0xF0,0x20d001\n"
		"	beq 2f\n"
		"	cmp.b #0xF0,0x20d001\n"
		"	beq 2f\n"
		"	cmp.b #0xF0,0x20d001\n"
		"	beq 2f\n"
		"	cmp.b #0xF0,0x20d001\n"
		"	beq 2f\n"
		"	cmp.b #0xF0,0x20d001\n"
		"	beq 2f\n"
		"	btst.b #3,0x20f001\n"
		"	bne read_no_disk\n"
		"	btst.b #5,0x20d001\n"
		"	bne 1b\n"
		"read_done:\n"
		"   movem.l %A0-%A1/%D0-%D1,-(%sp)\n"
		"	jsr ioStatus\n"
		"   movem.l (%sp)+,%A0-%A1/%D0-%D1\n"
		"read_done_no_status:\n"
		"	move.b #1,intDone\n" 
		"	move.w #0x4e73,0x0980018\n"
		"	move.l %a0,buffer\n"
		"	move.l (%sp)+,%a0\n"
		"	rte\n"
		"2:\n"
		"	move.b 0x20d003,(%a0)+\n"
		"	bra 1b\n"
		"read_no_disk:\n"
		"	move.b #1,intError\n"
		"	bra read_done_no_status\n"
		);
		__builtin_unreachable(); 
}

static void __attribute__((noinline))_int5_write(void) {
	intDone = 1;	
	nopInt(5);
}

static void int5_write(void) {
	asm volatile("rte");	
	__builtin_unreachable(); 
}

int16_t readWriteSectors(uint8_t track, uint8_t side, uint8_t startSectorID, uint8_t endSectorID, char* data, uint8_t mode) {
	if (mode != DRIVE_READ)
		return IO_ERROR;
	
	int16_t didRead;
	*MISC_CONTROL = 0x42;
	intDone = 0;
	intError = 0;
	buffer = data;
	patchInt(5, mode == DRIVE_READ ? int5_read : int5_write);

	waitForRQM();
	*DRIVE_DATA = mode;
	waitForRQM();
	*DRIVE_DATA = side ? 0 : 4;
	waitForRQM();
	*DRIVE_DATA = track;
	waitForRQM();
	*DRIVE_DATA = side;
	waitForRQM();
	*DRIVE_DATA = startSectorID;
	
	waitForRQM();
	*DRIVE_DATA = DISK_SECTOR_SIZE_SELECT;
	waitForRQM();
	*DRIVE_DATA = endSectorID;
	waitForRQM();
	*DRIVE_DATA = DISK_GPL;
	waitForRQM();
	*DRIVE_DATA = 0xFF; //DTL

	while(!intDone);

	didRead = buffer-data;
	if (!intError) {
		if (st2 & 0x20)
			didRead = -didRead;
	}
	*MISC_CONTROL = 0x40;

	return didRead;
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

	return (0 == (st0 & (0x80|0x40))) && (finalTrack == track);
}

static char trackBuffer[DISK_SECTORS_PER_TRACK*DISK_SECTOR_SIZE] = {0};

int main(int argc, char** argv) {
	(void)argc;
	(void)argv;
	
	initScreen(0, WRITE_BLACK);
	
	printf("Seek: %d\n", driveSeek(0,0));
	int16_t n = readWriteSectors(0,0,1,5,trackBuffer,DRIVE_READ);
	printf("Read %d\n", n);
	printf("At 0: %08lx\n", *(uint32_t*)trackBuffer);
	printf("status %x %x %x\n", st0,st1,st2);
	printf("pos %x %x %x\n", finalTrack,finalSide,finalSector);
	
	getKey(1);
	reload(); 
	return 0;
}
