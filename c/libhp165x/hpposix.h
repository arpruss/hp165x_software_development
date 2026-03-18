#ifndef _HPPOSIX_H
#define _HPPOSIX_H

#include <stddef.h>

#define DEFAULT_BUFFERED_READ_MAXIMUM 150000

typedef __uint64_t ino_t;

struct dirent {
    ino_t     d_ino; /* Inode number */
    uint8_t   d_type;
    char      d_name[256]; /* Null-terminated filename */
};

typedef struct {
    int           fd;
    size_t        offset;
    size_t        count;
    struct dirent dirent;
    union {
        char       buf[512];
        __uint64_t align;
    };
} DIR;

struct stat;

/* if set, then files subsequently opened in O_RDONLY mode are NOT read
   entirely into memory; they are opened and reclosed as needed to take
   care of the ROM's one-file-at-a-time limit */
void hpPosixSetBufferedReadMaximum(uint32_t bufferedMax); 
int open(const char* name, int flags, ...);
int close(int fd);
int read(int fd, void* ptr, size_t size);
off_t lseek(int fd, off_t offset, int origin);
int write(int fd, const void* p, size_t size);
int fsync(int fd);
int unlink(const char *pathname);
int closedir(DIR *dirp);
long telldir(DIR* dirp);
struct dirent* readdir(DIR* dirp);
DIR *opendir(const char *name);
int unlink(const char *pathname);
int stat(const char *path, struct stat *buf);

#ifndef O_RDONLY
#define O_RDONLY 0
#endif
#ifndef O_WRONLY
#define O_WRONLY 1
#endif
//#ifndef O_RDWR // TODO
//#define O_RDWR   2
//#endif
#ifndef O_CREAT
#define O_CREAT 0x0040
#endif

#endif