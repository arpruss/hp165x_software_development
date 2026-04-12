#include <hp165x.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
//#include <errno.h> //TODO!

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <wchar.h>
#include <malloc.h>

#include "hpposix.h"

#define LIFPACK // pack disk if not enough space

#define DEFAULT_CHUNK_SIZE 4096

#define LIF_BLOCK_DATA_SIZE (LIF_BLOCK_SIZE - 2)

#define MAX_FILES 6
#define DEFAULT_FILE_TYPE 1
#define FD_OFFSET 3

#ifndef SEEK_SET
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#endif

#define DISK_UNCHANGED        0x9107

#define MODE_READ    		  0
#define MODE_READ_WRITE		  1
#define MODE_WRITE            2

//#define readBlock driveReadBlock

/* The files are entirely stored in memory. 
   This is a kludge to get around the fact that I don't know how to seek
   within the HP's files, and the HP has a limit of one open file at a time. 
   But, hey, there is 1mb of RAM. */
   
extern void (*_posixCleanup)(void);
static void posixCleanup(void);

struct filesize {
	uint16_t id; // 'sz'
	uint32_t size;
};

struct chunk {
	uint32_t offset;
	struct chunk* next;
	uint32_t chunkSize;
	char data[];
};

typedef struct {
	char filename[MAX_FILENAME_LENGTH+1];
	uint8_t dirty;
	uint16_t fileType;
	struct chunk* currentChunk;
	struct chunk firstChunk;
} WriteData_t;

typedef struct {
	uint32_t startBlock;
	uint32_t numBlocks;
	uint32_t currentBlock;
	uint32_t blockOffset;
	uint32_t fileSize;
	struct {
		uint16_t size;
		char     data[LIF_BLOCK_DATA_SIZE];
	} buffer;
	uint16_t fileType;
	char     filename[MAX_FILENAME_LENGTH+1];
	char	 sizeIsExact;
} ReadData_t;

typedef struct {
	uint16_t mode;
	uint32_t position;
	uint32_t dataSize;
	union {
		WriteData_t* writeData;
		ReadData_t* readData;
		void* data;
	} rw;
} HPFILE;

static HPFILE files[MAX_FILES] = { {0} };

static uint16_t fromHex(const char* p) {
	uint16_t out = 0;
	while(1) {
		if ('0' <= *p && *p <= '9')
			out = out * 0x10 + (*p - '0');
		else if ('a' <= *p && *p <= 'f') 
			out = out * 0x10 + (*p + (int16_t)(0xA-'a'));
		else if ('A' <= *p && *p <= 'F') 
			out = out * 0x10 + (*p + (int16_t)(0xA-'A'));
		else {
			return out;
		}
		p++;
	}
}

/* specify type with ":type" (in hex) */
static uint16_t getHpName(char* hpName, const char* name) {
	uint16_t i,j;
	uint16_t fileType = 0;
	
	for (i=j=0; name[i]; i++) {
		if (name[i] == ':') {
			fileType = fromHex(name+i+1);
			break;
		}
		else if (j < MAX_FILENAME_LENGTH)
			hpName[j++] = name[i];
	}
	hpName[j] = 0;
	return fileType;
}

static int checkDiskChange(HPFILE* f) {
	DirEntry_t d;
	refreshDir();
	if (*(volatile uint16_t*)0x984154 != DISK_UNCHANGED) {
		for (int i=0;i<MAX_FILES;i++) {
			if (files[i].rw.data != NULL && files[i].mode == MODE_READ) {
				files[i].rw.readData->startBlock = (uint32_t)(-1);
			}
		}
		*(volatile uint16_t*)0x984154 = DISK_UNCHANGED;
	}
	if (f == NULL || f->mode != MODE_READ)
		return 0;
	ReadData_t* r = f->rw.readData;
	if (r->startBlock != (uint32_t)(-1))
		return 0;
	if (findDirEntry(r->filename, r->fileType, &d) < 0)
		return -1;
	if (d.numBlocks != r->numBlocks) // different disk with same file?
		return -1;
	r->currentBlock = (uint32_t)(-1);
	r->startBlock = d.startBlock;
	return 0;
}

int open(const char* name, int flags, ...) {
	DirEntry_t d;
	
	char hpName[MAX_FILENAME_LENGTH+1];
	uint16_t fileType = getHpName(hpName, name);
	int fd;
	
	if ((flags & O_RDWR) && !(flags & O_TRUNC)) {
		DirEntry_t d;
		if (findDirEntry(hpName, fileType, &d) < 0)
			return -1;
		fd = open(name, O_RDONLY);
		if (fd < 0)
			return fd;
		int size = lseek(fd, 0, SEEK_END);
		if (size < 0 || lseek(fd, 0, SEEK_SET) < 0) {
			close(fd);
			return -1;
		}
		WriteData_t *w = malloc(sizeof(WriteData_t)+size);
		w->firstChunk.chunkSize = size;
		if (w == NULL) {
			close(fd);
			return -1;
		}
		size = read(fd, w->firstChunk.data, size);
		close(fd);
		if (size < 0) {
			free(w);
			return -1;
		}
		fd -= FD_OFFSET; // reuse FD
		w->firstChunk.chunkSize = size;
		w->firstChunk.next = NULL;
		w->firstChunk.offset = 0;
		w->dirty = 0;
		w->currentChunk = &(w->firstChunk);
		strcpy(w->filename, hpName);
		w->fileType = d.type;
		files[fd].rw.writeData = w;
		files[fd].position = size;
		files[fd].mode = MODE_READ_WRITE;
		files[fd].dataSize = size;
		_posixCleanup = posixCleanup; /* close on _exit() */
		return fd + FD_OFFSET;
	}
		
	for (fd=0;fd<MAX_FILES;fd++) {
		if (files[fd].rw.data == NULL)
			break;
	}
	if (fd >= MAX_FILES) {
        return -1;		
	}

	if ((flags & O_WRONLY) || (flags & O_RDWR)) { // If we have O_RDWR here, we also have O_TRUNC
		WriteData_t *w = malloc(sizeof(WriteData_t)+DEFAULT_CHUNK_SIZE);
		if (w == NULL) {
			return -1;
		}

		strcpy(w->filename,hpName);
		w->fileType = fileType == 0 ? DEFAULT_FILE_TYPE : fileType;
		w->firstChunk.next = NULL;
		w->firstChunk.offset = 0;
		w->firstChunk.chunkSize = DEFAULT_CHUNK_SIZE;
		w->currentChunk = &(w->firstChunk);
		w->dirty = 0 != (flags & O_TRUNC);

		files[fd].rw.writeData = w;
		files[fd].position = 0;
		files[fd].mode = (flags & O_RDWR) ? MODE_READ_WRITE : MODE_WRITE;
		files[fd].dataSize = 0;
		_posixCleanup = posixCleanup; /* close on _exit() */
		return fd+FD_OFFSET;
	}
	else { // O_RDONLY
		checkDiskChange(NULL);
	
		if (findDirEntry(hpName, fileType, &d) < 0)
			return -1;
		
		ReadData_t* r = malloc(sizeof(ReadData_t));
		
		if (r == NULL)
			return -1;
		
		r->startBlock = d.startBlock;
		r->numBlocks = d.numBlocks;
		r->currentBlock = (uint32_t)(-1);
		r->sizeIsExact = 0;
		r->fileType = d.type;
		strcpy(r->filename, hpName);
		files[fd].dataSize = d.numBlocks * LIF_BLOCK_DATA_SIZE;
		files[fd].mode = MODE_READ;
		files[fd].position = 0;
		files[fd].rw.readData = r;
		return fd+FD_OFFSET;
	}
}

int close(int fd) {
	int e = 0;
	
	HPFILE*f = &files[fd-FD_OFFSET];
	
	if (f->mode == MODE_WRITE || (f->mode == MODE_READ_WRITE && f->rw.writeData->dirty)) {
		uint32_t size = f->dataSize;
		
#ifdef LIFPACK
		uint32_t available;
		uint32_t freeSpace;
		
		if (diskSpace(NULL, &freeSpace, &available) >= 0) {
			if (size > freeSpace * 254) {
				return -1;
			}
			if (size > available * 254) {
				if (lifPack(1) < 0) {
					return -1;
				}
			}
		}
#endif		
		
		WriteData_t* w = (WriteData_t*)f->rw.writeData;
		struct chunk *chunkP = &(w->firstChunk);
		char first = 1;
		int foundType = getFileType(w->filename);
		if (foundType > 0 && foundType != w->fileType)
			deleteByNameAndType(w->filename, foundType);
		int out = openFile(w->filename, w->fileType, WRITE_FILE);
		uint32_t wrote = 0;
		do {
			struct chunk* next = chunkP->next;
			if (0 <= out) 
			{
				int32_t s = chunkP->chunkSize;
				if ((uint32_t)s > size)
					s = size;
				if (s > 0) {
					if (s != writeFile(out, chunkP->data, s)) {
						e = -1;
						closeFile(out);
						out = -1;
					}
					else {
						wrote += s;
					}
				}
				size -= s;
			}
			if (! first) {
				free(chunkP);
			}
			else {
				first = 0;
			}
			chunkP = next;
		} while(chunkP != NULL);
		if (0 <= out) {
			closeFile(out);
		}
	}
	if (f->rw.data != NULL)
		free(f->rw.data);
	files[fd-FD_OFFSET].rw.data = NULL;
	
	return e;
}

static void posixCleanup(void) {
	for (uint16_t fd=0;fd<MAX_FILES;fd++) {
		if (files[fd].rw.data != NULL)
			close(fd+FD_OFFSET);
	}
}

static int readWrite(HPFILE* f, void* p, size_t size, uint8_t writeMode) {
	WriteData_t* w = (WriteData_t*)f->rw.writeData;

	struct chunk* curChunk = w->currentChunk;
	
	if (!writeMode) {
		if (f->position >= f->dataSize)
			return 0;
		if (f->position + size > f->dataSize) 
			size = f->dataSize - f->position;		
	}
	
	if (f->position < curChunk->offset) {
		curChunk = &w->firstChunk;
	}

	while (curChunk->offset + curChunk->chunkSize <= f->position) {
		if (f->dataSize < curChunk->offset + curChunk->chunkSize) {
			f->dataSize = curChunk->offset + curChunk->chunkSize;
		}
		if (curChunk->next == NULL) {
			curChunk->next = malloc(sizeof(struct chunk)+DEFAULT_CHUNK_SIZE);
			if (curChunk->next == NULL) {
				w->currentChunk = curChunk;
				return 0;
			}
			memset(curChunk->next, 0, sizeof(struct chunk)+DEFAULT_CHUNK_SIZE);
			curChunk->next->chunkSize = DEFAULT_CHUNK_SIZE;
			curChunk->next->offset = curChunk->offset + curChunk->chunkSize;
		}			
		curChunk = curChunk->next;
	}
	
	if (f->dataSize < f->position)
		f->dataSize = f->position;
	
	uint32_t wrote = 0;
	
	while (0 < size) {
		uint32_t posInChunk = f->position - curChunk->offset;
		uint32_t toCopy = curChunk->chunkSize - posInChunk;
		if (toCopy > size)
			toCopy = size;
		
		if (writeMode) {
			memcpy(curChunk->data + posInChunk, p, toCopy);
			w->dirty = 1;
		}
		else {
			memcpy(p, curChunk->data + posInChunk, toCopy);
		}
		
		f->position = curChunk->offset + posInChunk + toCopy;
		if (f->position > f->dataSize)
			f->dataSize = f->position;
		size -= toCopy;
		wrote += toCopy;
		p = (char*)p + toCopy;
		if (0 < size) {
			if (curChunk->next == NULL) {
				curChunk->next = malloc(sizeof(struct chunk)+DEFAULT_CHUNK_SIZE);
				if (curChunk->next == NULL) {
					w->currentChunk = curChunk;
					return wrote;
				}
				memset(curChunk->next, 0, sizeof(struct chunk));
				curChunk->next->chunkSize = DEFAULT_CHUNK_SIZE;
				curChunk->next->offset = curChunk->offset + curChunk->chunkSize;
			}
			curChunk = curChunk->next;
		}
	}
	
	w->currentChunk = curChunk;
	return wrote;
}

int read(int fd, void* ptr, size_t size) {
	HPFILE* f = &files[fd-FD_OFFSET];
	
	if (f->mode == MODE_READ_WRITE) {
		return readWrite(f, ptr, size, 0);
	}

	if (f->mode != MODE_READ) 
		return -1;

	if (checkDiskChange(f) < 0)
		return 0;
	
	ReadData_t* r = f->rw.readData;
	
	if (r->numBlocks == 0)
		return 0;
	
	int didRead = 0;
	
	while (size > 0) {
		if (f->position >= f->dataSize)
			return didRead;
		
		if (f->position + size > f->dataSize)
			size = f->dataSize - f->position;

		if (r->currentBlock == (uint32_t)(-1) ||
			 f->position < r->blockOffset ||
			 r->blockOffset + LIF_BLOCK_DATA_SIZE <= f->position) {
			
			r->currentBlock = f->position / LIF_BLOCK_DATA_SIZE;
			r->blockOffset = r->currentBlock * LIF_BLOCK_DATA_SIZE;
			
			if (readBlock(r->startBlock+r->currentBlock, &r->buffer) < 0) {
				return 0;
			}
			if ( r->buffer.size < LIF_BLOCK_DATA_SIZE ) {
				f->dataSize = r->blockOffset + r->buffer.size;
				r->sizeIsExact = 1;
				if (f->position >= f->dataSize)
					return 0;
				if (f->position + size > f->dataSize)
					size = f->dataSize - f->position;
			}
		}
		uint32_t doCopy;
		
		if ( f->position + size > r->blockOffset + LIF_BLOCK_DATA_SIZE ) {
			doCopy = r->blockOffset + LIF_BLOCK_DATA_SIZE - f->position;
		}
		else {
			doCopy = size;
		}
		memcpy(ptr, r->buffer.data + ( f->position - r->blockOffset ), doCopy);
		
		ptr = (char*)ptr + doCopy;
		size -= doCopy;
		f->position += doCopy;
		didRead += doCopy;
	}
	
	return didRead;
}

static int getHPLength(HPFILE* f) {
	if (f->mode != MODE_READ) 
		return -1;
	if (checkDiskChange(f) < 0)
		return -1;
	ReadData_t* r = f->rw.readData;
	
	if (r->numBlocks == 0)
		return 0;
	
	r->currentBlock = r->numBlocks - 1;
	r->blockOffset = r->currentBlock * LIF_BLOCK_DATA_SIZE;

	if (readBlock(r->startBlock + r->currentBlock, &r->buffer) < 0) {
		r->currentBlock = -1;
		return -1;
	}
	
	return r->blockOffset + r->buffer.size;
}


off_t lseek(int fd, off_t offset, int origin) {
	int32_t pos;
	
	HPFILE* f = &files[fd-FD_OFFSET];
	
	switch(origin) {
		case SEEK_SET:
			pos = offset;
			break;
		case SEEK_CUR:
			if (offset == 0) {
				return f->position;
			}
			pos = f->position + offset;
			break;
		case SEEK_END:
			if (f->mode == MODE_READ && ! f->rw.readData->sizeIsExact ) {
				int n = getHPLength(f);
				if (n < 0) 
					return -1;
				f->dataSize = n;
				f->rw.readData->sizeIsExact = 1;
			}
			pos = f->dataSize + offset;
			break;
		default:
			return -1;
	}
	if (pos < 0) {
		f->position = 0;
		return -1;
	}
	else if ((uint32_t)pos > f->dataSize) {
		f->position = f->dataSize;
		return -1;
	}
	f->position = pos;
	return pos;
}



int write(int fd, const void* p, size_t size) {
	HPFILE* f = &files[fd-FD_OFFSET];

	if (f->mode != MODE_WRITE && f->mode != MODE_READ_WRITE)
		return -1;

	if (size == 0)
		return 0;
	
	return readWrite(f, (void*)p, size, 1);
}

int fsync(int fd) {
	(void)fd;
	return -1; // this cannot be supported on our memory-backed files
}

int unlink(const char *pathname) {
	char hpName[MAX_FILENAME_LENGTH+1];
	uint16_t type = getHpName(hpName, pathname);
    return deleteByNameAndType(hpName, type);
}

/* if no type is specified in newPathname, the type is unchanged */
int rename(const char *pathname, const char* newPathname) {
	char hpName[MAX_FILENAME_LENGTH+1];
	uint16_t type = getHpName(hpName, pathname);
	char hpName2[MAX_FILENAME_LENGTH+1];
	int32_t type2 = getHpName(hpName2, newPathname);
	if (type2 == 0)
		type2 = -1;
    return renameFile(hpName, type, hpName2, type2);
}

DIR *opendir(const char *name) {
	(void)name;
	DIR* dirp = malloc(sizeof(DIR));
	if (dirp == NULL)
		return NULL;
	dirp->offset = 0;
	return dirp;
}

struct dir_data {
	DirEntry_t dirEntry;
	struct dirent posixDirEnt;
};

struct dirent* readdir(DIR* dirp) {
	if (dirp->offset == (size_t)-1)
		return NULL;
	
	struct dir_data* d = (struct dir_data*)(dirp->buf);

	do {
		if (getDirEntry(dirp->offset, &(d->dirEntry)) == -1) {
			dirp->offset = (size_t)-1;
			return NULL;
		}
		dirp->offset++;
	} while(d->dirEntry.type == 0);
	strcpy(d->posixDirEnt.d_name, d->dirEntry.name);
	d->posixDirEnt.d_type = 6; // DT_REG
	d->posixDirEnt.d_ino = d->dirEntry.startBlock;

	return &d->posixDirEnt;
}

long telldir(DIR* dirp) {
	return dirp->offset;
}

int closedir(DIR *dirp) {
	free(dirp);
	return 0;
}

int stat(const char *path, struct stat *buf) {
	int fd = open(path, O_RDONLY);
	if (fd < 0)
		return -1;
	int length = lseek(fd, 0, SEEK_END);
	memset(buf, 0, sizeof(struct stat));
	buf->st_mode = 0100000 /* S_IFREG */ | 0644;
	buf->st_size = length;
	close(fd);
	return 0;
}

