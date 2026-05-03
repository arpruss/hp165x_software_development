# GNU MAKE Makefile for PDCurses library for VT
#
# Usage: [g]make [DEBUG=Y] [WIDE=Y] [UTF8=Y] [LIBNAME=(name)]
#                [DLLNAME=(name)] [target]
#
# where target can be any of:
# [all|demos|libpdcurses.a|testcurs]...

PREFIX=m68k-elf
CC=$(PREFIX)-gcc
LD=$(PREFIX)-ld
OBJCOPY=$(PREFIX)-objcopy
OBJDUMP=$(PREFIX)-objdump
AR = $(PREFIX)-ar

CPU=68000
GCC_LIB_DIR=C:/68k/lib/gcc/m68k-elf/13.1.0/m$(CPU)/ 
LIBC_INCLUDE = ../../picolibc-$(CPU)/usr/local/include
LIBC_LIB_DIR = ../../picolibc-$(CPU)/usr/local/lib
LIBC = c

CFLAGS=-I../../picolibc-68000/usr/local/include -I../../libhp165x -m68000 -ffunction-sections -fdata-sections  -Wno-unknown-pragmas -Wno-builtin-declaration-mismatch -Wall -Wextra -static -I. -msoft-float -MMD -MP -O99 -D_POSIX_MONOTONIC_CLOCK

O = o

ifndef PDCURSES_SRCDIR
	PDCURSES_SRCDIR = ..
endif

osdir		= $(PDCURSES_SRCDIR)/hp165x
common		= $(PDCURSES_SRCDIR)/common

include $(common)/libobjs.mif

RM		= rm -f

CFLAGS  += -Wall -Wextra 
ifeq ($(DEBUG),Y)
	CFLAGS  += -g -DPDCDEBUG
else
	CFLAGS  += -O2
endif

E =
RM = rm

ifeq ($(UTF8),Y)
	CFLAGS	+= -DPDC_FORCE_UTF8
else
	ifeq ($(WIDE),Y)
		CFLAGS	+= -DPDC_WIDE
	endif
endif

ifdef CHTYPE_32
	CFLAGS += -DCHTYPE_32
endif

ifdef CHTYPE_16
	CFLAGS += -DCHTYPE_16
endif

ifdef _w64
	PREFIX  = x86_64-w64-mingw32-
	IS_WIN = Y
endif

ifdef _w32
	PREFIX  = i686-w64-mingw32-
	IS_WIN = Y
endif

ifdef _a64
	PREFIX  = aarch64-w64-mingw32-
	IS_WIN = Y
endif

ifeq ($(OS),Windows_NT)
	IS_WIN = Y
	RM = cmd /c del
endif

E = .bin

LIBNAME=libpdcurses
DLLNAME=pdcurses

LIBFLAGS = rv
LIBCURSES	= $(LIBNAME).a
CC	= $(PREFIX)-gcc
LIBEXE = $(PREFIX)-ar
STRIP	= $(PREFIX)-strip

ifeq ($(shell uname -s),FreeBSD)
	CC = cc
endif

TARGET_TRIPLE := $(subst -, ,$(shell $(CC) -dumpmachine))
TARGET_ARCH   := $(word 1,$(TARGET_TRIPLE))
TARGET_OS     := $(word 3,$(TARGET_TRIPLE))

#LIBFLAGS = -Wl,--out-implib,$(LIBNAME).a -shared -o
LIBSADDED = -lhp165x640

BUILD		= $(CC) $(CFLAGS) -I$(PDCURSES_SRCDIR)

LINK		= $(CC)
LDFLAGS		= -nodefaultlibs -nostartfiles $(LIBCURSES) -Wl,-gc-sections -L../../libhp165x -L$(LIBC_LIB_DIR) -Wl,--script=platform.ld -lhp165x640 -l$(LIBC) -lhp165x640 -L$(GCC_LIB_DIR) -l$(LIBC) -lgcc -l$(LIBC)
RANLIB		= $(PREFIX)-ranlib

.PHONY: all libs clean demos tests

all:	libs

libs:	$(LIBCURSES)

clean:
	-$(RM) *.o trace $(LIBCURSES) $(DEMOS) $(TESTS) *.bin *.elf *.map *.d *.s68

demos:	libs $(DEMOS)
ifneq ($(DEBUG),Y)
	$(STRIP) $(DEMOS)
endif

tests:	libs $(TESTS)
ifneq ($(DEBUG),Y)
	$(STRIP) $(TESTS)
endif

$(LIBCURSES) : $(LIBOBJS) $(PDCOBJS)
	$(LIBEXE) $(LIBFLAGS) $@ $? $(LIBSADDED)
ifeq ($(LIBCURSES),$(LIBNAME).a)
	-$(RANLIB) $@
endif

$(LIBOBJS) $(PDCOBJS) : $(PDCURSES_HEADERS)
$(DEMOS) : $(PDCURSES_CURSES_H) $(LIBCURSES)
$(TESTS) : $(PDCURSES_CURSES_H) $(LIBCURSES)
tui.o tuidemo.o : $(PDCURSES_CURSES_H)
terminfo.o : $(TERM_HEADER)
panel.o ptest: $(PANEL_HEADER)

$(LIBOBJS) : %.o: $(srcdir)/%.c
	$(BUILD) -c $<

$(PDCOBJS) : %.o: $(osdir)/%.c
	$(BUILD) -c $<

$(DEMOS_EXCEPT_TUIDEMO): %$(E): $(demodir)/%.c
	$(BUILD) $(DEMOFLAGS) -o$@.elf $< $(LDFLAGS) -Wl,-Map=$@.map $(EXELIBS) 
	$(OBJCOPY) -O srec $@.elf $@.s68
	python ../../../buildbinary.py $@.s68 $@
    
$(TESTS): %$(E): $(testdir)/%.c
	$(BUILD) $(DEMOFLAGS) -o$@ $< $(LDFLAGS) $(EXELIBS)

tuidemo$(E) : tuidemo.o tui.o
	$(LINK) tui.o tuidemo.o -o $@ $(LDFLAGS) $(EXELIBS)

tui.o: $(demodir)/tui.c $(demodir)/tui.h
	$(BUILD) -c $(DEMOFLAGS) $(demodir)/tui.c

tuidemo.o: $(demodir)/tuidemo.c
	$(BUILD) -c $(DEMOFLAGS) $(demodir)/tuidemo.c

configure :
	$(CC) $(CFLAGS) -o config_curses$(E) $(common)/config_curses.c
ifdef PREFIX
	wine config_curses$(E) -v -d.. $(CFLAGS)
else
	./config_curses$(E) -v -d.. $(CFLAGS)
endif
	rm config_curses$(E)

include $(demodir)/nctests.mif

install:
ifneq ($(OS),Windows_NT)
	cp lib$(DLLNAME).so /usr/local/lib
	ldconfig /usr/local/lib
endif

uninstall:
ifneq ($(OS),Windows_NT)
	$(RM) /usr/local/lib/lib$(DLLNAME).so
	ldconfig /usr/local/lib
endif
