        .title "crt0.S for m68k bare metal - standalone"

        /* A bunch of variables supplied by the linker */
        .extern _ram_base
        .extern _bss_start
        .extern _bss_end
        .extern _rodata_end
        .extern _data_start
        .extern _data_end
        .extern main

        .section .text
        .align 2

        .type _start, @function
        .globl _start
_start:
/*        movea.l #_stack_end, %sp  */      

        /* Initialise (clear) the BSS area */
1:      movea.l #_bss_start, %a0        /* Starting address */
        movea.l #_bss_end, %a1          /* End address */

2:      cmpa.l  %a1, %a0                /* Check if start < end */
        bge     7f

        clr.w   %a0@+                   /* Clear word and increment start */

        bra     2b

7:      jmp     main

        /* In a bare metal environment, main() should not return as there is
         * nowhere to resume execution from. Behaviour is undefined if this
         * happens! */

        .end
