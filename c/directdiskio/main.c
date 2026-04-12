#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stddef.h>
#include <hp165x.h>


static char trackBuffer[DISK_DEFAULT_SECTORS_PER_TRACK*DISK_DEFAULT_SECTOR_SIZE] = {0};

int main(int argc, char** argv) {
	(void)argc;
	(void)argv;
	
	initScreen(0, WRITE_BLACK);
	
	int16_t n = driveReadWriteSectors(0,0,1,5,DISK_DEFAULT_SECTOR_SIZE_SELECT,trackBuffer,DRIVE_READ);
	printf("Read %d\n", n);
	printf("At 0: %08lx\n", *(uint32_t*)trackBuffer);
	printf("status %x %x %x\n", driveGetST0(),driveGetST1(),driveGetST2());
	uint8_t finalTrack,finalSide,finalSector;
	driveGetFinalPosition(&finalTrack,&finalSide,&finalSector);
	printf("pos %x %x %x\n", finalTrack,finalSide,finalSector);
	
	getKey(1);
	reload(); 
	return 0;
}
