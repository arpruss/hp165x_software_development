    .globl scrollDown
	.globl scrollUp

#ifndef SCREEN_WIDTH
# define SCREEN_WIDTH 592
#endif
	
#if SCREEN_WIDTH != 592 && SCREEN_WIDTH != 640
# define NO_SCROLL_OPTIMIZE 
#endif

#define LINE_BYTES (SCREEN_WIDTH/4*2)
#define LINE_WORDS (SCREEN_WIDTH/4)
#define LINE_DWORDS (SCREEN_WIDTH/8)
#define SCREEN 0x600000	
#define SCREEN_MEMORY_CONTROL 0x00201000

#define ARGL(saved,n) ((saved)*4+4+4*(n))(%sp)
#define ARGW(saved,n) ((saved)*4+4+4*(n)+2)(%sp)
#define ARGB(saved,n) ((saved)*4+4+4*(n)+3)(%sp)

#define lines     0
#define leftX     1
#define topY      2
#define rightX    3
#define bottomY   4
#define fillMode  5
#define bitplanes 6
	
    .text
    .align 2
    .type scrollDown, @function 
	.section	.text.scrollUp,"ax",@progbits
scrollDown:  /* void scrollDown(uint16_t lines, uint16_t leftX, uint16_t topY, uint16_t rightX, uint16_t bottomY, 
					uint16_t fillMode, uint8_t bitplanes) */
		   /* leftX and rightX must be divisible by 4 and their difference by 8 */
#define SCROLLDOWN_SAVED 7		   
#define SCROLLDOWN_SAVE_REGS %d2-%d7/%a2
	movem.l SCROLLDOWN_SAVE_REGS, -(%sp)
	move.l ARGL(SCROLLDOWN_SAVED,lines),%d0     /* lines */
	ext.l  %d0
	move.l ARGL(SCROLLDOWN_SAVED,leftX),%d4     /* leftX */
	ext.l  %d4
	move.l ARGL(SCROLLDOWN_SAVED,topY),%d5     /* topY */
	ext.l  %d5
	move.l ARGL(SCROLLDOWN_SAVED,rightX),%d6     /* rightX */
	ext.l  %d6
	move.l ARGL(SCROLLDOWN_SAVED,bottomY),%d7     /* bottomY */
	ext.l  %d7
	subq.l #1,%d7
	move.l ARGL(SCROLLDOWN_SAVED,bitplanes),%d2    /* bitplanes */
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
	bsr copyLineNot
	move.w #(~(0x101)),SCREEN_MEMORY_CONTROL
	bsr copyLine
notBit0Down:
	btst #1,%d2
	beq notBit1Down
	/* copy 1s */
	move.w #(~(0x202)),SCREEN_MEMORY_CONTROL
	bsr copyLine
  	move.w #(~(0x002)),SCREEN_MEMORY_CONTROL
	bsr copyLineNot
notBit1Down:
	btst #2,%d2
	beq notBit2Down
	/* copy 1s */
	move.w #(~(0x404)),SCREEN_MEMORY_CONTROL
	bsr copyLine
	move.w #(~(0x004)),SCREEN_MEMORY_CONTROL
	bsr copyLineNot
notBit2Down:
	btst #3,%d2
	beq notBit3Down
	/* copy 1s */
	move.w #(~(0x808)),SCREEN_MEMORY_CONTROL
	bsr copyLine
	move.w #(~(0x008)),SCREEN_MEMORY_CONTROL
	bsr copyLineNot
notBit3Down:
	sub.l  #(LINE_BYTES),%a0
	sub.l  #(LINE_BYTES),%a1
	bra 0b
1:

/* clear top */
	move.w ARGW(SCROLLDOWN_SAVED,fillMode),SCREEN_MEMORY_CONTROL

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
	movem.l (%sp)+, SCROLLDOWN_SAVE_REGS
	rts

	.section	.text.scrollUp,"ax",@progbits

#define SCROLLUP_SAVED 7		   
#define SCROLLUP_SAVE_REGS %d2-%d7/%a2

#ifndef NO_SCROLL_OPTIMIZE
copyTwoLines:
/* %a1 = source, %a0 = dest */
	.macro  move52
	movem.l (%a1)+,%d0-%d7/%a2-%a6 /* 52 bytes */
	movem.l %d0-%d7/%a2-%a6,(%a0)
	lea 52(%a0),%a0
	.endm
	
	.rept 11
	move52 
	.endr
	/* moved 572 bytes */
#if SCREEN_WIDTH==592
	/* need to move 20 bytes more */
	movem.l (%a1)+,%d0-%d4 /* 20 bytes */
	movem.l %d0-%d4,(%a0)
	lea 20(%a0),%a0
#elif SCREEN_WIDTH==640
	move52
	/* moved 624 bytes, need to move 16 more */
	movem.l (%a1)+,%d0-%d3 /* 16 bytes */
	movem.l %d0-%d3,(%a0)
	lea 16(%a0),%a0
#else
#error Unsupported width for optimization.
#endif
	
	move.l  #(LINE_BYTES*2),%d0
	sub.l	%d0,%a0
	sub.l	%d0,%a1

	rts

copyTwoLinesNot:
/* %a1 = source, %a0 = dest */

	.macro  move32not
	movem.l (%a1)+,%d0-%d7 /* 48 bytes */
	not.l	%d0
	not.l	%d1
	not.l	%d2
	not.l	%d3
	not.l	%d4
	not.l	%d5
	not.l	%d6
	not.l	%d7
	movem.l %d0-%d7,(%a0)
	lea 32(%a0),%a0
	.endm

	.rept 18
	move32not
	.endr
	/* moved 576 bytes */	
#if SCREEN_WIDTH==592
	/* need to move 16 bytes more */
	movem.l (%a1)+,%d0-%d3 /* 16 bytes */
	not.l	%d0
	not.l	%d1
	not.l	%d2
	not.l	%d3
	movem.l %d0-%d3,(%a0)
	lea 16(%a0),%a0
#elif SCREEN_WIDTH==640
	/* need to move 64 bytes more */
	move32not
	move32not
#else
#error Unsupported width for optimization.
#endif
	
	move.l  #(LINE_BYTES*2),%d0
	sub.l	%d0,%a0
	sub.l	%d0,%a1

	rts

	.globl _bitplanes
_bitplanes:
	.byte   0
	.byte   0 /* for alignment */
_bottom:
	.long	0
	
wholeLines:
/* %a1 = source, %a0 = dest, %a2 = bottom: keep copying until %a1 >= %a2 */
/* assume scrolling is by an even number of lines */
/* %d2 = bitplanes */
/* use all the other data registers as scrap */
/* assumes: even number of lines to scroll, even height of region */
	move.b  %d2,_bitplanes 
	move.l  %a2,_bottom

#define WHOLELINES_SAVE_REGS %a3-%a6
#define WHOLELINES_SAVED 	 4
	movem.l WHOLELINES_SAVE_REGS,-(%sp)

1:
	btst.b  #0,_bitplanes
	beq		4f

	move.w #(~(0x101)),SCREEN_MEMORY_CONTROL	
	bsr		copyTwoLines
	move.w #(~(0x001)),SCREEN_MEMORY_CONTROL
	bsr		copyTwoLinesNot

4:
	btst.b  #1,_bitplanes
	beq		4f	

	move.w #(~(0x202)),SCREEN_MEMORY_CONTROL	
	bsr		copyTwoLines
	move.w #(~(0x002)),SCREEN_MEMORY_CONTROL
	bsr		copyTwoLinesNot

4:
	btst.b  #2,_bitplanes
	beq		4f	

	move.w #(~(0x404)),SCREEN_MEMORY_CONTROL	
	bsr		copyTwoLines
	move.w #(~(0x004)),SCREEN_MEMORY_CONTROL
	bsr		copyTwoLinesNot

4:
	btst.l  #3,_bitplanes
	beq		4f	

	move.w #(~(0x808)),SCREEN_MEMORY_CONTROL	
	bsr		copyTwoLines
	move.w #(~(0x008)),SCREEN_MEMORY_CONTROL
	bsr		copyTwoLinesNot

4:
	move.l  #(LINE_BYTES*2),%d0 

	add.l	%d0,%a0
	add.l	%d0,%a1
	
	cmp.l   _bottom,%a1
	blt     1b	
	
	/* at this point, %a1 should be pointing to the bottom of the clear area
	   and %a0 to the top of the clear area */

	move.w ARGW(SCROLLUP_SAVED+WHOLELINES_SAVED,fillMode),SCREEN_MEMORY_CONTROL
	move.l #0x000F000F,%d7
	move.l %d7,%d6
	move.l %d7,%d5
	move.l %d7,%d4
1:
	movem.l %d4-%d7,-(%a1) 
	cmp.l	%a0,%a1
	bgt		1b /* a1 > a0 ==> not yet done */

	movem.l (%sp)+, WHOLELINES_SAVE_REGS
	movem.l (%sp)+, SCROLLUP_SAVE_REGS 

	rts		
#endif	
	
    .type scrollUp, @function 
scrollUp:  /* void scrollUp(uint16_t lines, uint16_t leftX, uint16_t topY, uint16_t rightX, uint16_t bottomY, 
					uint16_t fillMode, uint8_t bitplanes) */
		   /* leftX and rightX must be divisible by 4 and their difference by 8 */
	movem.l SCROLLUP_SAVE_REGS, -(%sp)
	move.l ARGL(SCROLLUP_SAVED,lines),%d0     /* lines */
	ext.l  %d0
	move.l ARGL(SCROLLUP_SAVED,leftX),%d4     /* leftX */
	ext.l  %d4
	move.l ARGL(SCROLLUP_SAVED,topY),%d5     /* topY */
	ext.l  %d5
	move.l ARGL(SCROLLUP_SAVED,rightX),%d6     /* rightX */
	ext.l  %d6
	move.l ARGL(SCROLLUP_SAVED,bottomY),%d7     /* bottomY */
	ext.l  %d7
	move.l ARGL(SCROLLUP_SAVED,bitplanes),%d2    /* bitplanes */
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
   	cmp.w  #(LINE_DWORDS-1),%d6 /* full lines */
	bne	   0f
	btst.l #0,ARGL(SCROLLUP_SAVED,lines)      
	bne	   0f                   /* vertical distance is even */
	move.l ARGL(SCROLLUP_SAVED,topY),%d5     
	move.l ARGL(SCROLLUP_SAVED,bottomY),%d7     
	sub.l  %d5,%d7
	btst.l #0,%d7				/* height is even */
	beq    wholeLines           /* optimized for: even number of lines + full lines */
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
	bsr copyLineNot
notBit0:
	btst #1,%d2
	beq notBit1 
	/* copy 1s */
	move.w #(~(0x202)),SCREEN_MEMORY_CONTROL
	bsr copyLine
	move.w #(~(0x002)),SCREEN_MEMORY_CONTROL
	bsr copyLineNot
notBit1:
	btst #2,%d2
	beq notBit2
	/* copy 1s */
	move.w #(~(0x404)),SCREEN_MEMORY_CONTROL
	bsr copyLine
	move.w #(~(0x004)),SCREEN_MEMORY_CONTROL
	bsr copyLineNot
notBit2:
	btst #3,%d2
	beq notBit3
	/* copy 1s */
	move.w #(~(0x808)),SCREEN_MEMORY_CONTROL
	bsr copyLine
	move.w #(~(0x008)),SCREEN_MEMORY_CONTROL
	bsr copyLineNot
notBit3:
	add.l  #(LINE_BYTES),%a0
	add.l  #(LINE_BYTES),%a1
	bra 0b
1:

/* clear bottom */
	move.w ARGW(SCROLLUP_SAVED,fillMode),SCREEN_MEMORY_CONTROL

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
	movem.l (%sp)+, SCROLLUP_SAVE_REGS
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
	
copyLineNot: /* same, but negating */
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
	

	