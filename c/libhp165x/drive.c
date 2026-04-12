#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stddef.h>
#include <hp165x.h>

#define BLOCKS_PER_SECTOR (DISK_DEFAULT_SECTOR_SIZE/LIF_BLOCK_SIZE)

#define BLOCKS_PER_TRACK (DISK_DEFAULT_SECTORS_PER_TRACK*BLOCKS_PER_SECTOR)


static void __attribute__((noinline)) shortPause(void) {
	asm volatile("   move.w #0x0C,%D0\n"		
				 "1: dbf %D0,1b");
}

extern uint8_t _dirtyDisk;

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

static char trackBuffer[DISK_DEFAULT_SECTOR_SIZE * DISK_DEFAULT_SECTORS_PER_TRACK];
static uint8_t bufferPositionTrack;
static uint8_t bufferPositionSide = 0xFF;
static uint8_t bufferPositionSector;

uint8_t driveGetST0(void) {
	return st0;
}

uint8_t driveGetST1(void) {
	return st1;
}

uint8_t driveGetST2(void) {
	return st2;
}

void driveGetFinalPosition(uint8_t* trackP, uint8_t* sideP, uint8_t* sectorP) {
	if (trackP != NULL)
		*trackP = finalTrack;
	if (sideP != NULL) 
		*sideP = finalSide;
	if (sectorP != NULL)
		*sectorP = finalSector;
}

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

static char __attribute__((used)) topByte;

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

int16_t driveSeek(uint8_t track, uint8_t side) {
	*MISC_CONTROL = 0x48|0x02;
	intDone = 0;
	
	patchInt(5, int5_seek);

	waitForRQM();
	*DRIVE_DATA = DRIVE_SEEK;
	waitForRQM();
	*DRIVE_DATA = side ? 0 : 4; 
	waitForRQM();
	*DRIVE_DATA = track;

	while (!intDone);

	*MISC_CONTROL = 0x48;

	return (0 == (st0 & (0x80|0x40))) && (finalTrack == track) ? 0 : -1;
}

int16_t driveReadWriteSectors(uint8_t track, uint8_t side, uint8_t startSectorID, uint8_t endSectorID, uint8_t sectorSizeSelect, void* data, uint8_t mode) {
	if (mode != DRIVE_READ)
		return DRIVE_ERROR; // TODO
	
	if (driveSeek(track, side) < 0)
		return DRIVE_ERROR;
	
	int16_t didRead;
	*MISC_CONTROL = 0x48|0x02;
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
	*DRIVE_DATA = sectorSizeSelect;
	waitForRQM();
	*DRIVE_DATA = endSectorID;
	waitForRQM();
	*DRIVE_DATA = DISK_GPL;
	waitForRQM();
	*DRIVE_DATA = 0xFF; //DTL

	while(!intDone);

	didRead = buffer-(char*)data;
	if (!intError) {
		if ((st2 & 0x20) || (st1 & 0x20))
			didRead = -didRead;
	}
	*MISC_CONTROL = 0x48;

	return didRead;
}

int16_t driveReadBlock(uint16_t blockNum, void* data) {
	if (blockNum > *(volatile uint16_t*)0x9842a8)
		return -1;
	uint8_t track = blockNum / (BLOCKS_PER_TRACK*2);
	blockNum -= track * (uint16_t)(BLOCKS_PER_TRACK*2);
	uint8_t side;
	if (blockNum >= BLOCKS_PER_TRACK) {
		side = 1;
		blockNum -= BLOCKS_PER_TRACK;
	}
	else {
		side = 0;
	}
	
	uint8_t sector = blockNum / BLOCKS_PER_SECTOR;
	blockNum -= sector * (uint16_t)BLOCKS_PER_SECTOR;
	
	if (_dirtyDisk || side != bufferPositionSide || track != bufferPositionTrack || sector < bufferPositionSector) {
		uint8_t sectorID = (track == 79 ? 97 : 1) + sector;
		uint8_t count = DISK_DEFAULT_SECTORS_PER_TRACK - sector;
		uint8_t lastSectorID = sectorID + count - 1;
		uint16_t i;
		
		for (i = 0 ; i < 3 ; i++) {
			int16_t n = driveReadWriteSectors(track,side,sectorID,lastSectorID,DISK_DEFAULT_SECTOR_SIZE_SELECT,trackBuffer,DRIVE_READ);
			if (n == count * (uint16_t)DISK_DEFAULT_SECTOR_SIZE)
				break;
		}
		if (i >= 3)
			return -1;
		bufferPositionSide = side;
		bufferPositionTrack = track;
		bufferPositionSector = sector;
		_dirtyDisk = 0;
	}
	memcpy(data, trackBuffer+(sector-bufferPositionSector)*(uint16_t)DISK_DEFAULT_SECTOR_SIZE+blockNum*(uint16_t)LIF_BLOCK_SIZE, LIF_BLOCK_SIZE);
	
	return 0;	
}

#if 0
static char trackBuffer[DISK_DEFAULT_SECTORS_PER_TRACK*DISK_DEFAULT_SECTOR_SIZE] = {0};

int main(int argc, char** argv) {
	(void)argc;
	(void)argv;
	
	initScreen(0, WRITE_BLACK);
	
	int16_t n = rawReadWriteSectors(0,0,1,5,DISK_DEFAULT_SECTOR_SIZE_SELECT,trackBuffer,DRIVE_READ);
	printf("Read %d\n", n);
	printf("At 0: %08lx\n", *(uint32_t*)trackBuffer);
	printf("status %x %x %x\n", st0,st1,st2);
	printf("pos %x %x %x\n", finalTrack,finalSide,finalSector);
	
	getKey(1);
	reload(); 
	return 0;
}
#endif