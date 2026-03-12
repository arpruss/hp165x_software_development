include ../../py/mkenv.mk
include $(TOP)/py/py.mk
include $(TOP)/extmod/extmod.mk

# Set CFLAGS and libraries.
CFLAGS += -I. -I$(BUILD) -I$(TOP)
LIBS += -lm
PYTHON=c:/python3/python

CPU=68000

# Dont modify below this line (unless you know what youre doing).
BUILDDIR=build

CC=m68k-elf-gcc
LD=m68k-elf-ld
OBJCOPY=m68k-elf-objcopy
OBJDUMP=m68k-elf-objdump

CFLAGS += -std=c11 -falign-functions=4 -falign-labels=4 -falign-loops=4 -falign-jumps=4 -DMICROPY_GCREGS_SETJMP -m$(CPU) --save-temps -Wno-unknown-pragmas -Wno-builtin-declaration-mismatch -Wall -Wextra -static -I../../../picolibc-68000/usr/local/include -I../../../libhp165x -I. -msoft-float -MMD -MP -O99
LFLAGS=-gc-sections -L../../../libhp165x -L../../../picolibc-68000/usr/local/lib --script=platform.ld -lhp165x640 -lc -lm -lhp165x640 -LC:/68k/bin/../lib/gcc/m68k-elf/13.1.0/m68000/ -lgcc

SRC_C = \
    main.c \
    mphalport.c \
    shared/readline/readline.c \
    shared/runtime/gchelper_generic.c \
    shared/runtime/pyexec.c \
    shared/runtime/stdout_helpers.c \
    extmod/vfs.c \
	extmod/modvfs.c \
    extmod/vfs_posix.c \
    extmod/vfs_reader.c \
    extmod/vfs_posix_file.c \
	extmod/modos.c \
	extmod/modtime.c

# Define source files containung qstrs.
SRC_QSTR += shared/readline/readline.c shared/runtime/pyexec.c

# Define the required object files.
OBJ = $(PY_CORE_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o))

include $(TOP)/py/mkrules.mk

all: bmbinary.s68	

bmbinary: $(OBJ)
	$(LD) -o $@ $(OBJ) $(LFLAGS)

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

bmbinary.s68: bmbinary
	$(OBJCOPY) -O binary bmbinary bmbinary.rom
	$(OBJCOPY) -O srec bmbinary bmbinary.s68
	
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
	