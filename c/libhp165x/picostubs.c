#include <stdio.h>
#include <hp165x.h>

#if 0
extern char _heap_start;       
extern char _heap_end; 

void * _sbrk(ptrdiff_t incr) {
    static char *heap_end = &_heap_start;
    char *prev_heap_end;

    if (heap_end + incr > &_heap_end) {
        errno = ENOMEM; // Out of memory
        return (void *)-1;
    }

    prev_heap_end = heap_end;
    heap_end += incr;

    return (void *)prev_heap_end;
}
#endif

static int
hpputc(char c, FILE *file)
{
return c;
	(void) file;		
	putTextN(&c, 1);	
	return c;
}

static int
hpgetc(FILE *file)
{
	return ' ';
	static char initialized = 0;
	if (!initialized) {
		initKeyboard(1);
		initialized = 1;
	}
	while (!kbhit());
	return getch();
}

static FILE __stdio = FDEV_SETUP_STREAM(hpputc, hpgetc, NULL, _FDEV_SETUP_RW);
FILE *const stdin = &__stdio;
__strong_reference(stdin, stdout);
__strong_reference(stdin, stderr);

#if 0
void _exit(int status) {
    (void)status;
    reload(); 
}
#endif