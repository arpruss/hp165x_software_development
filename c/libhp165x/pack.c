//
// NOT WORKING ON ACTUAL DEVICE: cannot write dir
//

//#define TEST

#include <stdint.h>
#include <malloc.h>
#include <string.h>

#define BLOCK_SIZE 256
#define RETRY 4

#ifdef TEST
#include <stdio.h>

#define MAX_FILENAME_LENGTH 10

int _refreshDir(void) {
	return 0;
}

typedef struct {
	char name[MAX_FILENAME_LENGTH]; // space padded
	uint16_t type;
	uint32_t startBlock;
	uint32_t numBlocks;
	uint8_t dateAndTime[6];
	uint8_t misc[6];
} ROMDirEntry_t;

FILE* testFile;

uint32_t FIX32(uint32_t x) {
	return (x << 24) | ((x & 0xFF00) << 8) | ((x & 0xFF0000) >> 8) | (x >> 24);
}

uint32_t FIX16(uint32_t x) {
	return (x << 8) | (x >> 8);
}

void _saveAsteriskArea(void) {}
void _restoreAsteriskArea(void) {}
int commitBlocks(void) { return 0; }

int readBlocks(uint32_t startBlock, uint32_t count, void* p) {
	if (fseek(testFile, startBlock * BLOCK_SIZE, SEEK_SET) < 0)
		return -1;
	if (count != fread(p, BLOCK_SIZE, count, testFile))
		return -1;
	return 0;
}

int writeBlocks(uint32_t startBlock, uint32_t count, const void* p) {
	if (fseek(testFile, startBlock * BLOCK_SIZE, SEEK_SET) < 0) {
		return -1;
	}
	if (count != fwrite(p, BLOCK_SIZE, count, testFile)) {
		return -1;
	}
	return 0;
}

#else
#include "hp165x.h"
#include "screensize.h"

#define FIX16(x) ((x))
#define FIX32(x) ((x))
#endif

static uint32_t bufferSize;
static uint32_t bufferBlocks;
static char* buffer;
static uint32_t dirStart;
static uint32_t dirBlocks;
static uint32_t totalBlocks;
static uint32_t lastUsedBlock;
static char progress;
static ROMDirEntry_t* dir;
static ROMDirEntry_t* endDir;
static uint16_t progressX;

#define MIN_BUFFER_SIZE 2048
#define MAX_BUFFER_SIZE 65536

static int writeBlocksRetry(uint32_t startBlock, uint32_t count, const void* p) {
	for (int i=0; i<RETRY; i++) {
		if (writeBlocks(startBlock, count, p) >= 0)
			return 0;
	}
	return -1;
}

static int readBlocksRetry(uint32_t startBlock, uint32_t count, void* p) {
	for (int i=0; i<RETRY; i++) {
		if (readBlocks(startBlock, count, p) >= 0)
			return 0;
	}
	return -1;
}


static char allFF(ROMDirEntry_t* p) {
	char* q = (char*)p;
	for (uint16_t i = 0 ; i < sizeof(ROMDirEntry_t) ; i++)
		if (q[i] != (char)0xFF)
			return 0;
	return 1;
}

void clearProgress(void) {
	if (!progress)
		return;
	*SCREEN_MEMORY_CONTROL = WRITE_CLEAR_ATTR;
	drawHorizontalLine(0,screenHeight-1,SCREEN_WIDTH-1);
	*SCREEN_MEMORY_CONTROL = WRITE_WHITE;
}

static void updateProgress(uint16_t block) {
	if (!progress)
		return;
	uint16_t total = totalBlocks;
	uint16_t dataStart = dirStart + dirBlocks;
	uint16_t dirPortion = dataStart * (SCREEN_WIDTH - 1) / total;
	uint16_t dataPortion = SCREEN_WIDTH - 1 - dirPortion;
	uint16_t x;
	if (block <= dataStart) {
		/* usedBlocks still not valid */
		x = dirPortion * block / dataStart;
	}
	else {
		uint16_t used = lastUsedBlock - dataStart;
		x = dirPortion + dataPortion * (block - dataStart) / used;
	}
	if (x > progressX) {
		*SCREEN_MEMORY_CONTROL = WRITE_SET_ATTR;
		drawHorizontalLine(progressX+1,screenHeight-1,x);
		progressX = x;
	}
}

static void initProgress(void) {
	if (!progress)
		return;
	clearProgress();
	progressX = 0;
	*SCREEN_MEMORY_CONTROL = WRITE_SET_ATTR;
	drawPixel(0,screenHeight-1);
}

static int16_t cleanupDir(void) {
	uint32_t lastChangedBlock = 0;
	ROMDirEntry_t* src = dir;
	ROMDirEntry_t* dest = dir;
	
	lastUsedBlock = 0;
	
	while (src < endDir) {
		if (src->name[0] != (char)0xFF && src->type != 0) {
			lastUsedBlock = src->startBlock + src->numBlocks;
			if (dest != src) {
				*dest = *src;
				lastChangedBlock = (src-dir) / (BLOCK_SIZE / sizeof(ROMDirEntry_t));
				memset(src, 0xFF, sizeof(ROMDirEntry_t));
			}
			dest++;
		}
		else if (!allFF(src)) {
			lastChangedBlock = (src-dir) / (BLOCK_SIZE / sizeof(ROMDirEntry_t));
			memset(src, 0xFF, sizeof(ROMDirEntry_t));
		}
		src++;
	}
	
	endDir = dest;
#ifdef TEST
	printf("dir has %u entries\n", (endDir-dir));
#endif	
	
//	uint32_t blocksToWrite = (((char*)endDir - (char*)dir) + BLOCK_SIZE - 1 ) / BLOCK_SIZE;
	
	if (writeBlocksRetry(dirStart, lastChangedBlock + 1, dir) < 0)
		return -1;
	
	updateProgress(dirStart+dirBlocks);
	
	return 0;
}

int moveData(uint32_t destBlock, uint32_t srcBlock, uint32_t numBlocks) {
#ifdef TEST	
	printf("moveData %u %u %u\n",destBlock,srcBlock,numBlocks);
#endif	
	while (numBlocks > 0) {
		uint16_t toCopy = numBlocks;
		if (toCopy > bufferBlocks)
			toCopy = bufferBlocks;
		if (readBlocksRetry(srcBlock, toCopy, buffer) < 0)
			return -1;
		if (writeBlocksRetry(destBlock, toCopy, buffer) < 0)
			return -1;
		updateProgress(destBlock+toCopy);
		srcBlock += toCopy;
		destBlock += toCopy;
		numBlocks -= toCopy;
	}
	return 0;
}

static int packDiskData(void) {
	uint32_t dest = dirStart + dirBlocks;
	
	ROMDirEntry_t* d = dir;
	
	while (d < endDir) {
#ifdef TEST
		printf("packing %.10s\n", d->name);
#endif		
		uint32_t start = FIX32(d->startBlock);
		uint32_t num = FIX32(d->numBlocks);
		if (start != dest) {
			if (moveData(dest, start, num) < 0)
				return -1;
#ifdef TEST			
			printf("moved %u blocks from %u to %u\n", num, start, dest);
#endif			
			d->startBlock = FIX32(dest);
			uint32_t dirBlock = ((char*)d - (char*)dir) / BLOCK_SIZE;
			if (writeBlocksRetry(dirStart + dirBlock, 1, (char*)dir + dirBlock * BLOCK_SIZE) < 0)
				return -1;
		}
		else {
			updateProgress(start + num);
		}
		dest += num;
		d++;
	}
	
	return 0;
}

static int _lifPack(void) {
	if (refreshDir()<0)
		return -1;
	
	uint8_t* blockZero = malloc(BLOCK_SIZE);
	char success = 0;
	
	if (blockZero == NULL)
		return -1;
	if (readBlocksRetry(0, 1, blockZero) < 0) {
		free(blockZero);
		return -1;
	}
	dirStart = FIX32(*(uint32_t*)(blockZero+8));
	dirBlocks = FIX32(*(uint32_t*)(blockZero+16));
	/* sides * tracks * blocksPerTrack */
	totalBlocks = FIX32(*(uint32_t*)(blockZero+24)) * FIX32(*(uint32_t*)(blockZero+28)) * FIX32(*(uint32_t*)(blockZero+32));
	
	free(blockZero);
	
	dir = malloc(dirBlocks * BLOCK_SIZE);
	
	if (dir == NULL)
		return -1;
	
	if (readBlocksRetry(dirStart, dirBlocks, dir) < 0)
		goto cleanup;
	
	endDir = dir + dirBlocks * (BLOCK_SIZE / sizeof(ROMDirEntry_t));
	
	if (cleanupDir() < 0)
		goto cleanup;
	
	bufferSize = MAX_BUFFER_SIZE;
	do {
		buffer = malloc(bufferSize);
		if (buffer != NULL)
			break;
		bufferSize /= 2;
	} while (bufferSize >= MIN_BUFFER_SIZE);
	
	if (buffer == NULL)
		goto cleanup;
	
	bufferBlocks = bufferSize / 256;
	
	success = (packDiskData() >= 0);
	
	free(buffer);

cleanup:
	free(dir);
	
	_refreshDir();
	
	if (!success)
		return -1;
	return 0;
}

int lifPack(char _progress) {
	progress = _progress;
	initProgress();
	_saveAsteriskArea();
	int r = _lifPack();
	_restoreAsteriskArea();
	clearProgress();
	return r;
}


#ifdef TEST
main(int argc, char** argv) {
	testFile = fopen(argv[1], "r+");
	if (_lifPack()<0)
		printf("Fail!\n");
	fclose(testFile);
}
#endif