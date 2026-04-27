#ifndef _HPFILE_H
#define _HPFILE_H

#define OPEN_READ 1
#define OPEN_WRITE 2
#define MAX_FILENAME_LENGTH 10
#define LIF_BLOCK_SIZE 256
#define NUM_DIR_BLOCKS ((uint32_t*)0x984162)
#define FIRST_DIR_ENTRY ((ROMDirEntry_t*)0x9808ac)

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
void  __attribute__ ((noinline)) _saveAsteriskArea(void);
void  __attribute__ ((noinline)) _restoreAsteriskArea(void);
int writeBlocks(uint32_t blockNum, unsigned count, const void* data);
int readBlocks(uint32_t blockNum, unsigned count, void* data);
int renameFile(const char* name, uint16_t fileType, const char* newName, int32_t newFileType);
int openFile(const char* filename, uint32_t fileType, uint32_t mode);
int writeFile(int32_t fd, const void* data, int32_t size);
int readFile(int32_t fd, void* data, int32_t size);
void closeFile(int32_t fd);
//int findDirEntry(const char*filename, uint32_t type, DirEntry_t* dirEntry,uint32_t startIndex, uint32_t nameLength);//it's been hanging
int getDirEntry(int index, DirEntry_t* dirEntry); 
int _getDirEntry(int index, ROMDirEntry_t* dirEntry); 
int deleteByNameAndType(const char* name, uint16_t fileType);
int getFileType(const char* name);
int lifPack(char progress);
int diskSpace(uint32_t* totalBlocksP, uint32_t* freeBlocksP, uint32_t* largestSpaceP);
int _commitBlocks(void);
void bigDiskSupport(void);
void _initializeDriveSettings(void);
int findDirEntry(const char* name, uint16_t fileType, DirEntry_t* dirEntry);
int readBlock(uint32_t blockNum, void* data);
ROMDirEntry_t* getROMDirEntry(uint16_t index);
void unpadFilename(char* unpaddedName, const char* name);
int padFilename(char* paddedName, const char* name);

#define ERROR_FILE_NOT_FOUND (-5)
#endif