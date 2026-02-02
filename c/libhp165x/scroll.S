    .text
    .align 2
    .globl scrollUp
	
#define LINE_BYTES (592/4*2)
#define SCREEN 0x600000	
#define SCREEN_END (0x600000 + LINE_BYTES*384)
#define SCREEN_MEMORY_CONTROL 0x00201000
	
    .type scrollUp, @function 
scrollUp:  /* void scrollUp(uint16_t lines, uint16_t fillMode, uint8_t bitplanes) */
	movem.l %d2-%d3/%a2, -(%sp)
	move.l (12+4)(%sp),%d0     /* lines */
	ext.l  %d0
	move.l (12+8)(%sp),%d1     /* fillMode */
	move.l (12+12)(%sp),%d2    /* bitplanes */
	mulu.w #(LINE_BYTES),%d0  /* bytes to move */
	move.l #0x600000,%a0

0:	
	move.l %a0,%a1
	add.l  %d0,%a1 
	move.l %a1,%a2
	add.l  %d0,%a2
	cmp.l  #(SCREEN_END), %a2
	bgt	   1f		
/* move (a1)-(a2) to (a0)-(a1) */
	btst #0,%d2
	beq notBit0
	/* copy 1s */
	move.w #(~(0x001)),SCREEN_MEMORY_CONTROL
	bsr copyLineNeg
	move.w #(~(0x101)),SCREEN_MEMORY_CONTROL
	bsr copyLine
notBit0:
	btst #1,%d2
	beq notBit1 
	/* copy 1s */
	move.w #(~(0x202)),SCREEN_MEMORY_CONTROL
	bsr copyLine
	move.w #(~(0x002)),SCREEN_MEMORY_CONTROL
	bsr copyLineNeg
notBit1:
	btst #2,%d2
	beq notBit2
	/* copy 1s */
	move.w #(~(0x404)),SCREEN_MEMORY_CONTROL
	bsr copyLine
	move.w #(~(0x004)),SCREEN_MEMORY_CONTROL
	bsr copyLineNeg
notBit2:
	btst #3,%d2
	beq notBit3
	/* copy 1s */
	move.w #(~(0x808)),SCREEN_MEMORY_CONTROL
	bsr copyLine
	move.w #(~(0x008)),SCREEN_MEMORY_CONTROL
	bsr copyLineNeg
notBit3:
	move.l %a1,%a0
	bra 0b
1:
	move.w %d1,SCREEN_MEMORY_CONTROL
	move.l #(SCREEN_END),%a1
	move.l #0x000F000F, %d0
	move.l %d0,%d1
2:	
	movem.l %d0-%d1,-(%a1)
	cmp.l %a0,%a1
	bgt 2b

	movem.l (%sp)+, %d2-%d3/%a2
	rts

copyLine: /* copy interval [a1,a2) to a0 using d3 and d4 as scrap, assuming length multiple of 8 */
	move.l %a0,-(%sp)
	move.l %a1,-(%sp)
1:
	move.l (%a1)+,%d3
	move.l %d3,(%a0)+
	move.l (%a1)+,%d3
	move.l %d3,(%a0)+
	cmp.l	%a2,%a1
	blt		1b

	move.l (%sp)+,%a1
	move.l (%sp)+,%a0
	rts
	
copyLineNeg: /* copy interval [a1,a2) to a0 using d3 and d4 as scrap, assuming length multiple of 8, and negating */
	move.l %a0,-(%sp)
	move.l %a1,-(%sp)
1:
	move.l (%a1)+,%d3
	not.l  %d3
	move.l %d3,(%a0)+
	move.l (%a1)+,%d3
	not.l  %d3
	move.l %d3,(%a0)+
	cmp.l  %a2,%a1
	blt		1b

	move.l (%sp)+,%a1
	move.l (%sp)+,%a0
	rts
	