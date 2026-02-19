#include <stddef.h>
#include <stdio.h>
#include <hp165x.h>

#if 0
extern char _heap_start;       
extern char _heap_end; 

void * _sbrk(ptrdiff_t incr) {
    static char *heap_end = &_heap_start;
    char *prev_heap_end;

    if (heap_end + incr > &_heap_end) {
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
	(void) file;		
	putTextN(&c, 1);	
	return c;
}

static int
hpgetc(FILE *file)
{
	(void)file;
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
FILE *const stdout = &__stdio;
FILE *const stderr = &__stdio;

