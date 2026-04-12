#ifndef _DRIVE_H
#define _DRIVE_H
#include <stdlib.h>
#include <hp165x.h>

#define MISC_CONTROL ((volatile uint8_t*)0x202001)
#define DRIVE_STATUS ((volatile uint8_t*)0x20d001)
#define DRIVE_STATUS_RQM 0x80
#define DRIVE_STATUS_EXM 0x20
#define DRIVE_DATA   ((volatile uint8_t*)0x20d003)
#define DRIVE_SEEK       	  0x0F
#define DRIVE_SENSE_INTERRUPT 0x08
#define DRIVE_READ			  0x46
#define DRIVE_WRITE			  0x45
#define DISK_DEFAULT_SECTORS_PER_TRACK     5
#define DISK_DEFAULT_SECTOR_SIZE_SELECT			  3
#define DISK_GPL		0x32
#define DISK_DEFAULT_SECTOR_SIZE      1024

#define DRIVE_ERROR  ((int16_t)-32768)

// These are not thoroughly tested.
int16_t driveSeek(uint8_t track, uint8_t side);
int16_t driveReadWriteSectors(uint8_t track, uint8_t side, uint8_t startSectorID, uint8_t endSectorID, uint8_t sectorSizeSelect, char* data, uint8_t mode);
uint8_t driveGetST0(void);
uint8_t driveGetST1(void);
uint8_t driveGetST2(void);
void driveGetFinalPosition(uint8_t* trackP, uint8_t* sideP, uint8_t* sectorP);
int16_t driveReadBlock(uint16_t blockNum, char* data);

#endif