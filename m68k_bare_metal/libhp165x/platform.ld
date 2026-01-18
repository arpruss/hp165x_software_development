/*
 * Modify the ROM and RAM base and sz (size) variables below to
 * suit your systems memory layout.
 *
 *   base indicates at what address a memory begins
 *   sz indicates the size of that memory
 *
 * Values can be specified in decimal, hexadecimal, or as units such
 * as 1K or 1M.
 *
 * Space for the stack may also be reserved here. The heap will then
 * consume all remaining RAM between the end of the .bss section and
 * the lowest address reserved for the stack.
 *
 * Since there are no controls in place to ensure the stack does not
 * grow into the heap, the user is responsible for reserving sufficient
 * stack space to prevent it overwriting the upper areas of the heap.
 */
_ram_base = 0x980000;
_stack_sz = 16K;
_stack_sz += 15;
_stack_sz &= ~0xF;
_ram_sz = 1024K - _stack_sz; 
_stack_start = _ram_base + _ram_sz + _stack_sz - 4;

/*
 * Dont modify below this line (unless you know what youre doing),
 * except to add user interrupt vectors.
 */

STARTUP(crt0.o)
OUTPUT_ARCH(m68k)

__ram_end = (_ram_base + _ram_sz);

__text_org = _ram_base + 0x4500;
__text_sz = _ram_sz - 0x4500;


/* Make sure stack size is a multiple of 16, and at least the amount the user
 * thought they were getting. */

MEMORY {
    text         (rwx!w) : ORIGIN = __text_org, LENGTH = __text_sz
}

SECTIONS {
    .text : {
        _text_start = .;
        *(.text .text.*)
        . = ALIGN(0x10);
        _text_end = .;
    } > text

    .rodata : AT(_text_end) {
        _rodata_start = .;
        *(.rodata .rodata.*)
        . = ALIGN(0x10);
        _rodata_end = .;
    } > text

    .data : AT(_rodata_end) {
        _data_start = .;
        *(.data .data.*)
        . = ALIGN(0x10);
        _data_end = .;
    } > text

    .bss : {
        _bss_start = .;
        *(.bss .bss.*)
        *(COMMON)
        . = ALIGN(0x10);
        _bss_end = .;
    } > text

    .heap : {
        _heap_start = .;
        . += (__text_sz /*- _stack_sz*/ - SIZEOF(.bss) - SIZEOF(.data) - SIZEOF(.text) - SIZEOF(.rodata));
        _heap_end = .;
    } > text
/*
    .stack : {
        _stack_start = .;
        . += _stack_sz;
        _stack_end = .;
    } > text */
}
