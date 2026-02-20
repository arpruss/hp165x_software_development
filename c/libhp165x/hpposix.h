#ifndef _HPPOSIX_H
#define _HPPOSIX_H

int open(const char* name, int flags, ...);
int close(int fd);
int read(int fd, void* ptr, size_t size);
off_t lseek(int fd, off_t offset, int origin);
int write(int fd, const void* p, size_t size);
int unlink(const char *pathname);

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