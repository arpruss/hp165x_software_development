    .globl scrollDown
	.globl scrollUp

/*#define NO_SCROLL_OPTIMIZE */

#ifndef SCREEN_WIDTH
# define SCREEN_WIDTH 592
#endif
	
#define LINE_BYTES (SCREEN_WIDTH/4*2)
#define LINE_WORDS (SCREEN_WIDTH/4)
#define LINE_DWORDS (SCREEN_WIDTH/8)
#define SCREEN 0x600000	
#define SCREEN_MEMORY_CONTROL 0x00201000
	
    .text
    .align 2
    .type scrollDown, @function 
	.section	.text.scrollUp,"ax",@progbits
scrollDown:  /* void scrollDown(uint16_t lines, uint16_t leftX, uint16_t topY, uint16_t rightX, uint16_t bottomY, 
					uint16_t fillMode, uint8_t bitplanes) */
		   /* leftX and rightX must be divisible by 4 and their difference by 8 */
	movem.l %d2-%d7/%a2, -(%sp)
	move.l (28+4)(%sp),%d0     /* lines */
	ext.l  %d0
	move.l (28+8)(%sp),%d4     /* leftX */
	ext.l  %d4
	move.l (28+12)(%sp),%d5     /* topY */
	ext.l  %d5
	move.l (28+16)(%sp),%d6     /* rightX */
	ext.l  %d6
	move.l (28+20)(%sp),%d7     /* bottomY */
	ext.l  %d7
	subq.l #1,%d7
	move.l (28+28)(%sp),%d2    /* bitplanes */
	mulu.w #(LINE_BYTES),%d0  /* distance to move */
	mulu.w #(LINE_BYTES),%d5  /* offset of top line of window */
	mulu.w #(LINE_BYTES),%d7  /* offset of bottom line of window */
	lsr.w  #1,%d4			  /* byte offset of leftX */
	add.w  %d4,%d7			  /* %d5: offset of start of scroll area (on bottom) */
	lsr.w  #1,%d6			  /* byte offset for start */
	sub.w  %d4,%d6			  /* byte count per line */
	lsr.w  #2,%d6			  /* dword count per line */
	sub.w  #1,%d6			  /* %d6: rep count per line */
	move.l #(SCREEN),%a0      
	add.l  %d7,%a0            /* destination pointer */
	move.l %a0,%a1
	sub.l  %d0,%a1			  /* source pointer */
	move.l #(SCREEN),%a2
	add.l  %d5,%a2			  /* top */
	
0:
	cmp.l  %a2,%a1 /* ensure source does not go too high */
	ble	   1f 
	
/* move d6+1 words from a0 to a1 */
	btst #0,%d2
	beq notBit0 
	/* copy 1s */
	move.w #(~(0x001)),SCREEN_MEMORY_CONTROL
	bsr copyLineNeg
	move.w #(~(0x101)),SCREEN_MEMORY_CONTROL
	bsr copyLine
notBit0Down:
	btst #1,%d2
	beq notBit1Down
	/* copy 1s */
	move.w #(~(0x202)),SCREEN_MEMORY_CONTROL
	bsr copyLine
  	move.w #(~(0x002)),SCREEN_MEMORY_CONTROL
	bsr copyLineNeg
notBit1Down:
	btst #2,%d2
	beq notBit2Down
	/* copy 1s */
	move.w #(~(0x404)),SCREEN_MEMORY_CONTROL
	bsr copyLine
	move.w #(~(0x004)),SCREEN_MEMORY_CONTROL
	bsr copyLineNeg
notBit2Down:
	btst #3,%d2
	beq notBit3Down
	/* copy 1s */
	move.w #(~(0x808)),SCREEN_MEMORY_CONTROL
	bsr copyLine
	move.w #(~(0x008)),SCREEN_MEMORY_CONTROL
	bsr copyLineNeg
notBit3Down:
	sub.l  #(LINE_BYTES),%a0
	sub.l  #(LINE_BYTES),%a1
	bra 0b
1:

/* clear top */
	move.l (28+24)(%sp),%d1     /* fillMode */
	move.w %d1,SCREEN_MEMORY_CONTROL

	move.w #(LINE_BYTES),%d7
 	move.w %d6,%d4
	add.w  #1,%d6 
	add.w  %d6,%d6 
	add.w  %d6,%d6 /* number of bytes to clear per line */
	add.w  %d6,%d7 /* offset between end of one line and beginning of previous */
	ext.l  %d7	
	
	move.l #0x000F000F, %d0
2:
	move.w %d4,%d6
3:	
	move.l %d0,(%a0)+
	dbra   %d4,3b
	sub.w  %d7,%a0
	cmp.l  %a2,%a0
	ble 4f
	move.w %d6,%d4
	bra 2b
4:
	movem.l (%sp)+, %d2-%d7/%a2
	rts

	.section	.text.scrollUp,"ax",@progbits
#ifndef NO_SCROLL_OPTIMIZE	
wholeLinesPlane0:
/* %a1 = source, %a0 = dest, %a2 = bottom: keep copying until %a1 >= %a2 */
/* %d0 = bitplanes */
/* use d4-d7 as scrap */
/* assumes: whole lines, even number of lines to scroll, even height of region */

	move.l #(LINE_BYTES*2),%d1
1:

	btst    #0,%d2
	beq		4f	 

	move.w  #(LINE_DWORDS/2-1),%d0
	move.w #(~(0x101)),SCREEN_MEMORY_CONTROL	
2:
	movem.l (%a1)+,%d4-%d7 
	move.l  %d4,(%a0)+
	move.l  %d5,(%a0)+
	move.l  %d6,(%a0)+
	move.l  %d7,(%a0)+
	dbra	%d0,2b

	sub.l   %d1,%a0
	sub.l   %d1,%a1

	move.w  #(LINE_DWORDS/2-1),%d0
	move.w #(~(0x001)),SCREEN_MEMORY_CONTROL

3:
	movem.l (%a1)+,%d4-%d7 
	not.l	%d4
	not.l	%d5
	not.l	%d6
	not.l	%d7
	move.l  %d4,(%a0)+
	move.l  %d5,(%a0)+
	move.l  %d6,(%a0)+
	move.l  %d7,(%a0)+
	dbra	%d0,3b

	sub.l   %d1,%a0
	sub.l   %d1,%a1

4:
	btst    #1,%d2
	beq		4f	

	move.w  #(LINE_DWORDS/2-1),%d0
	move.w #(~(0x202)),SCREEN_MEMORY_CONTROL	
2:
	movem.l (%a1)+,%d4-%d7 
	move.l  %d4,(%a0)+
	move.l  %d5,(%a0)+
	move.l  %d6,(%a0)+
	move.l  %d7,(%a0)+
	dbra	%d0,2b

	sub.l   %d1,%a0
	sub.l   %d1,%a1

	move.w  #(LINE_DWORDS/2-1),%d0
	move.w #(~(0x002)),SCREEN_MEMORY_CONTROL

3:
	movem.l (%a1)+,%d4-%d7 
	not.l	%d4
	not.l	%d5
	not.l	%d6
	not.l	%d7
	move.l  %d4,(%a0)+
	move.l  %d5,(%a0)+
	move.l  %d6,(%a0)+
	move.l  %d7,(%a0)+
	dbra	%d0,3b

	sub.l   %d1,%a0
	sub.l   %d1,%a1
4:

	btst    #2,%d2
	beq		4f	

	move.w  #(LINE_DWORDS/2-1),%d0
	move.w #(~(0x404)),SCREEN_MEMORY_CONTROL	
2:
	movem.l (%a1)+,%d4-%d7 
	move.l  %d4,(%a0)+
	move.l  %d5,(%a0)+
	move.l  %d6,(%a0)+
	move.l  %d7,(%a0)+
	dbra	%d0,2b

	sub.l   %d1,%a0
	sub.l   %d1,%a1

	move.w  #(LINE_DWORDS/2-1),%d0
	move.w #(~(0x004)),SCREEN_MEMORY_CONTROL

3:
	movem.l (%a1)+,%d4-%d7 
	not.l	%d4
	not.l	%d5
	not.l	%d6
	not.l	%d7
	move.l  %d4,(%a0)+
	move.l  %d5,(%a0)+
	move.l  %d6,(%a0)+
	move.l  %d7,(%a0)+
	dbra	%d0,3b

	sub.l   %d1,%a0
	sub.l   %d1,%a1
4:

	btst    #3,%d2
	beq		4f	

	move.w  #(LINE_DWORDS/2-1),%d0
	move.w #(~(0x808)),SCREEN_MEMORY_CONTROL	
2:
	movem.l (%a1)+,%d4-%d7 
	move.l  %d4,(%a0)+
	move.l  %d5,(%a0)+
	move.l  %d6,(%a0)+
	move.l  %d7,(%a0)+
	dbra	%d0,2b

	sub.l   %d1,%a0
	sub.l   %d1,%a1

	move.w  #(LINE_DWORDS/2-1),%d0
	move.w #(~(0x008)),SCREEN_MEMORY_CONTROL

3:
	movem.l (%a1)+,%d4-%d7 
	not.l	%d4
	not.l	%d5
	not.l	%d6
	not.l	%d7
	move.l  %d4,(%a0)+
	move.l  %d5,(%a0)+
	move.l  %d6,(%a0)+
	move.l  %d7,(%a0)+
	dbra	%d0,3b

	sub.l   %d1,%a0
	sub.l   %d1,%a1
4:

	add.l   %d1,%a0
	add.l	%d1,%a1
	
	cmp.l %a2,%a1
	blt     1b	

	move.l (28+24)(%sp),%d1     /* fillMode */
	move.w %d1,SCREEN_MEMORY_CONTROL 
	move.l #0x000F000F,%d7
	move.l %d7,%d6
	move.l %d7,%d5
	move.l %d7,%d4
1:
	movem.l %d4-%d7,-(%a1) 
	cmp.l	%a0,%a1
	bgt		1b

	movem.l (%sp)+, %d2-%d7/%a2
	rts		
#endif	
	
    .type scrollUp, @function 
scrollUp:  /* void scrollUp(uint16_t lines, uint16_t leftX, uint16_t topY, uint16_t rightX, uint16_t bottomY, 
					uint16_t fillMode, uint8_t bitplanes) */
		   /* leftX and rightX must be divisible by 4 and their difference by 8 */
	movem.l %d2-%d7/%a2, -(%sp)
	move.l (28+4)(%sp),%d0     /* lines */
	ext.l  %d0
	move.l (28+8)(%sp),%d4     /* leftX */
	ext.l  %d4
	move.l (28+12)(%sp),%d5     /* topY */
	ext.l  %d5
	move.l (28+16)(%sp),%d6     /* rightX */
	ext.l  %d6
	move.l (28+20)(%sp),%d7     /* bottomY */
	ext.l  %d7
	move.l (28+28)(%sp),%d2    /* bitplanes */
	mulu.w #(LINE_BYTES),%d0  /* distance to move */
	mulu.w #(LINE_BYTES),%d5  /* offset of top line of window */
	mulu.w #(LINE_BYTES),%d7  /* offset of bottom line of window */
	lsr.w  #1,%d4			  /* byte offset of leftX */
	add.w  %d4,%d5			  /* %d5: offset of start of window */
	lsr.w  #1,%d6			  /* byte offset for top */
	sub.w  %d4,%d6			  /* byte count per line */
	lsr.w  #2,%d6			  /* dword count per line */
	sub.w  #1,%d6			  /* %d6: rep count per line */
	move.l #(SCREEN),%a0      
	add.l  %d5,%a0            /* destination pointer */
	move.l %a0,%a1
	add.l  %d0,%a1			  /* source pointer */
	move.l #(SCREEN),%a2
	add.l  %d7,%a2			  /* bottom */

#ifndef NO_SCROLL_OPTIMIZE	
	/* special case optimization */
	cmp.w  #1,%d2             /* bitplane 0 */
	bne	   0f
	cmp.w  #(LINE_DWORDS-1),%d6 /* full lines */
	bne	   0f
	btst.l #0,(28+4)(%sp)      
	bne	   0f                   /* vertical distance is even */
	move.l (28+12)(%sp),%d5     
	move.l (28+20)(%sp),%d7     
	sub.l  %d5,%d7
	btst.l #0,%d7				/* height is even */
	beq    wholeLinesPlane0   /* optimized for: even number of lines + full lines + bit plane 0 */
#endif	
	
0:
	cmp.l  %a2,%a1
	bge	   1f 
	
/* move d6+1 words from a0 to a1 */
	btst #0,%d2
	beq notBit0 
	/* copy 1s */
	move.w #(~(0x101)),SCREEN_MEMORY_CONTROL
	bsr copyLine
	move.w #(~(0x001)),SCREEN_MEMORY_CONTROL
	bsr copyLineNeg
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
	add.l  #(LINE_BYTES),%a0
	add.l  #(LINE_BYTES),%a1
	bra 0b
1:

/* clear bottom */
	move.l (28+24)(%sp),%d1     /* fillMode */
	move.w %d1,SCREEN_MEMORY_CONTROL

	move.l %a0,%a1
	add.l  %d0,%a1

	move.w #(LINE_BYTES),%d7
	move.w %d6,%d4
	add.w  #1,%d6
	add.w  %d6,%d6
	add.w  %d6,%d6 /* number of bytes to clear per line */
	sub.w  %d6,%d7 /* offset between end of one line and beginning of next */
	ext.l  %d7	
	
	move.l #0x000F000F, %d0
2:
	move.w %d4,%d6
3:	
	move.l %d0,(%a0)+
	dbra   %d4,3b
	add.w  %d7,%a0
	cmp.l  %a1,%a0
	bge 4f
	move.w %d6,%d4
	bra 2b
4:
	movem.l (%sp)+, %d2-%d7/%a2
	rts

copyLine: /* copy %d6 dwords from a1 to a0 clobbering d3, d4, d5, d7 */
	move.l %a0,%d3
	move.l %a1,%d5
	move.w %d6,%d7
1:
	move.l (%a1)+,%d4
	move.l %d4,(%a0)+
	dbra   %d6,1b

	move.l %d3,%a0
	move.l %d5,%a1
	move.w %d7,%d6
	rts
	
copyLineNeg: /* same, but negating */
	move.l %a0,%d3
	move.l %a1,%d5
	move.w %d6,%d7
1:
	move.l (%a1)+,%d4
	not.l  %d4
	move.l %d4,(%a0)+
	dbra   %d6,1b

	move.l %d3,%a0
	move.l %d5,%a1
	move.w %d7,%d6
	rts
	