_program_base = 0xA72008;
_program_end =  0xA77FFE;
_stack_start = 0; /* there should be about 30K here */
_program_sz = _program_end - _program_base;

EXTERN(_start)
ENTRY(_start)
OUTPUT_ARCH(m68k)

__text_org = _program_base;
__text_sz = _program_sz;
__heap_start = _bss_end;
_heap_start = __heap_start;
__heap_end = ORIGIN(text)+LENGTH(text);
_heap_end = __heap_end;

MEMORY {
    text         (rwx!w) : ORIGIN = __text_org, LENGTH = __text_sz
}

SECTIONS {
    .text : {
        _text_start = .;
		*(.text.init)
        *(.text .text.*)
        . = ALIGN(4);
        _text_end = .;
    } > text

    .rodata : {
        _rodata_start = .;
        *(.rodata .rodata.*)
        . = ALIGN(4);
        _rodata_end = .;
    } > text

	.eh_frame : {
        _eh_frame_start = .;
        KEEP(*(.eh_frame))
        . = ALIGN(4);
        _eh_frame_end = .;
    } > text

    .data : {
        _data_start = .;
        *(.data .data.*)
        . = ALIGN(4);
        _data_end = .;
    } > text

    .bss (NOLOAD) : {
        _bss_start = .;
        *(.bss .bss.*)
        *(COMMON)
        . = ALIGN(4);
        _bss_end = .;
    } > text

    .heap (NOLOAD) : {
        _heap_start = .;
        __heap_start = .;
        . = ALIGN(4);
		. = ORIGIN(text)+LENGTH(text);
		_heap_end = .;
		__heap_end = .;
    } > text

}
