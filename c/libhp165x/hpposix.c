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

#define CHUNK_SIZE 4096

#define MAX_FILES 6
#define DEFAULT_FILE_TYPE 1
#define FD_OFFSET 3

#ifndef SEEK_SET
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#endif

#define MODE_BUFFERED_READ    0
#define MODE_UNBUFFERED_READ  1
#define MODE_WRITE            2

/* The files are entirely stored in memory. 
   This is a kludge to get around the fact that I don't know how to seek
   within the HP's files, and the HP has a limit of one open file at a time. 
   But, hey, there is 1mb of RAM. */
   
extern void (*_posixCleanup)(void);
static void posixCleanup(void);
static uint32_t bufferedReadMaximum = DEFAULT_BUFFERED_READ_MAXIMUM;

struct filesize {
	uint16_t id; // 'sz'
	uint32_t size;
};

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
	char filename[MAX_FILENAME_LENGTH+1];
	uint16_t fileType;
	int32_t  fd;
	uint32_t fdPos;
	char	 sizeIsExact;
} UnbufferedReadData_t;

typedef struct {
	uint16_t mode;
	uint32_t position;
	uint32_t dataSize;
	union {
		WriteData_t* writeData;
		UnbufferedReadData_t* unbufferedReadData;
		char* readData;
		void* data;
	} rw;
} HPFILE;

static HPFILE files[MAX_FILES] = { {0} };

void hpPosixSetBufferedReadMaximum(uint32_t m) {
	bufferedReadMaximum = m;
}

static void closeUnbuffered(void) {
	for (uint16_t i = 0 ; i < MAX_FILES ; i++) {
		if (files[i].rw.data != NULL && files[i].mode == MODE_UNBUFFERED_READ && files[i].rw.unbufferedReadData->fd >= 0) {
			closeFile(files[i].rw.unbufferedReadData->fd);
			files[i].rw.unbufferedReadData->fd = -1;			
		}
	}
}

static int ensureOpen(HPFILE* f) {
	if (f->mode != MODE_UNBUFFERED_READ)
		return 0;
	UnbufferedReadData_t* u = f->rw.unbufferedReadData;
	if (u->fd >= 0)
		return 0;
	closeUnbuffered();
	u->fd = openFile(u->filename, u->fileType, READ_FILE);
	if (u->fd < 0) {
		return -1;
	}
	u->fdPos = 0;
	return 0;
}

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
	
	closeUnbuffered();
	
	if (flags & O_WRONLY) {
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
		files[fd].mode = MODE_WRITE;
		files[fd].dataSize = 0;
		_posixCleanup = posixCleanup; /* close on _exit() */
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
		
		uint16_t unbuffered = dataSize > bufferedReadMaximum;
		
		void* data = NULL;
		
		if (!unbuffered) {
			data = malloc(dataSize);
			if (data == NULL)
				unbuffered = 1;
		}
		
		if (data == NULL) {
			data = malloc(sizeof(UnbufferedReadData_t));
			
			if (data == NULL)
				return -1;
		}
		
		int f = openFile(hpName, d.type, READ_FILE);
		if (f < 0) {
			free(data);
			return -1;
		}

		if (unbuffered) {
			struct filesize fs;
			UnbufferedReadData_t* u = data;

			u->sizeIsExact = 0;

			if (getFileMisc(hpName, d.type, &fs) >= 0 && 
					fs.id == ID_SIZE && fs.size <= dataSize && dataSize < fs.size+254) {
						
				dataSize = fs.size;
				u->sizeIsExact = 1;
			}
			strcpy(u->filename, hpName);
			u->fileType = d.type;
			u->fd = f;
			u->fdPos = 0;
			files[fd].dataSize = dataSize;
			files[fd].rw.unbufferedReadData = u;
			files[fd].mode = MODE_UNBUFFERED_READ;
		}
		else {		
			int s = readFile(f, data, -1);
			closeFile(f);
			if (s < 0) {
				free(data);
				return -1;
			}
			files[fd].dataSize = s;
			files[fd].rw.readData = data;
			files[fd].mode = MODE_BUFFERED_READ;
		}
		files[fd].position = 0;
		return fd+FD_OFFSET;
	}
}

int close(int fd) {
	int e = 0;
	
	HPFILE*f = &files[fd-FD_OFFSET];
	
	if (f->mode == MODE_WRITE) {
		closeUnbuffered();
		
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
				int32_t s = CHUNK_SIZE;
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
			if (w->fileType < 0xC001 || w->fileType > 0xC400) {
				struct filesize fs = { ID_SIZE, wrote };
				setFileMisc(w->filename, w->fileType, &fs);
			}
		}
	}
	else if (f->mode == MODE_UNBUFFERED_READ) {
		if (f->rw.unbufferedReadData->fd >= 0) 
			closeFile(f->rw.unbufferedReadData->fd);
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

static int skipHPFile(int hpFD, uint32_t size) {
	while(size > 0) {
		int32_t toSkip = size <= 65536 ? size : 65536;
		if (toSkip != readFile(hpFD, NULL, toSkip))
			return -1;
		size -= toSkip;
	}
	return 0;
}

int read(int fd, void* ptr, size_t size) {
	HPFILE* f = &files[fd-FD_OFFSET];
	if (f->mode == MODE_WRITE) {
		return -1;
	}
	if (f->position >= f->dataSize)
		return 0;
	
	if (f->mode == MODE_UNBUFFERED_READ) {
		if (ensureOpen(f) < 0) 
			return -1;
		UnbufferedReadData_t* u = f->rw.unbufferedReadData;
		if (f->position < u->fdPos) {
			closeFile(u->fd);
			u->fd = -1;
			if (ensureOpen(f) < 0)
				return -1;
		}
		if (u->fdPos < f->position &&
			  skipHPFile(u->fd, f->position - u->fdPos) < 0) {
			closeFile(u->fd);
			u->fd = -1;
			return -1;
		}				
		int n = readFile(u->fd, ptr, size);
		if (n < 0)
			return -1;
		f->position += n;
		u->fdPos = f->position;
		return n;
	}
	else {	
		if (f->position + size >= f->dataSize)
			size = f->dataSize - f->position;
		memcpy(ptr, f->rw.readData + f->position, size);
		f->position += size;
		return size;
	}
}

static int getHPLength(int hpFD) {
	uint32_t count = 0;
	
	int32_t n;
	while( (n=readFile(hpFD, NULL, 65536)) == 65536) 
		count += n;
	if (n < 0)
		return count;
	else
		return count + n;
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
			if (f->mode == MODE_UNBUFFERED_READ && ! f->rw.unbufferedReadData->sizeIsExact ) {
				if (ensureOpen(f)<0)
					return -1;
				UnbufferedReadData_t* u = f->rw.unbufferedReadData;
				int n = getHPLength(u->fd);
				closeFile(u->fd);
				u->fd = -1;
				if (n < 0) 
					return -1;
				f->dataSize = u->fdPos + n;
				u->sizeIsExact = 1;
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
	return f->position = pos;
}

int write(int fd, const void* p, size_t size) {
	HPFILE* f = &files[fd-FD_OFFSET];

	if (f->mode != MODE_WRITE)
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
	char hpName[MAX_FILENAME_LENGTH+1];
	uint16_t fileType = getHpName(hpName, path);
	DirEntry_t d;
    
	int16_t i = 0;
	while(1) {
		if ( -1 == getDirEntry(i, &d) ) {
			return -1;
		}
		if ((fileType == 0 || d.type == fileType) && !strcmp(d.name, hpName)) {
			memset(buf, 0, sizeof(struct stat));
			buf->st_mode = 0100000 /* S_IFREG */ | 0644;
			struct filesize* fs = (struct filesize*)&d.misc;
			uint32_t approxSize = 254*d.numBlocks;
			if (fs->id == ID_SIZE && fs->size <= approxSize && approxSize < fs->size+254)
				buf->st_size = fs->size;
			else
				buf->st_size = approxSize; // TODO: consider reading to find the actual size
			return 0;
		}				
		i++;
	}		
	return -1;
}

