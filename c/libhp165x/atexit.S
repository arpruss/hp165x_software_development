		.globl atexit
		
#define AT_EXIT_COUNT 32		

atexit:
		move.l  #_call_atexit_functions,__call_atexit_functions /* activate atexit functions in crt0.S */
		move.l 	#_atexit_functions,%a0
		move.w  #(AT_EXIT_COUNT-1),%d0
1:
		tst.l	(%a0)
		beq		2f
		addq	#4,%a0
		dbra	%d0,1b

		moveq	#-1,%d0
		bra		3f
2:
		move.l  4(%sp),(%a0)           /* add new function */
		moveq	#0,%d0
3:		
		rts
		
_atexit_functions:
		.zero	(AT_EXIT_COUNT)*4

		.globl  _call_atexit_functions
_call_atexit_functions:		
		clr.l   __call_atexit_functions /* deactivate atexit functions in crt0.S */
		move.l	#(_atexit_functions+AT_EXIT_COUNT*4-4),%a0
		move.w	#(AT_EXIT_COUNT-1),%d0
1:
		tst.l	(%a0)
		beq		2f

		move.l	(%a0),%a1
		clr.l	(%a0) /* cannot be called again */

		movem.l	%d0/%a0,-(%sp)
		jsr		(%a1) 
		movem.l	(%sp)+,%d0/%a0

2:		
		subq.l	#4,%a0
		dbra	%d0,1b

		rts
		
        .end
