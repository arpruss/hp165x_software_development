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
        movea.l #_stack_start, %sp

        /* Initialise (clear) the BSS area */
1:      movea.l #_bss_start, %a0        /* Starting address */
        movea.l #_bss_end, %a1          /* End address */

2:      cmpa.l  %a1, %a0                /* Check if start < end */
        bge     7f

        clr.w   %a0@+                   /* Clear word and increment start */

        bra     2b
7:      jsr     main
		bra		_exit
;		jsr		0x0000ece2				/* reload OS */
		
		.globl atexit
atexit:
		move.l	%a0,-(%sp)
		move.l 	#_atexit_functions,%a0
		move.w  #31,%d0
1:
		tst.l	(%a0)
		beq		2f
		add		#4,%a0
		dbra	%d0,1b

		moveq	#-1,%d0
		bra		3f
2:
		move.l  8(%sp),(%a0)           /* add new function */
		moveq	#0,%d0
3:		
		move.l	(%sp)+,%a0
		rts
		
_atexit_functions:
		.zero	32*4

		.globl  _exit
		.globl	exit
exit:
_exit:
		move.l	#(_atexit_functions+32*4-4),%a0
		move.w	#31,%d0
1:
		tst.l	(%a0)
		bne		4f
2:		
		sub.l	#4,%a0
		dbra	%d0,1b

		move.l	0,%d0

3:
		move.l  %d0,-(%sp) /* save d0 */

		move.l  %d0,-(%sp)
		jsr		0x0000eb7a /* close file */
		add.l	#4,%sp
		
		move.l	(%sp)+,%d0 /* restore d0 */
		
		add.l	#1,%d0
		cmp.l	#16,%d0  /* very conservative: my 1653B's 1987 ROM only allows one file open! */
		bne		3b

hang:
		bra 	hang
4:
		movem.l	%d0/%a0,-(%sp)

		move.l	(%a0),%a0
		jsr		(%a0) 

		movem.l	(%sp)+,%d0/%a0
		bra		2b
		
        .end
