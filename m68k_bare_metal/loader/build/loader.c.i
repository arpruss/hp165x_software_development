# 0 "loader.c"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "loader.c"
# 1 "../include/stdio.h" 1



# 1 "../include/printf.h" 1
# 42 "../include/printf.h"
# 1 "../include/stdarg.h" 1
# 13 "../include/stdarg.h"
typedef __builtin_va_list va_list;
typedef __builtin_va_list __isoc_va_list;
# 43 "../include/printf.h" 2
# 1 "../include/stddef.h" 1



# 1 "../include/_types/_ptrdiff_t.h" 1



typedef int ptrdiff_t;
# 5 "../include/stddef.h" 2
# 1 "../include/_types/_size_t.h" 1



typedef unsigned int size_t;

typedef int ssize_t;
# 6 "../include/stddef.h" 2
# 36 "../include/stddef.h"
typedef long double max_align_t;
# 44 "../include/printf.h" 2
# 74 "../include/printf.h"
void _putchar(char character);
# 85 "../include/printf.h"
int printf_(const char* format, ...) __attribute__((format(__printf__, (1), (2))));
# 96 "../include/printf.h"
int sprintf_(char* buffer, const char* format, ...) __attribute__((format(__printf__, (2), (3))));
int vsprintf_(char* buffer, const char* format, va_list va) __attribute__((format(__printf__, (2), (0))));
# 110 "../include/printf.h"
int snprintf_(char* buffer, size_t count, const char* format, ...) __attribute__((format(__printf__, (3), (4))));
int vsnprintf_(char* buffer, size_t count, const char* format, va_list va) __attribute__((format(__printf__, (3), (0))));
# 120 "../include/printf.h"
int vprintf_(const char* format, va_list va) __attribute__((format(__printf__, (1), (0))));
# 132 "../include/printf.h"
int fctprintf(void (*out)(char character, void* arg), void* arg, const char* format, ...) __attribute__((format(__printf__, (3), (4))));
int vfctprintf(void (*out)(char character, void* arg), void* arg, const char* format, va_list va) __attribute__((format(__printf__, (3), (0))));
# 5 "../include/stdio.h" 2


# 1 "../include/wctype.h" 1







# 1 "../include/_types/_wchar_t.h" 1




typedef unsigned wchar_t;
# 9 "../include/wctype.h" 2
# 1 "../include/_types/_wint_t.h" 1



typedef unsigned wint_t;
# 10 "../include/wctype.h" 2

typedef const int* wctrans_t;
typedef unsigned long wctype_t;
# 30 "../include/wctype.h"
int iswalnum(wint_t);
int iswalpha(wint_t);
int iswblank(wint_t);
int iswcntrl(wint_t);
int iswctype(wint_t, wctype_t);
int iswdigit(wint_t);
int iswgraph(wint_t);
int iswlower(wint_t);
int iswprint(wint_t);
int iswpunct(wint_t);
int iswspace(wint_t);
int iswupper(wint_t);
int iswxdigit(wint_t);

wint_t towctrans(wint_t, wctrans_t);
wint_t towlower(wint_t);
wint_t towupper(wint_t);
wctrans_t wctrans(const char*);
wctype_t wctype(const char*);
# 8 "../include/stdio.h" 2
# 25 "../include/stdio.h"
typedef union _G_fpos64_t
{
 char __opaque[16];
 long long __lldata;
 double __align;
} fpos_t;




struct __sbuf
{
 unsigned char* _base;
 int _size;
};






struct __sFILE_fake
{
 unsigned char* _p;
 int _r;
 int _w;
 short _flags;
 short _file;
 struct __sbuf _bf;
 int _lbfsize;

 struct _reent* _data;
};


typedef struct __sFILE_fake FILE;






int putchar(int c);
int puts(const char*);
# 79 "../include/stdio.h"
int fseek(FILE*, long, int);
long ftell(FILE*);
void rewind(FILE*);

int fgetpos(FILE* __restrict, fpos_t* __restrict);
int fsetpos(FILE*, const fpos_t*);

size_t fread(void* __restrict, size_t, size_t, FILE* __restrict);
size_t fwrite(const void* __restrict, size_t, size_t, FILE* __restrict);

char* fgets(char* __restrict, int, FILE* __restrict);




int fputc(int, FILE*);
int putc(int, FILE*);

wchar_t* fgetws(wchar_t* __restrict, int, FILE* __restrict);
int fputws(const wchar_t* __restrict, FILE* __restrict);

int fgetc(FILE*);
int getc(FILE*);
int getchar(void);
int ungetc(int, FILE*);

wint_t fgetwc(FILE*);
wint_t getwc(FILE*);
wint_t getwchar(void);
wint_t ungetwc(wint_t, FILE*);

wint_t fputwc(wchar_t, FILE*);
wint_t putwc(wchar_t, FILE*);
wint_t putwchar(wchar_t);

char* tmpnam(char*);
FILE* tmpfile(void);

int fwide(FILE*, int);

int fputs(const char* __restrict, FILE* __restrict);

FILE* fopen(const char* __restrict, const char* __restrict);
FILE* freopen(const char* __restrict, const char* __restrict, FILE* __restrict);
int fclose(FILE*);

int feof(FILE*);
int ferror(FILE*);
int fflush(FILE*);
void clearerr(FILE*);

int remove(const char*);
int rename(const char*, const char*);

int setvbuf(FILE* __restrict, char* __restrict, int, size_t);
void setbuf(FILE* __restrict, char* __restrict);

int scanf(const char* __restrict, ...);
int fscanf(FILE* __restrict, const char* __restrict, ...);
int sscanf(const char* __restrict, const char* __restrict, ...);
int vscanf(const char* __restrict, __isoc_va_list);
int vfscanf(FILE* __restrict, const char* __restrict, __isoc_va_list);
int vsscanf(const char* __restrict, const char* __restrict, __isoc_va_list);
int wscanf(const wchar_t* __restrict, ...);
int fwscanf(FILE* __restrict, const wchar_t* __restrict, ...);
int swscanf(const wchar_t* __restrict, const wchar_t* __restrict, ...);
int vwscanf(const wchar_t* __restrict, __isoc_va_list);
int vfwscanf(FILE* __restrict, const wchar_t* __restrict, __isoc_va_list);
int vswscanf(const wchar_t* __restrict, const wchar_t* __restrict, __isoc_va_list);
# 156 "../include/stdio.h"
void perror(const char*);

int wprintf(const wchar_t* __restrict, ...);
int fprintf(FILE* __restrict, const char* __restrict, ...);
int vfprintf(FILE* __restrict, const char* __restrict, __isoc_va_list);
int vsprintf_(char* __restrict, const char* __restrict, __isoc_va_list);
int fwprintf(FILE* __restrict, const wchar_t* __restrict, ...);
int swprintf(wchar_t* __restrict, size_t, const wchar_t* __restrict, ...);
int vwprintf(const wchar_t* __restrict, __isoc_va_list);
int vfwprintf(FILE* __restrict, const wchar_t* __restrict, __isoc_va_list);
int vswprintf(wchar_t* __restrict, size_t, const wchar_t* __restrict, __isoc_va_list);
# 2 "loader.c" 2
# 1 "../include/string.h" 1
# 9 "../include/string.h"
# 1 "../include/stdint.h" 1
# 9 "../include/stdint.h"
# 1 "../include/limits.h" 1




# 1 "../include/_limits.h" 1
# 6 "../include/limits.h" 2
# 10 "../include/stdint.h" 2
# 20 "../include/stdint.h"
# 1 "../include/_types/_int16_t.h" 1



typedef signed short int16_t;
# 21 "../include/stdint.h" 2
# 1 "../include/_types/_int32_t.h" 1



typedef signed int int32_t;
# 22 "../include/stdint.h" 2
# 1 "../include/_types/_int64_t.h" 1



typedef signed long long int64_t;
# 23 "../include/stdint.h" 2
# 1 "../include/_types/_int8_t.h" 1



typedef signed char int8_t;
# 24 "../include/stdint.h" 2
# 1 "../include/_types/_uint16_t.h" 1



typedef unsigned short uint16_t;
# 25 "../include/stdint.h" 2
# 1 "../include/_types/_uint32_t.h" 1



typedef unsigned int uint32_t;
# 26 "../include/stdint.h" 2
# 1 "../include/_types/_uint64_t.h" 1



typedef unsigned long long uint64_t;
# 27 "../include/stdint.h" 2
# 1 "../include/_types/_uint8_t.h" 1



typedef unsigned char uint8_t;
# 28 "../include/stdint.h" 2



typedef int8_t int_least8_t;


typedef int16_t int_least16_t;


typedef int32_t int_least32_t;


typedef int64_t int_least64_t;


typedef uint8_t uint_least8_t;


typedef uint16_t uint_least16_t;


typedef uint32_t uint_least32_t;


typedef uint64_t uint_least64_t;



typedef int8_t int_fast8_t;


typedef int32_t int_fast16_t;


typedef int32_t int_fast32_t;


typedef int64_t int_fast64_t;


typedef uint8_t uint_fast8_t;


typedef uint32_t uint_fast16_t;


typedef uint32_t uint_fast32_t;


typedef uint64_t uint_fast64_t;


# 1 "../include/_types/_intptr_t.h" 1



typedef int intptr_t;
# 81 "../include/stdint.h" 2
# 1 "../include/_types/_uintptr_t.h" 1



typedef unsigned uintptr_t;
# 82 "../include/stdint.h" 2


# 1 "../include/_types/_intmax_t.h" 1



typedef long long intmax_t;
# 85 "../include/stdint.h" 2
# 1 "../include/_types/_uintmax_t.h" 1



typedef unsigned long long uintmax_t;
# 86 "../include/stdint.h" 2
# 10 "../include/string.h" 2
# 29 "../include/string.h"
int memcmp(const void* s1, const void* s2, size_t n);
# 45 "../include/string.h"
void* memset(void* dest, int c, size_t n);
# 62 "../include/string.h"
void* memcpy(void* __restrict dest, const void* __restrict src, size_t n);
# 78 "../include/string.h"
void* memmove(void* dest, const void* src, size_t n);
# 94 "../include/string.h"
void* memchr(const void* s, int c, size_t n);
# 111 "../include/string.h"
void* memmem(const void* l, size_t l_len, const void* s, size_t s_len);
# 127 "../include/string.h"
size_t strlen(const char* str);
# 144 "../include/string.h"
size_t strnlen(const char* str, size_t maxlen);
# 164 "../include/string.h"
char* strcpy(char* __restrict dst, const char* __restrict src);
# 187 "../include/string.h"
char* strncpy(char* __restrict dst, const char* __restrict src, size_t maxlen);
# 204 "../include/string.h"
char* strstr(const char* string, const char* substring);
# 222 "../include/string.h"
char* strnstr(const char* s, const char* find, size_t slen);
# 237 "../include/string.h"
int strcmp(const char* s1, const char* s2);
# 255 "../include/string.h"
int strncmp(const char* s1, const char* s2, size_t n);
# 267 "../include/string.h"
char* strdup(const char* str);
# 280 "../include/string.h"
char* strndup(const char* str, size_t n);
# 296 "../include/string.h"
char* strchr(const char* s, int c);
# 312 "../include/string.h"
char* strrchr(const char* s, int c);
# 332 "../include/string.h"
char* strcat(char* __restrict dst, const char* __restrict src);
# 355 "../include/string.h"
char* strncat(char* __restrict dst, const char* __restrict src, size_t maxlen);
# 375 "../include/string.h"
char* strtok(char* s, const char* delim);
char* strtok_r(char* s, const char* delim, char** last);


size_t strxfrm(char* __restrict, const char* __restrict, size_t);
size_t strcspn(const char*, const char*);
size_t strspn(const char*, const char*);
char* strpbrk(const char*, const char*);
int strcoll(const char*, const char*);
char* strerror(int);
int strerror_r(int, char*, size_t);
# 3 "loader.c" 2
# 1 "../include/stdlib.h" 1
# 15 "../include/stdlib.h"
typedef struct
{
 int quot;
 int rem;
} div_t;


typedef struct
{
 long quot;
 long rem;
} ldiv_t;


typedef struct
{
 long long quot;
 long long rem;
} lldiv_t;
# 54 "../include/stdlib.h"
void abort(void) __attribute__((noreturn));
int atexit(void (*)(void));
void exit(int) __attribute__((noreturn));






void _Exit(int) __attribute__((noreturn));
int at_quick_exit(void (*)(void));
void quick_exit(int) __attribute__((noreturn));
int cxa_atexit(void (*)(void*), void*, void*);



char* getenv(const char*);




int system(const char*);





int mblen(const char*, size_t);
int mbtowc(wchar_t* __restrict, const char* __restrict, size_t);
int wctomb(char*, wchar_t);
size_t mbstowcs(wchar_t* __restrict, const char* __restrict, size_t);
size_t wcstombs(char* __restrict, const wchar_t* __restrict, size_t);
# 106 "../include/stdlib.h"
int atoi(const char* str);
# 124 "../include/stdlib.h"
long atol(const char* str);
# 142 "../include/stdlib.h"
long long atoll(const char* str);
# 166 "../include/stdlib.h"
double atof(const char* str);
# 209 "../include/stdlib.h"
float strtof(const char* __restrict str, char** __restrict str_end);
# 250 "../include/stdlib.h"
double strtod(const char* __restrict str, char** __restrict str_end);
# 295 "../include/stdlib.h"
long strtol(const char* __restrict str, char** __restrict str_end, int base);
# 338 "../include/stdlib.h"
unsigned long strtoul(const char* __restrict str, char** __restrict str_end, int base);
# 383 "../include/stdlib.h"
long long strtoll(const char* __restrict str, char** __restrict str_end, int base);
# 427 "../include/stdlib.h"
unsigned long long strtoull(const char* __restrict str, char** __restrict str_end, int base);


long double strtold(const char* __restrict, char** __restrict);
# 443 "../include/stdlib.h"
int abs(int n);
# 454 "../include/stdlib.h"
long labs(long n);
# 465 "../include/stdlib.h"
long long llabs(long long n);
# 484 "../include/stdlib.h"
div_t div(int x, int y);
# 508 "../include/stdlib.h"
ldiv_t ldiv(long x, long y);
# 532 "../include/stdlib.h"
lldiv_t lldiv(long long x, long long y);



int rand_r(unsigned int* ctx);
# 553 "../include/stdlib.h"
int rand(void);
# 566 "../include/stdlib.h"
void srand(unsigned seed);
# 594 "../include/stdlib.h"
int heapsort(void* vbase, size_t nmemb, size_t size, int (*compar)(const void*, const void*));
# 627 "../include/stdlib.h"
int heapsort_r(void* vbase, size_t nmemb, size_t size, void* thunk,
      int (*compar)(void*, const void*, const void*));
# 651 "../include/stdlib.h"
void* bsearch(const void* key, const void* ptr, size_t count, size_t size,
     int (*comp)(const void*, const void*));
# 676 "../include/stdlib.h"
void qsort_r(void* a, size_t n, size_t es, void* thunk,
    int (*cmp)(void*, const void*, const void*));
# 697 "../include/stdlib.h"
void qsort(void* a, size_t n, size_t es, int (*compar)(const void*, const void*));
# 714 "../include/stdlib.h"
void* malloc(size_t size);
# 734 "../include/stdlib.h"
void free(void* ptr);
# 747 "../include/stdlib.h"
void* calloc(size_t num, size_t size);
# 772 "../include/stdlib.h"
void* realloc(void* ptr, size_t size);
# 792 "../include/stdlib.h"
void* reallocf(void* ptr, size_t size);
# 4 "loader.c" 2
# 1 "../include/ctype.h" 1
# 44 "../include/ctype.h"
int isalnum(int ch);
# 58 "../include/ctype.h"
int isalpha(int ch);
# 72 "../include/ctype.h"
int isascii(int ch);
# 88 "../include/ctype.h"
int isblank(int ch);
# 103 "../include/ctype.h"
int iscntrl(int ch);
# 116 "../include/ctype.h"
int isdigit(int ch);
# 134 "../include/ctype.h"
int isgraph(int ch);
# 150 "../include/ctype.h"
int islower(int ch);
# 169 "../include/ctype.h"
int isprint(int ch);
# 184 "../include/ctype.h"
int ispunct(int ch);
# 204 "../include/ctype.h"
int isspace(int ch);
# 220 "../include/ctype.h"
int isupper(int ch);
# 233 "../include/ctype.h"
int isxdigit(int ch);
# 250 "../include/ctype.h"
int tolower(int ch);
# 267 "../include/ctype.h"
int toupper(int ch);
# 279 "../include/ctype.h"
int toascii(int ch);
# 5 "loader.c" 2

# 1 "../libhp165x/hp165x.h" 1
# 58 "../libhp165x/hp165x.h"
uint16_t getKey(void);
void drawPixel(uint16_t x, uint16_t y);
void fillScreen(void);
void drawBlack(void);
void drawWhite(void);
void reload(void);
void _exit(int status);
void exit(int status);





void setTextMode(uint32_t mode);
void drawText(const char* p);
void setCoordinates(int32_t x, int32_t y);
int _openFile(const char* filename, uint32_t fileType, uint32_t mode);
int openFile(const char* filename, uint32_t fileType, uint32_t mode);
int writeFile(int32_t fd, const void* data, int32_t size);
int readFile(int32_t fd, void* data, int32_t size);
void closeFile(int32_t fd);

typedef void (*Reload_t)(void);


typedef struct {
 char name[10];
 uint16_t type;
 uint32_t startBlock;
 uint32_t numBlocks;
 uint8_t dateAndTime[6];
 uint8_t misc[6];
} DirEntry_t;

int findDirEntry(const char*filename, uint32_t type, DirEntry_t* dirEntry,uint32_t startIndex, uint32_t nameLength);
int getDirEntry(int index, DirEntry_t* dirEntry);



uint16_t getKeyWait(void);
void drawVerticalLine(uint16_t x, uint16_t y1, uint16_t y2);
void drawHorizontalLine(uint16_t x1, uint16_t y, uint16_t x2);
void waitSeconds(uint16_t n);
uint32_t getVBLCounter(void);
void patchVBL(void);
void unpatchVBL(void);
void setVBLCounter(uint32_t value);
void initialScreen();
char parseKey(uint16_t key);
int loadAndRun(const char* filename);

# 1 "../libhp165x/puttext.h" 1






void setTextBlackOnWhite(char value);
uint16_t getTextX(void);
uint16_t getTextY(void);
void setTextXY(uint16_t x, uint16_t y);
void setTextX(uint16_t x);
void setTextY(uint16_t x);
void putText(char* s);
void putchar_(int c);
# 110 "../libhp165x/hp165x.h" 2
# 7 "loader.c" 2



char names[16][10 +1];
int numNames = 0;

enum { SELECT=0, WAIT } mode;

void getFiles(void) {
 DirEntry_t entry;

 int in = 0;
 numNames = 0;

 setTextXY(0,5);
 int i;
 while(numNames < 16 && -1 != (i=getDirEntry(in, &entry))) {
  if ((uint16_t)i == (uint16_t)0xC001) {
   printf_(entry.name);
   names[numNames][10] = 0;
   memcpy(names[numNames], entry.name, 10);
   numNames++;
  }
  in++;
 }
 printf_("num names %d\n", numNames);
}

void scan(void) {
 *((volatile uint16_t*)0x980704) = 0;
 while(1) {
  if ( ((1<<3) & *((volatile uint16_t*)0x20F000) ) ) {
   setTextXY(0,0);
   putText("No disc in drive...");
   while ( ((1<<3) & *((volatile uint16_t*)0x20F000) ) ) {
    uint16_t k = *((volatile uint16_t*)0x980704);
    *((volatile uint16_t*)0x980704) = 0;
    if (k) {
     if (k == 0x0104)
      reload();
    }
   }
   setTextXY(0,0);
   putText("                   ");
  }
  setTextXY(0,0);
  putText("Scanning...");
  getFiles();
  if (numNames == 0) {
   setTextXY(0,1);
   putText("No files found...");
  }
  else {
   setTextXY(0,1);
   putText("                 ");
   return;
  }
 } while(numNames == 0);
}

void menu(void) {
 drawBlack();
 fillScreen();
 setTextXY(0,0);
 putText("Choose program to execute:\n\n");
 for (int i=0; i<numNames; i++) {
  printf_(" [%X] %s\n", i, names[i]);
 }
 while (1) {
  uint16_t k = *((volatile uint16_t*)0x980704);
  *((volatile uint16_t*)0x980704) = 0;
  if (k == 0x0104)
   return;
  if ((1<<3) & *((volatile uint16_t*)0x20F000) )
   return;
  int c = parseKey(k);
  if ('0' <= c && c <= '9') {
   c -= '0';
   if (c < numNames)
    loadAndRun(names[c]);
   continue;
  }
  else if ('A' <= c && c <= 'F') {
   c += 10 - 'A';
   if (c < numNames)
    loadAndRun(names[c]);
   continue;
  }
 }
}


int main(void) {
 setTextBlackOnWhite(0);

 while(1) {
  drawBlack();
  fillScreen();

  scan();
  menu();
 }
}
