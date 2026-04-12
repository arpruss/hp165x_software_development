#include <stdlib.h>
#include <string.h>
#include "hp165x.h"

#define TOTAL_BLOCKS ((volatile uint32_t*)0x009842a6)

int32_t _renameDirEntry(uint32_t index, const ROMDirEntry_t* newEntry);
int _openFile(const char* filename, uint32_t fileType, uint32_t mode);
int _writeFile(int32_t fd, const void* data, int32_t size);
int _readFile(int32_t fd, void* data, int32_t size);
void _closeFile(int32_t fd);
int _getDirEntry(int index, ROMDirEntry_t* dirEntry);
void _diskPack(void);
int _commitDir(void);
int _writeBlocks(uint32_t blockNum, unsigned count, const void* data);
int _readBlocks(uint32_t blockNum, unsigned count, void* data);
int _readBlock(uint32_t blockNum, void* data);
int _commitBlocks(void);

uint8_t _dirtyDisk=1;

_WRAP_0(_initializeDiskSettings,0xec10);
_WRAP_0(_commitBlocks,0xec0a);
_WRAP_2(_writeBlock,0xebf8);
_WRAP_3(_writeBlocks,0xebbc);
_WRAP_3(_readBlocks,0xebc2);
_WRAP_2(_readBlock,0xebfe);
_WRAP_2(_renameDirEntry,0xebc8);
_WRAP_3(_openFile,0xeb74);
_WRAP_1(_closeFile,0xeb7a);
_WRAP_3(_readFile,0xeb86);
_WRAP_3(_writeFile,0xeb80);
//_WRAP_5(findDirEntry,0xeb98); // it's been hanging
_WRAP_2(_getDirEntry,0xebce);
_WRAP_0(_refreshDir, 0xebb0);
_WRAP_1(_eb62, 0x227c);
_WRAP_0(_packDir, 0xeb92);
_WRAP_0(_commitDir, 0xeb9e);

void _eb62(int x);

static uint8_t _savedAsteriskAreaData[4][15];
static uint32_t savedAsteriskAreaDepth = 0;

void  __attribute__ ((noinline, optimize("Os"))) _saveAsteriskArea(void) {
	if (savedAsteriskAreaDepth++ > 0) 
		return;
	
	uint8_t mask;
	mask = 1;
	volatile uint16_t* pos = SCREEN+592/4*14+71*8/4; 
	for (uint16_t bitplane=0; bitplane<4 ; bitplane++, mask<<=1) {
		*SCREEN_MEMORY_CONTROL = ~mask;
		volatile uint16_t* pos2 = pos;
		for (uint16_t y=0; y<15; y++, pos2 += 592/4) {
			_savedAsteriskAreaData[bitplane][y] = (*pos2&0xF) | (pos2[1]&0xF)<<4;
		}
	}
}

void __attribute__ ((noinline, optimize("Os"))) _restoreAsteriskArea(void) {
	if (--savedAsteriskAreaDepth > 0)
		return;
	
	volatile uint16_t* pos = SCREEN+592/4*14+71*8/4; 
	*SCREEN_MEMORY_CONTROL = 0xF00; // clear all
	volatile uint16_t* pos2 = pos;
	for (uint16_t y=0; y<15; y++, pos2 += 592/4) {
		*pos2 = pos2[1] = 0xF;
	}
	uint8_t mask;
	mask = 1;
	for (uint16_t bitplane=0; bitplane<4 ; bitplane++, mask<<=1) {
		*SCREEN_MEMORY_CONTROL = 0xF&~mask; // 0xF&~mask; // set
		volatile uint16_t* pos2 = pos;
		for (uint16_t y=0; y<15; y++, pos2 += 592/4) {
			*pos2 = _savedAsteriskAreaData[bitplane][y];
			pos2[1] = _savedAsteriskAreaData[bitplane][y] >> 4;
		}
	}
}

int writeBlocks(uint32_t blockNum, unsigned count, const void* data) {
	_saveAsteriskArea();
	int r = _writeBlocks(blockNum, count, data);
	if (r >= 0)
		r = _commitBlocks();
	_dirtyDisk = 1;
	_restoreAsteriskArea();
	return r;
}

int readBlocks(uint32_t blockNum, unsigned count, void* data) {
	_saveAsteriskArea();
	int r = _readBlocks(blockNum, count, data);
	_restoreAsteriskArea();
	return r;
}

int readBlock(uint32_t blockNum, void* data) {
	_saveAsteriskArea();
	int r = _readBlock(blockNum, data);
	_restoreAsteriskArea();
	return r;
}

void closeFile(int32_t h) {
	_saveAsteriskArea();
	_closeFile(h);
	_dirtyDisk = 1;
	_restoreAsteriskArea();
}

int writeFile(int32_t fd, const void* data, int32_t size) {
	_saveAsteriskArea();
	int n = _writeFile(fd,data,size);
	_dirtyDisk = 1;
	_restoreAsteriskArea();
	return n;
}

int readFile(int32_t fd, void* data, int32_t size) {
	_saveAsteriskArea();
	int n = _readFile(fd,data,size);
	_restoreAsteriskArea();
	return n;
}

int refreshDir(void) {
	if (*HARDWARE_STATUS & HARDWARE_STATUS_NO_DISK)
		return -1;
	_saveAsteriskArea();
	if (*HARDWARE_STATUS & HARDWARE_STATUS_OLD_DISK) {
		bigDiskSupport();
		_restoreAsteriskArea();
		return 0;
	}
	_dirtyDisk = 1;
	int retVal = _refreshDir();
	bigDiskSupport();
	_restoreAsteriskArea();
	return retVal;
}

void padFilename(char* paddedName, const char* name) {
	memset(paddedName, ' ', MAX_FILENAME_LENGTH);
	uint16_t l = strlen(name);
	if (l > MAX_FILENAME_LENGTH)
		l = MAX_FILENAME_LENGTH;
	memcpy(paddedName, name, l);
}

void unpadFilename(char* unpaddedName, const char* name) {
	const char* p = name+9;
	while (p > name && *p == ' ') p--;
	if (*p == ' ') {
		*unpaddedName = 0;
	}
	else {
		uint16_t length = p+1-name;
		memcpy(unpaddedName, name, length);
		unpaddedName[length] = 0;
	}
}

static void romDirEntryToDirEntry(ROMDirEntry_t* d, DirEntry_t* dirEntry) {
	unpadFilename(dirEntry->name, d->name);
	dirEntry->type = d->type;
	dirEntry->startBlock = d->startBlock;
	dirEntry->numBlocks = d->numBlocks;
	memcpy(&dirEntry->dateAndTime, &d->dateAndTime, sizeof(d->dateAndTime));
	memcpy(&dirEntry->misc, &d->misc, sizeof(d->misc));
}

int __attribute__((noinline)) getDirEntry(int index, DirEntry_t* dirEntry) {
	ROMDirEntry_t d;
	if (index == 0 && refreshDir() < 0)
		return -1;
	_saveAsteriskArea();
	int type = _getDirEntry(index, &d);
	_restoreAsteriskArea();
	if (type == -1) {
		return -1;
	}
	romDirEntryToDirEntry(&d, dirEntry);
	return type & 0xFFFF;
}

int openFile(const char* name, uint32_t fileType, uint32_t mode) {
	if (refreshDir() < 0)
		return -1;
	char paddedName[MAX_FILENAME_LENGTH];
	padFilename(paddedName, name);
	_saveAsteriskArea();
	int h = _openFile(paddedName, fileType, mode);
	_restoreAsteriskArea();
	if (mode == OPEN_WRITE)
		_dirtyDisk = 1;
	return h;
}

/*void _ebda();
void _ebe6();
void _ebec();
void _eb68();
_WRAP_0(_ebda,0xebda);
_WRAP_0(_ebe6,0xebe6);
_WRAP_0(_ebec,0xebec);
_WRAP_0(_eb68,0xeb68); */

/* if newFileType == -1, don't change fileType */
int renameFile(const char* name, uint16_t fileType, const char* newName, int32_t newFileType) {
	if (refreshDir() < 0)
		return -1;
	char paddedName[MAX_FILENAME_LENGTH];
	padFilename(paddedName, name);
	char newPaddedName[MAX_FILENAME_LENGTH];
	padFilename(newPaddedName, newName);
	ROMDirEntry_t d;
	int i = 0;
	_saveAsteriskArea();
	while(1) {
		if ( -1 == _getDirEntry(i, &d) ) {
			_restoreAsteriskArea();
			return -1;
		}
		if (d.type != 0 && !strncmp(d.name, paddedName, MAX_FILENAME_LENGTH) && (d.type == fileType || fileType==0)) {
			if (newFileType >= 0) 
				d.type = newFileType;
			memcpy(d.name, newPaddedName, MAX_FILENAME_LENGTH);
			_dirtyDisk = 1;
			_renameDirEntry(i, &d);
			int r = _commitDir();
			_restoreAsteriskArea();
			return r;
		}
		i++;
	}
}

int setFileMisc(const char* name, uint16_t fileType, const void* misc) {
	if (refreshDir() < 0)
		return -1;
	char paddedName[MAX_FILENAME_LENGTH];
	padFilename(paddedName, name);
	ROMDirEntry_t d;
	int i = 0;
	_saveAsteriskArea();
	while(1) {
		if ( -1 == _getDirEntry(i, &d) ) {
			_restoreAsteriskArea();
			return -1;
		}
		if (d.type != 0 && !strncmp(d.name, paddedName, MAX_FILENAME_LENGTH) && (d.type == fileType || fileType==0)) {
			memcpy(d.misc, misc, sizeof(d.misc));
			_dirtyDisk = 1;
			_renameDirEntry(i, &d);
			int r = _commitDir();
			_restoreAsteriskArea();
			return r;
		}
		i++;
	}
}

int getFileMisc(const char* name, uint16_t fileType, void* misc) {
	if (refreshDir() < 0)
		return -1;
	char paddedName[MAX_FILENAME_LENGTH];
	padFilename(paddedName, name);
	ROMDirEntry_t d;
	int i = 0;
	_saveAsteriskArea();
	while(1) {
		if ( -1 == _getDirEntry(i, &d) ) {
			_restoreAsteriskArea();
			return -1;
		}
		if (d.type != 0 && !strncmp(d.name, paddedName, MAX_FILENAME_LENGTH) && (d.type == fileType || fileType==0)) {
			memcpy(misc, d.misc, sizeof(d.misc));
			_restoreAsteriskArea();
			return 0;
		}
		i++;
	}
}

int findDirEntry(const char* name, uint16_t fileType, DirEntry_t* dirEntry) {
	if (refreshDir() < 0)
		return -1;
	char paddedName[MAX_FILENAME_LENGTH];
	padFilename(paddedName, name);
	ROMDirEntry_t d;
	int i = 0;
	_saveAsteriskArea();
	while(1) {
		if ( -1 == _getDirEntry(i, &d) ) {
			_restoreAsteriskArea();
			return -1;
		}
		if (d.type != 0 && !strncmp(d.name, paddedName, MAX_FILENAME_LENGTH) && (d.type == fileType || fileType==0)) {
			_restoreAsteriskArea();
			romDirEntryToDirEntry(&d, dirEntry);
			return d.type & 0xFFFF;
		}
		i++;
	}
}

int deleteByNameAndType(const char* name, uint16_t fileType) {
	return renameFile(name, fileType, name, 0);
}

int getFileType(const char* name) {
	if (refreshDir() < 0)
		return -1;
	char paddedName[MAX_FILENAME_LENGTH];
	padFilename(paddedName, name);
	ROMDirEntry_t d;
	int i = 0;
	_saveAsteriskArea();
	while(1) {
		if ( -1 == _getDirEntry(i, &d) ) {
			_restoreAsteriskArea();
			return 0;
		}
		if (!strncmp(d.name, paddedName, MAX_FILENAME_LENGTH) && d.type != 0) {
			_restoreAsteriskArea();
			return d.type;
		}
		i++;
	}
}

int diskSpace(uint32_t* totalBlocksP, uint32_t* freeBlocksP, uint32_t* largestSpaceP) {
	if (refreshDir()<0)
		return -1;
	
	uint32_t nonDataBlocks = *(uint32_t*)0x00984162 + *(uint32_t*)0x0098415a;
	uint32_t totalBlocks = *(uint16_t*)0x009842a6 - nonDataBlocks;
	uint32_t freeBlocks = 0;
	uint32_t largestSpace = 0;
	uint32_t lastBlock = nonDataBlocks;
	uint32_t space;
	
	DirEntry_t d;
	int i = 0;

	while (getDirEntry(i, &d) >= 0) {
		//printf("%u %.10s %u  %u:%u\n", i, d.name, d.type, d.startBlock, d.startBlock+d.numBlocks); 
		if (d.type != 0) {
			space = d.startBlock - lastBlock;
			freeBlocks += space;
			if (space > largestSpace)
				largestSpace = space;
			lastBlock = d.startBlock + d.numBlocks;
		}
		i++;
	}
	
	space = totalBlocks + nonDataBlocks - lastBlock;
	freeBlocks += space;
	
	if (space > largestSpace)
		largestSpace = space;
	if (totalBlocksP != NULL)
		*totalBlocksP = totalBlocks;
	if (freeBlocksP != NULL)
		*freeBlocksP = freeBlocks;
	if (largestSpaceP != NULL)
		*largestSpaceP = largestSpace;
	
	return 0;
}