        .title "crt0.S for m68k bare metal - standalone"

        /* A bunch of variables supplied by the linker */
        .extern _ram_base
        .extern _bss_start
        .extern _bss_end
        .extern _rodata_end
        .extern _data_start
        .extern _data_end
        .extern main

        .section .text.init
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

	/* save original interrupt vectors */
7:		move.l	#0x00980000,%a0
		move.l	#_original_int1_handler,%a1
		move.w  #(7*3-1),%d0		
3:		
		move.w  (%a0)+,%d1
		move.w	%d1,(%a1)+
		dbra	%d0,3b

#if defined(DEFAULT_SCREEN_HEIGHT) || defined(SCREEN_WIDTH)
		jsr		setDefaultScreenSize
#endif		

        jsr     main
		bra		_exit
;;		jsr		0x0000ece2				/* reload OS */
		
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

_final_cleanup:
	/* TODO: close files */
	
		move.w 	#0xF00, 0x201000 /* restore screen writing mode */
#if defined(DEFAULT_SCREEN_HEIGHT) || defined(SCREEN_WIDTH)
		jsr	resetMC6845
#else
		cmp.w   #384, screenHeight
		beq		1f
		jsr		resetMC6845
1:		
#endif
	/*	bra _restore_original_int_handlers */
		
_restore_original_int_handlers:
		move.l	#_original_int1_handler,%a0
		move.l	#0x00980000,%a1
		move.w  #(7-1),%d1
1:
		move.l	2(%a0),%d0 /* move address, and then move opcode */
		move.l  %d0,2(%a1)
		move.w	(%a0),%d0
		move.w	%d0,(%a1)
		add.q	#6,%a0
		add.q	#6,%a1
		dbra	%d1,1b

		rts
		
_atexit_functions:
		.zero	32*4

		.globl _restore_original_int_handlers
		.globl _final_cleanup
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

		bsr		_final_cleanup

hang:
		bra 	hang
4:
		movem.l	%d0/%a0,-(%sp)

		move.l	(%a0),%a0
		jsr		(%a0) 

		movem.l	(%sp)+,%d0/%a0
		bra		2b

		.globl _original_int1_handler
_original_int1_handler:
		.space 6
		.globl _original_int2_handler
_original_int2_handler:
		.space 6
		.globl _original_int3_handler
_original_int3_handler:
		.space 6
		.globl _original_int4_handler
_original_int4_handler:
		.space 6
		.globl _original_int5_handler
_original_int5_handler:
		.space 6
		.globl _original_int6_handler
_original_int6_handler:
		.space 6
		.globl _original_int7_handler
_original_int7_handler:
		.space 6
		

		
        .end
