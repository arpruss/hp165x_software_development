#include <stdlib.h>
#include <string.h>
#include "hp165x.h"

_WRAP_2(renameDirEntry,0xebc8);
_WRAP_3(_openFile,0xeb74);
_WRAP_1(closeFile,0xeb7a);
_WRAP_3(readFile,0xeb86);
_WRAP_3(writeFile,0xeb80);
//_WRAP_5(findDirEntry,0xeb98); // it's been hanging
_WRAP_2(_getDirEntry,0xebce);
//_WRAP_0(_refreshDir, 0xebb0);
_WRAP_1(_eb62, 0x227c);
_WRAP_0_RET_D1(getKeyBIOS,0xeb38);

void _eb62(int x);
int16_t _ebb0(void);

uint32_t _refreshDir_savedStack;

/* for unknown reasons, the stack runs into something important when calling 0xebb0 */
asm(".globl _refreshDir\n"
	"_refreshDir:\n"
	"  move.l  %SP,_refreshDir_savedStack\n"
    "  move.l  #0x983FFE,%SP\n"
    "  movem.l %d1-%d7/%a0-%a6, -(%sp)\n"
	"  jsr 0xebb0\n"
    "  movem.l (%SP)+,%d1-%d7/%a0-%a6\n"
	"  move.l _refreshDir_savedStack,%sp\n"
	"  rts\n");

int refreshDir(void) {
	if (*HARDWARE_STATUS & HARDWARE_STATUS_NO_DISK)
		return -1;
	if (*HARDWARE_STATUS & HARDWARE_STATUS_OLD_DISK)
		return 0;
	return _refreshDir();
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
	int type = _getDirEntry(index, &d);
	if (type == -1)
		return -1;
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
	return _openFile(paddedName, fileType, mode);
}

int deleteByNameAndType(const char* name, uint16_t fileType) {
	if (refreshDir() < 0)
		return -1;
	char paddedName[MAX_FILENAME_LENGTH];
	padFilename(paddedName, name);
	ROMDirEntry_t d;
	int i = 0;
	while(1) {
		if ( -1 == _getDirEntry(i, &d) ) {
			return -1;
		}
		if (!strncmp(d.name, paddedName, MAX_FILENAME_LENGTH) && (d.type == fileType || fileType==0)) {
			d.type = 0;
			renameDirEntry(i, (ROMNameAndType_t*)&d);
			return 0;
		}
		i++;
	}
}
