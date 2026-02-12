#include <stdlib.h>
#include <string.h>
#include "hp165x.h"

void _renameDirEntry(uint32_t index, const ROMNameAndType_t* newEntry);
int _openFile(const char* filename, uint32_t fileType, uint32_t mode);
int _writeFile(int32_t fd, const void* data, int32_t size);
int _readFile(int32_t fd, void* data, int32_t size);
void _closeFile(int32_t fd);
int _getDirEntry(int index, ROMDirEntry_t* dirEntry);

_WRAP_2(_renameDirEntry,0xebc8);
_WRAP_3(_openFile,0xeb74);
_WRAP_1(_closeFile,0xeb7a);
_WRAP_3(_readFile,0xeb86);
_WRAP_3(_writeFile,0xeb80);
//_WRAP_5(findDirEntry,0xeb98); // it's been hanging
_WRAP_2(_getDirEntry,0xebce);
_WRAP_0(_refreshDir, 0xebb0);
_WRAP_1(_eb62, 0x227c);

void _eb62(int x);

static uint8_t _savedAsteriskAreaData[4][15];

void _saveAsteriskArea(void) {
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

void _restoreAsteriskArea(void) {
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

void closeFile(int h) {
	_saveAsteriskArea();
	_closeFile(h);
	_restoreAsteriskArea();
}

int writeFile(int32_t fd, const void* data, int32_t size) {
	_saveAsteriskArea();
	int n = _writeFile(fd,data,size);
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
	if (*HARDWARE_STATUS & HARDWARE_STATUS_OLD_DISK)
		return 0;
	_saveAsteriskArea();
	int retVal = _refreshDir();
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

int getDirEntry(int index, DirEntry_t* dirEntry) {
	ROMDirEntry_t d;
	if (index == 0 && refreshDir() < 0)
		return -1;
	_saveAsteriskArea();
	int type = _getDirEntry(index, &d);
	_restoreAsteriskArea();
	if (type == -1) {
		return -1;
	}
	unpadFilename(dirEntry->name, d.name);
	dirEntry->type = d.type;
	dirEntry->startBlock = d.startBlock;
	dirEntry->numBlocks = d.numBlocks;
	memcpy(&dirEntry->dateAndTime, &d.dateAndTime, sizeof(d.dateAndTime));
	memcpy(&dirEntry->misc, &d.misc, sizeof(d.misc));
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
	return h;
}

int deleteByNameAndType(const char* name, uint16_t fileType) {
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
		if (!strncmp(d.name, paddedName, MAX_FILENAME_LENGTH) && (d.type == fileType || fileType==0)) {
			d.type = 0;
			_renameDirEntry(i, (ROMNameAndType_t*)&d);
			_restoreAsteriskArea();
			return 0;
		}
		i++;
	}
}

