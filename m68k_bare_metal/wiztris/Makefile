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
SRCS=wiztris.c blacksquare.S graysquare.S

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

CC=$(PREFIX)-gcc
LD=$(PREFIX)-ld
OBJCOPY=$(PREFIX)-objcopy
OBJDUMP=$(PREFIX)-objdump

CFLAGS=-m$(CPU) --save-temps -Wno-unknown-pragmas -Wno-builtin-declaration-mismatch -Wall -Wextra -static -I../libhp165x -I../include -I. -msoft-float -MMD -MP -O99
LFLAGS=-L../libhp165x -L../libmetal --script=platform.ld -lmetal-$(CPU) -lhp165x -lhp165x

OBJS=$(patsubst %.c,$(BUILDDIR)/%.c.o,$(SRCS))
OBJS:=$(patsubst %.S,$(BUILDDIR)/%.S.o,$(OBJS))
OBJS:=$(patsubst %.s,$(BUILDDIR)/%.s.o,$(OBJS))
DEPS=$(OBJS:.o=.d)


all: bmbinary.s68


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

bmbinary.s68: bmbinary
	$(OBJCOPY) -O binary bmbinary bmbinary.rom
	$(OBJCOPY) -O srec bmbinary bmbinary.s68

dump:
	#$(OBJDUMP) -mm68k:$(CPU) -belf32-m68k -st -j.evt bmbinary
	$(OBJDUMP) -mm68k:$(CPU) -belf32-m68k -dt -j.text bmbinary
	$(OBJDUMP) -mm68k:$(CPU) -belf32-m68k -st -j.rodata -j.data -j.bss -j.heap -j.stack bmbinary

dumps:
	$(OBJDUMP) -mm68k:$(CPU) -belf32-m68k -st -j.evt bmbinary
	$(OBJDUMP) -mm68k:$(CPU) -belf32-m68k -St -j.text bmbinary
	$(OBJDUMP) -mm68k:$(CPU) -belf32-m68k -st -j.rodata -j.data -j.bss -j.heap -j.stack bmbinary

hexdump:
	hexdump -C bmbinary.rom
