SRCS=wiztris.c blacksquare.S

CPU=68000

LIBRARY=picolibc 
#LIBRARY=libmetal
PREFIX=m68k-elf
HPLIB=hp165x
PRINTF_VERSION= #--defsym=vfscanf=__d_vfscanf --defsym=vfprintf=__d_vfprintf 
GCC_LIB_DIR=C:/68k/bin/../lib/gcc/m68k-elf/13.1.0/m$(CPU)/ 

LIBRARY=picolibc
ifeq ($(LIBRARY),picolibc)
LIBC_INCLUDE = ../picolibc-$(CPU)/usr/local/include
LIBC_LIB_DIR = ../picolibc-$(CPU)/usr/local/lib
LIBC = c
LIBC_OPTIONS =
else
LIBC_INCLUDE = ../../m68k_bare_metal/include
LIBC_LIB_DIR = ../../m68k_bare_metal/libmetal
LIBC = metal-$(CPU)	
LIBC_OPTIONS = -Dsetjmp=hpsetjmp -Dlongjmp=hplongjmp -Djmp_buf=hpjmp_buf
endif

BUILDDIR=build
CC=$(PREFIX)-gcc
LD=$(PREFIX)-ld
OBJCOPY=$(PREFIX)-objcopy
OBJDUMP=$(PREFIX)-objdump

CFLAGS=-m$(CPU) $(LIBC_OPTIONS) --save-temps -Wno-unknown-pragmas -Wno-builtin-declaration-mismatch -Wall -Wextra -static -I../libhp165x -I$(LIBC_INCLUDE) -I. -msoft-float -MMD -MP -O99
LFLAGS=-gc-sections $(PRINTF_VERSION) -L../libhp165x -L$(LIBC_LIB_DIR) --script=platform.ld -l$(HPLIB) -l$(LIBC) -l$(HPLIB) -L$(GCC_LIB_DIR) -lgcc -Map=build/map

OBJS=$(patsubst %.c,$(BUILDDIR)/%.c.o,$(SRCS))
OBJS:=$(patsubst %.S,$(BUILDDIR)/%.S.o,$(OBJS))
OBJS:=$(patsubst %.s,$(BUILDDIR)/%.s.o,$(OBJS))
DEPS=$(OBJS:.o=.d)


all: wiztris.bin

.PHONY: bmbinary release all clean rom dump dumps hexdump

graysquare.S: imagetoassembly.py graysquare.txt
	python imagetoassembly.py graysquare.txt > graysquare.S

blacksquare.S: imagetoassembly.py blacksquare.txt
	python imagetoassembly.py blacksquare.txt > blacksquare.S

bmbinary: $(OBJS)
	$(LD) -o $@ $(OBJS) $(LFLAGS)

release: CFLAGS+= -DNDEBUG
release: all

$(BUILDDIR):
	mkdir -p $@

$(BUILDDIR)/%.c.o: %.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILDDIR)/%.S.o: %.S
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILDDIR)/%.s.o: %.s
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

-include $(DEPS)

clean:
	rm -rf $(BUILDDIR)/*
	rm -f bmbinary*

wiztris.bin: bmbinary
	$(OBJCOPY) -O binary bmbinary bmbinary.rom
	$(OBJCOPY) -O srec bmbinary bmbinary.s68
	python ../../buildbinary.py bmbinary.s68 wiztris.bin

dump:
	#$(OBJDUMP) -mm68k:$(CPU) -belf32-m68k -st -j.evt bmbinary
	$(OBJDUMP) -mm68k:$(CPU) -belf32-m68k -dt -j.text bmbinary
	$(OBJDUMP) -mm68k:$(CPU) -belf32-m68k -st -j.rodata -j.data -j.bss -j.heap -j.stack bmbinary

dumps:
	#$(OBJDUMP) -mm68k:$(CPU) -belf32-m68k -st -j.evt bmbinary
	$(OBJDUMP) -mm68k:$(CPU) -belf32-m68k -St -j.text bmbinary
	$(OBJDUMP) -mm68k:$(CPU) -belf32-m68k -st -j.rodata -j.data -j.bss -j.heap -j.stack bmbinary

hexdump:
	hexdump -C bmbinary.rom
