    .globl hpsetjmp
	.globl hplongjmp
    .text
    .align 2
    .type hpsetjmp, @function 
hpsetjmp:
	move.l 4(%sp),%a0 /* address of jmp_buf */
	movem.l %d2-%d7/%a2-%a7,(%a0)
	move.l (%sp),48(%a0) /* save return address */
	clr.l  %d0
	rts
	
    .type hplongjmp, @function 
hplongjmp:
	move.l  4(%sp),%a0 /* address of jmp_buf */
	move.l  8(%sp),%d0 /* return value */
	bne	    1f
	moveq.l #1,%d0
1:
	movem.l (%a0)+,%d2-%d7/%a2-%a7
	move.l  (%a0),(%sp)
	rts
	