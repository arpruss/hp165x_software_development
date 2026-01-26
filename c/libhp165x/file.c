#include <stdlib.h>
#include <string.h>
#include "hp165x.h"

_WRAP_2(renameDirEntry,0xebc8);
_WRAP_3(_openFile,0xeb74);
_WRAP_1(closeFile,0xeb7a);
_WRAP_3(readFile,0xeb86);
_WRAP_3(writeFile,0xeb80);
//_WRAP_5(findDirEntry,0xeb98); // it's been hanging
_WRAP_2(getDirEntry,0xebce);
_WRAP_0(_ebb0, 0xebb0);
_WRAP_1(_eb62, 0x227c);
_WRAP_0_RET_D1(getKeyBIOS,0xeb38);

void _eb62(int x);
int16_t _ebb0(void);

int refreshDir(void) {
	if (*HARDWARE_STATUS & HARDWARE_STATUS_OLD_DISK)
		return 0;
//	_eb62(0);
	return _ebb0();
}

int openFile(const char* name, uint32_t fileType, uint32_t mode) {
	char paddedName[MAX_FILENAME_LENGTH] = "          "; // 10
	int length = strlen(name);
	if (length > MAX_FILENAME_LENGTH)
		return ERROR_FILE_NOT_FOUND;
	memcpy(paddedName, name, strlen(name));
	return _openFile(paddedName, fileType, mode);
}

int deleteByNameAndType(const char* name, uint16_t fileType) {
	char paddedName[MAX_FILENAME_LENGTH] = "          "; // 10
	DirEntry_t d;
	int length = strlen(name);
	if (length > MAX_FILENAME_LENGTH)
		return ERROR_FILE_NOT_FOUND;
	memcpy(paddedName, name, strlen(name));
	int i = 0;
	while(1) {
		if ( -1 == getDirEntry(i, &d) ) {
			return -1;
		}
		if (!strncmp(d.name, paddedName, 10) && (d.type == fileType || fileType==0)) {
			d.type = 0;
			renameDirEntry(i, (NameAndType_t*)&d);
			return 0;
		}
		i++;
	}
}
