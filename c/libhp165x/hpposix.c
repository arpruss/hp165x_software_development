#include <hp165x.h>
#include <sys/types.h>
#include <fcntl.h>
//#include <errno.h> //TODO!

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <wchar.h>
#include <malloc.h>

#include "hpposix.h"

#define CHUNK_SIZE 4096

#define MAX_FILES 6
#define DEFAULT_FILE_TYPE 1
#define FD_OFFSET 3

#ifndef SEEK_SET
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#endif

/* The files are entirely stored in memory. 
   This is a kludge to get around the fact that I don't know how to seek
   within the HP's files, and the HP has a limit of one open file at a time. 
   But, hey, there is 1mb of RAM. */

struct chunk {
	uint32_t offset;
	struct chunk* next;
	char data[CHUNK_SIZE];
};

typedef struct {
	char filename[MAX_FILENAME_LENGTH+1];
	uint16_t fileType;
	struct chunk* currentChunk;
	struct chunk firstChunk;
} WriteData_t;

typedef struct {
	uint32_t position;
	uint32_t dataSize;
	char write;
	union {
		WriteData_t* writeData;
		char* readData;
		void* data;
	} rw;
} HPFILE;

static HPFILE files[MAX_FILES] = {};

uint16_t fromHex(const char* p) {
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

int open(const char* name, int flags, ...) {
	DirEntry_t d;
	
	char hpName[MAX_FILENAME_LENGTH+1];
	uint16_t fileType = getHpName(hpName, name);
	
	int fd = 0;
	for (fd=0;fd<MAX_FILES;fd++) {
		if (files[fd].rw.data == NULL)
			break;
	}
	if (fd >= MAX_FILES) {
        return -1;		
	}
	if (flags & O_RDWR) {
		return -1; // TODO
	}
	else if (flags & O_WRONLY) {
		WriteData_t *w = malloc(sizeof(WriteData_t));
		if (w == NULL) {
			return -1;
		}
		files[fd].rw.writeData = w;
		strcpy(w->filename,hpName);
		w->fileType = fileType == 0 ? DEFAULT_FILE_TYPE : fileType;
		w->firstChunk.next = NULL;
		w->firstChunk.offset = 0;
		w->currentChunk = &(w->firstChunk);
		files[fd].position = 0;
		files[fd].write = 1;
		files[fd].dataSize = 0;
		return fd+FD_OFFSET;
	}
	else { // O_RDONLY
		int16_t i = 0;
		while(1) {
			if ( -1 == getDirEntry(i, &d) ) {
				return -1;
			}
			if ((fileType == 0 || d.type == fileType) && !strcmp(d.name, hpName)) {
				break;
			}				
			i++;
		}		
		uint32_t dataSize = 254*d.numBlocks;
		char* data = malloc(dataSize);
		if (data == NULL)
			return -1;
		int f = openFile(hpName, d.type, READ_FILE);
		if (f < 0) {
			free(data);
			return -1;
		}
		int s = readFile(f, data, -1);
		closeFile(f);
		if (s < 0) {
			free(data);
			return -1;
		}
		files[fd].dataSize = s;
		files[fd].rw.readData = data;
		files[fd].position = 0;
		files[fd].write = 0;
		return fd+FD_OFFSET;
	}
}

int close(int fd) {
	int e = 0;
	
	HPFILE*f = &files[fd-FD_OFFSET];
	
	if (f->write) {
		uint32_t size = f->dataSize;
		WriteData_t* w = (WriteData_t*)f->rw.writeData;
		struct chunk *chunkP = &(w->firstChunk);
		char first = 1;
		int foundType = getFileType(w->filename);
		if (foundType > 0 && foundType != w->fileType)
			deleteByNameAndType(w->filename, foundType);
		int out = openFile(w->filename, w->fileType, WRITE_FILE);
		do {
			struct chunk* next = chunkP->next;
			if (0 <= out) 
			{
				int32_t s = CHUNK_SIZE;
				if ((uint32_t)s > size)
					s = size;
				if (s > 0) {
					if (s != writeFile(out, chunkP->data, s)) {
						e = -1;
						closeFile(out);
						out = -1;
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
		if (0 <= out)
			closeFile(out);
	}
	if (f->rw.data != NULL)
		free(f->rw.data);
	files[fd-FD_OFFSET].rw.data = NULL;
	
	return e;
}

int read(int fd, void* ptr, size_t size) {
	HPFILE* f = &files[fd-FD_OFFSET];
	if (f->write) {
		return -1;
	}
	if (f->position >= f->dataSize)
		return 0;
	if (f->position + size >= f->dataSize)
		size = f->dataSize - f->position;
	memcpy(ptr, f->rw.readData + f->position, size);
	f->position += size;
	return size;
}

off_t lseek(int fd, off_t offset, int origin) {
	int32_t pos;
	
	HPFILE* f = &files[fd-FD_OFFSET];
	
	switch(origin) {
		case SEEK_SET:
			pos = offset;
			break;
		case SEEK_CUR:
			if (offset == 0)
				return f->position;
			pos = f->position + offset;
			break;
		case SEEK_END:
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
	return f->position = pos;
}

int write(int fd, const void* p, size_t size) {
	
	HPFILE* f = &files[fd-FD_OFFSET];

	if (! f->write)
		return -1;

	if (size == 0)
		return 0;
	
	WriteData_t* w = (WriteData_t*)f->rw.writeData;

	struct chunk* curChunk = w->currentChunk;
	
	if (f->position < curChunk->offset) {
		curChunk = &w->firstChunk;
	}

	while (curChunk->offset + CHUNK_SIZE <= f->position) {
		if (f->dataSize < curChunk->offset + CHUNK_SIZE) {
			f->dataSize = curChunk->offset + CHUNK_SIZE;
		}
		if (curChunk->next == NULL) {
			curChunk->next = malloc(sizeof(struct chunk));
			if (curChunk->next == NULL) {
				w->currentChunk = curChunk;
				return 0;
			}
			memset(curChunk->next, 0, sizeof(struct chunk));
			curChunk->next->offset = curChunk->offset + CHUNK_SIZE;
		}			
		curChunk = curChunk->next;
	}
	
	if (f->dataSize < f->position)
		f->dataSize = f->position;
	
	uint32_t wrote = 0;
	
	while (0 < size) {
		uint32_t posInChunk = f->position - curChunk->offset;
		uint32_t toCopy = CHUNK_SIZE - posInChunk;
		if (toCopy > size)
			toCopy = size;
		memcpy(curChunk->data + posInChunk, p, toCopy);
		f->position = curChunk->offset + posInChunk + toCopy;
		if (f->position > f->dataSize)
			f->dataSize = f->position;
		size -= toCopy;
		wrote += toCopy;
		p = (char*)p + toCopy;
		if (0 < size) {
			if (curChunk->next == NULL) {
				curChunk->next = malloc(sizeof(struct chunk));
				if (curChunk->next == NULL) {
					w->currentChunk = curChunk;
					return wrote;
				}
				memset(curChunk->next, 0, sizeof(struct chunk));
				curChunk->next->offset = curChunk->offset + CHUNK_SIZE;
			}
			curChunk = curChunk->next;
		}
	}
	
	w->currentChunk = curChunk;
	return wrote;
}

int fsync(int fd) {
	(void)fd;
	return -1; // this cannot be supported on our memory-backed files
}

int unlink(const char *pathname) {
	char hpName[MAX_FILENAME_LENGTH+1];
	uint16_t type = getHpName(hpName, pathname);
    return deleteByNameAndType(pathname, type);
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

	if (getDirEntry(dirp->offset, &(d->dirEntry)) == -1) {
		dirp->offset = (size_t)-1;
		return NULL;
	}
	dirp->offset++;
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

