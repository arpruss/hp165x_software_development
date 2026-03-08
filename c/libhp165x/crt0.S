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
		move.l  %sp, _original_stack_pointer /* just in case */
		move.l	#_stack_start, %d0
		tst.l	%d0
		beq		1f
        movea.l %d0, %sp

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

		bsr		bigDiskSupport
        jsr     main
		
		.globl  exit
exit:		
		bsr		_final_cleanup_with_atexit_support
		bra		hang

		.globl	_exit
_exit:
		bsr		_final_cleanup
hang:
		bra 	hang
		
		.globl	_final_cleanup_with_atexit_support		
_final_cleanup_with_atexit_support:		
		move.l	__call_atexit_functions,%a0
		cmp.l	#0,%a0
		beq		_final_cleanup
		jsr		(%a0)

		.globl	_final_cleanup
_final_cleanup:
		move.l	_posixCleanup,%a0
		cmp.l	#0,%a0
		beq		1f
		jsr		(%a0)
1:	
		move.w 	#0xF00, 0x201000 /* restore screen writing mode */
		clr.w	0x98077c
		move.w  #0xF,0x9077e 
		move.w	#0x4,0x9800b2 /* black on white */
		move.w	#0,0x9800b0 /* not bold */
#if defined(DEFAULT_SCREEN_HEIGHT) || defined(SCREEN_WIDTH)
		jsr	resetMC6845
#else
		cmp.w   #384, screenHeight
		beq		1f
		jsr		resetMC6845
1:		
#endif		
		.globl _restore_original_int_handlers
_restore_original_int_handlers:
		move.l	#_original_int1_handler,%a0
		move.l	#0x00980000,%a1
		move.w  #(7-1),%d1
1:
		move.l	2(%a0),2(%a1) /* move address */
		move.w	(%a0),(%a1)   /* then change the opcode */
		addq	#6,%a0
		addq	#6,%a1
		dbra	%d1,1b

		rts
		
		.globl bigDiskSupport
bigDiskSupport:		
		tst.b 0x984152+13
		beq 4f
		/* TODO: maybe add a _refreshDir() call */
		move.w #(254*20*2-1),0x9842a6
		move.w #(254*20*2-1),0x9842a8
		clr.b 0x984152+13
	4:	
		rts

		.globl _posixCleanup
_posixCleanup:
		.long   0
		.globl __call_atexit_functions
__call_atexit_functions: /* avoid linking atexit stuff if atexit() is not called */
		.long   0
		
		.globl _original_stack_pointer
_original_stack_pointer:		
		.space  4

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
