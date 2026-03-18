    .globl drawLine

#ifndef SCREEN_WIDTH
# define SCREEN_WIDTH 592
#endif
	
#define LINE_BYTES (SCREEN_WIDTH/4*2)
#define LINE_WORDS (SCREEN_WIDTH/4)
#define LINE_DWORDS (SCREEN_WIDTH/8)
#define SCREEN 0x620000	

#define ARGL(saved,n) ((saved)*4+4+4*(n))(%sp)
#define ARGW(saved,n) ((saved)*4+4+4*(n)+2)(%sp)
#define ARGB(saved,n) ((saved)*4+4+4*(n)+3)(%sp)

#define x1     0
#define y1     1
#define x2     2
#define y2     3
	
    .text
    .align 2
    .type drawLine, @function 
	.section	.text.drawLine,"ax",@progbits
drawLine:  /* void drawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) */
#define LINE_SAVED 4		   
#define LINE_SAVE_REGS %d2-%d5
	movem.l LINE_SAVE_REGS, -(%sp)
	
	move.w ARGW(LINE_SAVED,x1),%D0
	move.w ARGW(LINE_SAVED,y1),%D1
	move.w ARGW(LINE_SAVED,x2),%D2
	move.w ARGW(LINE_SAVED,y2),%D3
		
    cmp.w  %D0,%D2
    bhs    leftToRight
    exg    %D0,%D2
    exg    %D1,%D3
leftToRight:    
    // ensured x1<=x2
    sub.w  %D0,%D2
// %D2=x2-x1 (non-negative)
    move.w %D3,%D5
    move.l #LINE_BYTES,%D4
    sub.w  %D1,%D5
    bhs    notNeg1
    neg.w  %D5
    move.l #(-LINE_BYTES),%D4
notNeg1:
// %D5=|y2-y1|
// %D4=ychange
// %D3 is now free
    move.l #SCREEN,%A0
    move.w %D0,%D3
    lsr.w  #2,%D3 // %D3=x1/4
    lsl.w  #1,%D3 // byte offset for x
    ext.l  %D3
    add.l  %D3,%A0
    move.w %D1,%D3 // y1
    mulu   #LINE_BYTES,%D3
    add.l  %D3,%A0
// %A0=screen address
    moveq.l #8,%D3
    and.w   #3,%D0
    lsr.w   %D0,%D3 
// %D3=bitmask
// %D0 clobbered
    cmp.w   %D5,%D2
    blo     moreVertical
//
// more horizontal
// %D3=bitmask
// %D2=dx
// %D5=dy
// %D4=incy
    tst.w   %D5
    beq     horizontal
    move.w  %D5,%D0
    add.w   %D0,%D0
// %D0=2 dy    
    move.w  %D0,%D1
    sub.w   %D2,%D1
// %D1=error, initially 2 dy - dx
    sub.w   %D2,%D5
    add.w   %D5,%D5
//  %D5=2(dy-dx)
loop1:
    move.w  %D3,(%A0) 
    lsr.w   #1,%D3
    bne     notZero1
    move.w  #8,%D3
    addq.l  #2,%A0
notZero1:    
    tst.w   %D1
    blt     notPosError1
    // error>0
    add     %D4,%A0
    add     %D5,%D1 // error += 2(dy-dx)
    dbra    %D2,loop1 // dec dx   
	movem.l (%sp)+, LINE_SAVE_REGS
    rts
notPosError1:    
    add     %D0,%D1 // error += 2 dy
    dbra    %D2,loop1
	movem.l (%sp)+, LINE_SAVE_REGS
    rts   
horizontal:
// D2=dx
// D3=mask
// A0=address
    move.w  #8,%D1
    moveq.l #2,%D0    
loop3:
    move.w  %D3,(%A0) 
    lsr.w   #1,%D3
    bne     notZero3
    add.l   %D0,%A0
    move.w  %D1,%D3    // D1=8: reset bitmask
    cmp.w   %D1,%D2
    blo     lessThan8
    move.l  #0x000F000F,%D4 
fastLoop:    
    move.l  %D4,(%A0)+
    sub.w   %D1,%D2    // D1=8
    cmp.w   %D1,%D2
    bhs     fastLoop
lessThan8: 
notZero3:    
    dbra    %D2,loop3 // dec dx   
	movem.l (%sp)+, LINE_SAVE_REGS
    rts        
moreVertical:
// %D3=bitmask
// %D2=dx
// %D5=dy
// %D4=incy
    tst.w   %D2
    beq     vertical
    move.w  %D2,%D0
    add.w   %D0,%D0
// %D0=2 dx    
    move.w  %D0,%D1
    sub.w   %D5,%D1
// %D1=error, initially 2 dx-dy
    sub.w   %D5,%D2
    add.w   %D2,%D2
// %D2=2(dx-dy)
loop2:
    move.w  %D3,(%A0) 
    add.l   %D4,%A0
    
    tst.w   %D1
    blt     notPosError2
    // error>0
    lsr.w   #1,%D3
    bne     notZero2
    move.w  #8,%D3
    addq.l  #2,%A0
notZero2:    
    add     %D2,%D1 // error += 2(dx-dy)
    dbra    %D5,loop2 // dec dy   
	movem.l (%sp)+, LINE_SAVE_REGS
    rts
notPosError2:    
    add     %D0,%D1 // error += 2 dx
    dbra    %D5,loop2
	movem.l (%sp)+, LINE_SAVE_REGS
    rts            
vertical:
    move.w  %D3,(%A0) 
    add.l   %D4,%A0
    dbra    %D5,vertical
	movem.l (%sp)+, LINE_SAVE_REGS
    rts
	