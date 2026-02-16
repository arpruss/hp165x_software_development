# List all of the source files that will be compiled into your binary.
#
# For example, if you have the following source files
#
#   main.c
#   user.c
#   driver.s
#
# then your SRCS list would be
#
#   SRCS=main.c user.c driver.s
#
# The list may span several lines of text by appending a backslash to each line,
# for example:
#
#   SRCS=main.c user.c \
#        driver.s
SRCS=hp165x.c puttext.c loadexec.c key.c hpfile.c serial.c scroll.S simple_serial.c \
	keyboard.c input.c mc6845.c screen.c crt0.S 

# Specify the CPU type that you are targeting your build towards.
#
# Supported architectures can usually be found with the --target-help argument
# passed to gcc, but a quick summary is:
#
# 68000, 68010, 68020, 68030, 68040, 68060, cpu32 (includes 68332 and 68360),
# 68302
CPU=68000

# Uncomment either of the following depending on how you have installed gcc on
# your system. m68k-linux-gnu for Linux installations, m68k-eabi-elf if gcc was
# built from scratch e.g. on a Mac by running the build script.
# PREFIX=m68k-linux-gnu
PREFIX=m68k-elf

# Dont modify below this line (unless you know what youre doing).
BUILDDIR=build

BUILDDIR640=build640

CC=$(PREFIX)-gcc
LD=$(PREFIX)-ld
OBJCOPY=$(PREFIX)-objcopy
OBJDUMP=$(PREFIX)-objdump
AR = $(PREFIX)-ar

CFLAGS=-m$(CPU) -ffunction-sections -fdata-sections  --save-temps -Wno-unknown-pragmas -Wno-builtin-declaration-mismatch -Wall -Wextra -static -I../hputils -I../../m68k_bare_metal/include -I. -msoft-float -MMD -MP -O99

CFLAGS640=$(CFLAGS) -DDEFAULT_SCREEN_HEIGHT=400 -DSCREEN_WIDTH=640

LFLAGS=--script=platform.ld -L../../m68k_bare_metal/libmetal -lmetal-$(CPU)

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

libhp165x.a: $(OBJS) platform.ld 
	cd $(BUILDDIR) && $(AR) rcs ../libhp165x.a *.o

libhp165x640.a: $(OBJS640) platform.ld 
	cd $(BUILDDIR640) && $(AR) rcs ../libhp165x640.a *.o

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

-include $(DEPS)

-include $(DEPS640)

clean:
	rm -rf $(BUILDDIR)/*
	rm -rf $(BUILDDIR640)/*

