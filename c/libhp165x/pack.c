//#define TEST

#include <stdint.h>
#include <malloc.h>
#include <string.h>

#define BLOCK_SIZE 256
#define RETRY 4
#define MAGIC_BLOCK (((79 * 2 + 0) * 5 + 1) * 4)
#define MAGIC_BLOCKS 4
#define MAGIC_TYPE 0xFEEF
#undef GET_INFO_FROM_DISK

#define DEBUG(fmt, ...) 

#ifdef TEST
#define GET_INFO_FROM_DISK
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

int writeBlock(uint32_t startBlock, const void* p) {
	return writeBlocks(startBlock, 1, p);
}

#else
#include "hp165x.h"
#include "screensize.h"

#define FIX16(x) ((x))
#define FIX32(x) ((x))




#endif

#define DIRENTRIES_PER_BLOCK (BLOCK_SIZE / sizeof(ROMDirEntry_t))

static const ROMDirEntry_t magicEntry = {
	"|RESERVED|",
	MAGIC_TYPE,
	FIX32(MAGIC_BLOCK),
	FIX32(MAGIC_BLOCKS),
	"\x90\x01\x01\x01\x01\x01",
	"\x80\x01RSVD"
};

static uint32_t bufferSize;
static uint32_t bufferBlocks;
static char* buffer;
static uint32_t dirStartBlock;
static uint32_t dirBlocks;
static uint32_t totalBlocks;
static uint32_t lastUsedBlock;
static char progress;
static ROMDirEntry_t* dir;
static ROMDirEntry_t* endDir;
static uint16_t progressX;
static char bigDisk;
static int32_t magicDirPosition;

#define MIN_BUFFER_SIZE 2048
#define MAX_BUFFER_SIZE 65536

static int writeBlocksRetry(uint32_t startBlock, uint32_t count, const void* p) {
	for (int i=0; i<RETRY; i++) {
		if (writeBlocks(startBlock, count, p) >= 0)
			return 0;
	}
	return -1;
}

static int writeBlockRetry(uint32_t startBlock, const void* p) {
	return writeBlocksRetry(startBlock, 1, p);
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

void clearProgress(uint8_t* buffer) {
	if (!progress)
		return;
	volatile uint16_t* pos = SCREEN + (screenHeight-1)*SCREEN_WIDTH_WORDS;
	volatile uint16_t* posSaved = pos;
	uint8_t* bufferSaved = buffer;
	*SCREEN_MEMORY_CONTROL = WRITE_SET_ATTR;
	for (uint16_t i=SCREEN_WIDTH_WORDS ; i > 0 ; i--) {
		*pos++ = *buffer++;
	}
	pos = posSaved;
	buffer = bufferSaved;
	*SCREEN_MEMORY_CONTROL = WRITE_CLEAR_ATTR;
	for (uint16_t i=SCREEN_WIDTH_WORDS ; i > 0 ; i--) {
		*pos++ = ~(*buffer++);
	}
	*SCREEN_MEMORY_CONTROL = WRITE_WHITE;
}

static void updateProgress(uint16_t block) {
	if (!progress)
		return;
	uint16_t total = totalBlocks;
	uint16_t dataStart = dirStartBlock + dirBlocks;
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

static void initProgress(uint8_t* buffer) {
	if (!progress)
		return;
	*SCREEN_MEMORY_CONTROL = WRITE_CLEAR_ATTR;
	volatile uint16_t* pos = SCREEN + (screenHeight-1)*SCREEN_WIDTH_WORDS;
	for (uint16_t i=SCREEN_WIDTH_WORDS ; i > 0 ; i--) {
		*buffer++ = (uint8_t)*pos++;
	}
	drawHorizontalLine(0,screenHeight-1,SCREEN_WIDTH-1);
	*SCREEN_MEMORY_CONTROL = WRITE_WHITE;
	progressX = 0;
	*SCREEN_MEMORY_CONTROL = WRITE_SET_ATTR;
	drawPixel(0,screenHeight-1);
}

static char isMagicFile(ROMDirEntry_t* d, char deleted) {
	if (!bigDisk)
		return 0;
	if (deleted && d->type != 0)
		return 0;
	if (!deleted && d->type != 0xFEEF)
		return 0;
	return 0==memcmp(d->misc, magicEntry.misc, 6);
}

static int16_t prepareDir(void) {
	ROMDirEntry_t* src = dir;
	ROMDirEntry_t* dest = dir;
	
	lastUsedBlock = 0;
	
	uint16_t changed = 0;
	uint32_t projectedBlockPos = dirStartBlock + dirBlocks;
	uint32_t entryCount = 0;
	magicDirPosition = -1;
	
	while (src < endDir) {
		if (src->name[0] != (char)0xFF && src->type != 0 && !isMagicFile(src, 0)) {
			DEBUG("ARP src=%d [%.10s] dest=%d\n", src-dir, src->name, dest-dir);
			lastUsedBlock = FIX32(src->startBlock) + FIX32(src->numBlocks);
			projectedBlockPos += FIX32(src->numBlocks);
			if (bigDisk) {
				if (magicDirPosition < 0 && projectedBlockPos >= MAGIC_BLOCK) {
					magicDirPosition = entryCount;
				}
				// corrupt disk: non-magic file overlaps magic
				if (lastUsedBlock > MAGIC_BLOCK && FIX32(src->startBlock) < MAGIC_BLOCK + MAGIC_BLOCKS)
					return -1;
			}
			entryCount++;
			if (dest != src) {
				*dest = *src;
				memset(src, 0xFF, sizeof(ROMDirEntry_t));				
				changed = 1;
			}
			dest++;
		}
		else if (!allFF(src)) {
			DEBUG("ARP src=%d erasing\n", src-dir);
			memset(src, 0xFF, sizeof(ROMDirEntry_t));
			changed = 1;
		}
		src++;
	}
	
	if (bigDisk) {
		DEBUG("Before insertion:count=%d", entryCount);
		if (magicDirPosition < 0) {
			magicDirPosition = entryCount;
		}
		DEBUG("ARP magicDirPosition=%d\n", magicDirPosition);
		if (entryCount >= dirBlocks * DIRENTRIES_PER_BLOCK)
			return -1;
		DEBUG("ARP inserting\n");
		memmove(dir+magicDirPosition+1, dir+magicDirPosition, (entryCount-magicDirPosition)*sizeof(ROMDirEntry_t));
		dir[magicDirPosition] = magicEntry;
		dir[magicDirPosition].type = 0; // temporarily marked as deleted
		changed = 1;
		endDir = dest+1;
		DEBUG("AFter insertion:count=%d", endDir-dir);
	}
	else {
		endDir = dest;
	}
	
	DEBUG("ARP dir has %u entries\n", (endDir-dir));
	DEBUG("ARP changed %d\n", changed);
	
	if (changed && writeBlocksRetry(dirStartBlock, dirBlocks, dir) < 0) {
		DEBUG("dir failed\n");
		return -1;
	}
	
	updateProgress(dirStartBlock+dirBlocks);
	
	return 0;
} 

int moveData(uint32_t destBlock, uint32_t srcBlock, uint32_t numBlocks) {
#ifdef TEST	
	DEBUG("moveData %u %u %u\n",destBlock,srcBlock,numBlocks);
#endif	
	while (numBlocks > 0) {
		uint16_t toCopy = numBlocks;
		if (toCopy > bufferBlocks)
			toCopy = bufferBlocks;
		if (readBlocksRetry(srcBlock, toCopy, buffer) < 0)
			return -1;
		char* b = buffer;
		while (toCopy > 0) {
			if (writeBlockRetry(destBlock, b) < 0)
				return -1;
			b += BLOCK_SIZE;
			toCopy--;
			srcBlock++;
			destBlock++;
			numBlocks--;
			updateProgress(destBlock);
		}
	}
	return 0;
}

static int packDiskData(void) {
	uint32_t dest = dirStartBlock + dirBlocks;
	
	ROMDirEntry_t* d = dir;
	
	while (d < endDir) {
		char updateDirEntry = 0;
		DEBUG("packing %d %d <%.10s>\n", d-dir, endDir-d, d->name);
		if (isMagicFile(d, 1)) {
			DEBUG("magic!\n");
			dest = MAGIC_BLOCK + MAGIC_BLOCKS;
			d->type = MAGIC_TYPE;
			updateDirEntry = 1;
		}
		else {
			uint32_t start = FIX32(d->startBlock);
			uint32_t num = FIX32(d->numBlocks);		
			if (start != dest) {
				if (moveData(dest, start, num) < 0)
					return -1;
#ifdef TEST			
				DEBUG("moved %u blocks from %u to %u\n", num, start, dest);
#endif			
				d->startBlock = FIX32(dest);
				updateDirEntry = 1;
			}
			else {
				updateProgress(start + num);
			}
			dest += num;
		}
		if (updateDirEntry) {
			uint32_t dirBlock = ((char*)d - (char*)dir) / BLOCK_SIZE;
			DEBUG("ARP Writing one block at %u", dirStartBlock + dirBlock);
			if (writeBlocksRetry(dirStartBlock + dirBlock, 1, (char*)dir + dirBlock * BLOCK_SIZE) < 0)
				return -1;
		}
		d++;
	}
	
	return 0;
}

static int _lifPack(void) {
	DEBUG("ARP: lifpack\n");
	if (refreshDir()<0)
		return -1;
	
//	bigDisk = (*(volatile char*)(0x984152+12) == 'b');
//	DEBUG("ARP: bigDisk=%d\n", bigDisk);

	char success = 0;

#ifdef GET_INFO_FROM_DISK
	uint8_t* blockZero = malloc(BLOCK_SIZE);

	if (blockZero == NULL)
		return -1;

	if (readBlocksRetry(0, 1, blockZero) < 0) {
		free(blockZero);
		return -1;
	}
	dirStartBlock = FIX32(*(uint32_t*)(blockZero+8));
	dirBlocks = FIX32(*(uint32_t*)(blockZero+16));
	/* sides * tracks * blocksPerTrack */
	totalBlocks = FIX32(*(uint32_t*)(blockZero+24)) * FIX32(*(uint32_t*)(blockZero+28)) * FIX32(*(uint32_t*)(blockZero+32));
	
	free(blockZero);
#else
	dirStartBlock = 2; // TODO
	dirBlocks = *(volatile uint32_t*)0x984162;
	totalBlocks = 1+*(uint16_t*)0x9842a8;
#endif	

	bigDisk = totalBlocks > 2*80*20;
	
	dir = malloc(dirBlocks * BLOCK_SIZE);
	
	if (dir == NULL)
		return -1;
	
	if (readBlocksRetry(dirStartBlock, dirBlocks, dir) < 0)
		goto cleanup;
	
	endDir = dir + dirBlocks * (BLOCK_SIZE / sizeof(ROMDirEntry_t));
	
	if (prepareDir() < 0)
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
	uint32_t totalBlocks;
	if (diskSpace(&totalBlocks, NULL, NULL)<0)
		return -1;
//	if (totalBlocks>=3200) 
//		return -1; // TODO: bigdisk mode is not yet implemented
	
	uint8_t progressBuffer[SCREEN_WIDTH/4];
	progress = _progress;
	initProgress(progressBuffer);
	_saveAsteriskArea();
	int r = _lifPack();
	_restoreAsteriskArea();
	clearProgress(progressBuffer);
	return r;
}


#ifdef TEST
main(int argc, char** argv) {
	testFile = fopen(argv[1], "r+");
	if (_lifPack()<0)
		DEBUG("Fail!\n");
	fclose(testFile);
}
#endif