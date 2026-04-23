SRCS=hp165x.c puttext.c loadexec.c key.c hpfile.c serial.c scroll.S simple_serial.c \
	inputevents.c input.c mc6845.c screen.c crt0.S atexit.S picostubs.c hpposix.c setjmp.S pack.c \
	pixel.S chooser.c mouseArrow.S drive.c seed32.c

PREFIX=m68k-elf

BUILDDIR=build
BUILDDIR640=build640

CC=$(PREFIX)-gcc
LD=$(PREFIX)-ld
OBJCOPY=$(PREFIX)-objcopy
OBJDUMP=$(PREFIX)-objdump
AR = $(PREFIX)-ar

CFLAGS=-m68000 -ffunction-sections -fdata-sections  --save-temps -Wno-unknown-pragmas -Wno-builtin-declaration-mismatch -Wall -Wextra -static -I../picolibc-68000/usr/local/include -I. -msoft-float -MMD -MP -O99

CFLAGS640=$(CFLAGS) -DDEFAULT_SCREEN_HEIGHT=400 -DSCREEN_WIDTH=640

OBJS=$(patsubst %.c,$(BUILDDIR)/%.c.o,$(SRCS))
OBJS:=$(patsubst %.S,$(BUILDDIR)/%.S.o,$(OBJS))
OBJS:=$(patsubst %.s,$(BUILDDIR)/%.s.o,$(OBJS))
DEPS=$(OBJS:.o=.d)

OBJS640=$(patsubst %.c,$(BUILDDIR640)/%.c.o,$(SRCS))
OBJS640:=$(patsubst %.S,$(BUILDDIR640)/%.S.o,$(OBJS640))
OBJS640:=$(patsubst %.s,$(BUILDDIR640)/%.s.o,$(OBJS640))
DEPS640=$(OBJS640:.o=.d)

all: libhp165x.a libhp165x640.a 


.PHONY: crt clean

release: CFLAGS+= -DNDEBUG
release: all

$(BUILDDIR):
	mkdir -p $@

$(BUILDDIR640):
	mkdir -p $@

$(BUILDDIR)/%.c.o: %.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILDDIR640)/%.c.o: %.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS640) -c -o $@ $<

$(BUILDDIR)/%.S.o: %.S
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILDDIR640)/%.S.o: %.S
	mkdir -p $(@D)
	$(CC) $(CFLAGS640) -c -o $@ $<

$(BUILDDIR640)/%.s.o: %.s
	mkdir -p $(@D)
	$(CC) $(CFLAGS640) -c -o $@ $<

libhp165x.a: $(OBJS) platform.ld 
	cd $(BUILDDIR) && $(AR) rcs ../libhp165x.a *.o

libhp165x640.a: $(OBJS640) platform.ld 
	cd $(BUILDDIR640) && $(AR) rcs ../libhp165x640.a *.o
	
mouseArrow.S: mouseArrow.txt imagetoassembly.py
	python imagetoassembly.py --clipx mouseArrow.txt > mouseArrow.S

-include $(DEPS)

-include $(DEPS640)

clean:
	rm -rf $(BUILDDIR)/*
	rm -rf $(BUILDDIR640)/*
	rm *.a

