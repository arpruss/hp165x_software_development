#ifndef _HPFILE_H
#define _HPFILE_H

#define OPEN_READ 1
#define OPEN_WRITE 2
#define MAX_FILENAME_LENGTH 10

typedef struct {
	char name[MAX_FILENAME_LENGTH]; // space padded
	uint16_t type;
	uint32_t startBlock;
	uint32_t numBlocks;
	uint8_t dateAndTime[6];
	uint8_t misc[6];
} ROMDirEntry_t;

typedef struct {
	char name[MAX_FILENAME_LENGTH+1]; // no padding
	uint16_t type;
	uint32_t startBlock;
	uint32_t numBlocks;
	uint8_t dateAndTime[6];
	uint8_t misc[6];
} DirEntry_t;

#define TYPE_EXE 0xC001
int renameFile(const char* name, uint16_t fileType, const char* newName, uint16_t newFileType);
int openFile(const char* filename, uint32_t fileType, uint32_t mode);
int writeFile(int32_t fd, const void* data, int32_t size);
int readFile(int32_t fd, void* data, int32_t size);
void closeFile(int32_t fd);
//int findDirEntry(const char*filename, uint32_t type, DirEntry_t* dirEntry,uint32_t startIndex, uint32_t nameLength);//it's been hanging
int getDirEntry(int index, DirEntry_t* dirEntry); 
int _getDirEntry(int index, ROMDirEntry_t* dirEntry); 
int deleteByNameAndType(const char* name, uint16_t fileType);
int getFileType(const char* name);

#define ERROR_FILE_NOT_FOUND (-5)
#endif