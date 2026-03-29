    .text
#ifndef SCREEN_WIDTH
# define SCREEN_WIDTH 592
#endif
#define SCREEN 0x620000
    .align 2
    .globl blacksquare
    .type blacksquare, @function 
blacksquare:  /* void blacksquare(uint16_t x,uint16_t y) */


	movem.l %d0-%d2/%a0,-(%sp)
    move.w 26(%sp),%d0 /* y */
    mulu.w #(SCREEN_WIDTH/4),%d0     /* d0.w = y*(SCREEN_WIDTH/4) */
    move.w 22(%sp),%d1      /* x */
    move.w %d1,%d2          
    lsr.w #2,%d1           /* d1 = x/4 */
    add.w %d1,%d0          /* d0 = y*(WIDTH/4) + x/4 */
    and.w #0xFFFF,%d0      
    add.l %d0,%d0          /* this may exceed 64k */
	move.l #(SCREEN),%a0
    add.l %d0,%a0          /* a0 = SCREEN + (y*SCREEN_WIDTH/4+x/4)*2 */
                           /* d2 = x */
    cmp.w #(SCREEN_WIDTH-19),%d2
    bhi .clip
    btst #0,%d2
    beq .even
    btst #1,%d2
    beq .image1
/* image 3 */
    move.l #0x010f0f0f,%d0
    movep.l %d0,(1+0*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+1*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+2*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+3*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+4*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+5*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+6*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+7*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+8*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+9*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+10*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+11*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+12*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+13*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+14*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+15*(SCREEN_WIDTH/2))(%a0)
    move.l #0x000f0008,%d0
    move.l %d0,(8+0*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(8+1*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(8+2*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(8+3*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(8+4*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(8+5*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(8+6*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(8+7*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(8+8*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(8+9*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(8+10*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(8+11*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(8+12*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(8+13*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(8+14*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(8+15*(SCREEN_WIDTH/2))(%a0)

    movem.l (%sp)+,%d0-%d2/%a0
    rts
.even:
    btst #1,%d2
    beq .image0
/* image 2 */
    move.l #0x030f0f0f,%d0
    movep.l %d0,(1+0*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+1*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+2*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+3*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+4*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+5*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+6*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+7*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+8*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+9*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+10*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+11*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+12*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+13*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+14*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+15*(SCREEN_WIDTH/2))(%a0)
    moveq #0x0f,%d0
    move.w %d0,(8+0*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+1*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+2*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+3*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+4*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+5*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+6*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+7*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+8*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+9*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+10*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+11*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+12*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+13*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+14*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+15*(SCREEN_WIDTH/2))(%a0)

    movem.l (%sp)+,%d0-%d2/%a0
    rts
.image0:
/* image 0 */
    move.l #0x0f0f0f0f,%d0
    movep.l %d0,(1+0*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+1*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+2*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+3*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+4*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+5*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+6*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+7*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+8*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+9*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+10*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+11*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+12*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+13*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+14*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+15*(SCREEN_WIDTH/2))(%a0)
    moveq #0x0c,%d0
    move.w %d0,(8+0*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+1*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+2*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+3*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+4*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+5*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+6*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+7*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+8*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+9*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+10*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+11*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+12*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+13*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+14*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+15*(SCREEN_WIDTH/2))(%a0)

    movem.l (%sp)+,%d0-%d2/%a0
    rts
.image1:
/* image 1 */
    move.l #0x070f0f0f,%d0
    movep.l %d0,(1+0*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+1*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+2*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+3*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+4*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+5*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+6*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+7*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+8*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+9*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+10*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+11*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+12*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+13*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+14*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+15*(SCREEN_WIDTH/2))(%a0)
    moveq #0x0e,%d0
    move.w %d0,(8+0*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+1*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+2*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+3*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+4*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+5*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+6*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+7*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+8*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+9*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+10*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+11*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+12*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+13*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+14*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+15*(SCREEN_WIDTH/2))(%a0)

    movem.l (%sp)+,%d0-%d2/%a0
    rts
.clip:
    sub.w #(SCREEN_WIDTH-1),%d2

    bhi .done
    neg.w %d2 /* %d2 = SCREEN_WIDTH-1-x */
    add %d2,%d2
    add %d2,%d2
    move.l #.jumpTable,%a1
    move.l (0,%a1,%d2.l),%a1
    jmp (%a1)
    
.clipped1:
/* image 3 */
    moveq #0x01,%d0
    move.w %d0,(%a0)
    move.w %d0,(0+1*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(0+2*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(0+3*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(0+4*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(0+5*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(0+6*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(0+7*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(0+8*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(0+9*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(0+10*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(0+11*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(0+12*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(0+13*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(0+14*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(0+15*(SCREEN_WIDTH/2))(%a0)

    movem.l (%sp)+,%d0-%d2/%a0
    rts
.clipped2:
/* image 2 */
    moveq #0x03,%d0
    move.w %d0,(%a0)
    move.w %d0,(0+1*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(0+2*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(0+3*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(0+4*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(0+5*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(0+6*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(0+7*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(0+8*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(0+9*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(0+10*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(0+11*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(0+12*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(0+13*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(0+14*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(0+15*(SCREEN_WIDTH/2))(%a0)

    movem.l (%sp)+,%d0-%d2/%a0
    rts
.clipped3:
/* image 1 */
    moveq #0x07,%d0
    move.w %d0,(%a0)
    move.w %d0,(0+1*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(0+2*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(0+3*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(0+4*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(0+5*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(0+6*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(0+7*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(0+8*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(0+9*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(0+10*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(0+11*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(0+12*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(0+13*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(0+14*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(0+15*(SCREEN_WIDTH/2))(%a0)

    movem.l (%sp)+,%d0-%d2/%a0
    rts
.clipped4:
/* image 0 */
    moveq #0x0f,%d0
    move.w %d0,(%a0)
    move.w %d0,(0+1*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(0+2*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(0+3*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(0+4*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(0+5*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(0+6*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(0+7*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(0+8*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(0+9*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(0+10*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(0+11*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(0+12*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(0+13*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(0+14*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(0+15*(SCREEN_WIDTH/2))(%a0)

    movem.l (%sp)+,%d0-%d2/%a0
    rts
.clipped5:
/* image 3 */
    move.l #0x0001000f,%d0
    move.l %d0,(%a0)
    move.l %d0,(0+1*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+2*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+3*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+4*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+5*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+6*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+7*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+8*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+9*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+10*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+11*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+12*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+13*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+14*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+15*(SCREEN_WIDTH/2))(%a0)

    movem.l (%sp)+,%d0-%d2/%a0
    rts
.clipped6:
/* image 2 */
    move.l #0x0003000f,%d0
    move.l %d0,(%a0)
    move.l %d0,(0+1*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+2*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+3*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+4*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+5*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+6*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+7*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+8*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+9*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+10*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+11*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+12*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+13*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+14*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+15*(SCREEN_WIDTH/2))(%a0)

    movem.l (%sp)+,%d0-%d2/%a0
    rts
.clipped7:
/* image 1 */
    move.l #0x0007000f,%d0
    move.l %d0,(%a0)
    move.l %d0,(0+1*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+2*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+3*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+4*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+5*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+6*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+7*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+8*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+9*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+10*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+11*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+12*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+13*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+14*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+15*(SCREEN_WIDTH/2))(%a0)

    movem.l (%sp)+,%d0-%d2/%a0
    rts
.clipped8:
/* image 0 */
    move.l #0x000f000f,%d0
    move.l %d0,(%a0)
    move.l %d0,(0+1*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+2*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+3*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+4*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+5*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+6*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+7*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+8*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+9*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+10*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+11*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+12*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+13*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+14*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+15*(SCREEN_WIDTH/2))(%a0)

    movem.l (%sp)+,%d0-%d2/%a0
    rts
.clipped9:
/* image 3 */
    move.l #0x0001000f,%d0
    move.l %d0,(%a0)
    move.l %d0,(0+1*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+2*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+3*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+4*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+5*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+6*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+7*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+8*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+9*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+10*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+11*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+12*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+13*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+14*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+15*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+0*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+1*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+2*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+3*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+4*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+5*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+6*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+7*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+8*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+9*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+10*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+11*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+12*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+13*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+14*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+15*(SCREEN_WIDTH/2))(%a0)

    movem.l (%sp)+,%d0-%d2/%a0
    rts
.clipped10:
/* image 2 */
    move.l #0x0003000f,%d0
    move.l %d0,(%a0)
    move.l %d0,(0+1*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+2*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+3*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+4*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+5*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+6*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+7*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+8*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+9*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+10*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+11*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+12*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+13*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+14*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+15*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+0*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+1*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+2*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+3*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+4*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+5*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+6*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+7*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+8*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+9*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+10*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+11*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+12*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+13*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+14*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+15*(SCREEN_WIDTH/2))(%a0)

    movem.l (%sp)+,%d0-%d2/%a0
    rts
.clipped11:
/* image 1 */
    move.l #0x0007000f,%d0
    move.l %d0,(%a0)
    move.l %d0,(0+1*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+2*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+3*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+4*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+5*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+6*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+7*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+8*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+9*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+10*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+11*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+12*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+13*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+14*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+15*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+0*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+1*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+2*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+3*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+4*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+5*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+6*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+7*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+8*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+9*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+10*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+11*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+12*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+13*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+14*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+15*(SCREEN_WIDTH/2))(%a0)

    movem.l (%sp)+,%d0-%d2/%a0
    rts
.clipped12:
/* image 0 */
    move.l #0x000f000f,%d0
    move.l %d0,(%a0)
    move.l %d0,(0+1*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+2*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+3*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+4*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+5*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+6*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+7*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+8*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+9*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+10*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+11*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+12*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+13*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+14*(SCREEN_WIDTH/2))(%a0)
    move.l %d0,(0+15*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+0*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+1*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+2*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+3*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+4*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+5*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+6*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+7*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+8*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+9*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+10*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+11*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+12*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+13*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+14*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(4+15*(SCREEN_WIDTH/2))(%a0)

    movem.l (%sp)+,%d0-%d2/%a0
    rts
.clipped13:
/* image 3 */
    move.l #0x010f0f0f,%d0
    movep.l %d0,(1+0*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+1*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+2*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+3*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+4*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+5*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+6*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+7*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+8*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+9*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+10*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+11*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+12*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+13*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+14*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+15*(SCREEN_WIDTH/2))(%a0)

    movem.l (%sp)+,%d0-%d2/%a0
    rts
.clipped14:
/* image 2 */
    move.l #0x030f0f0f,%d0
    movep.l %d0,(1+0*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+1*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+2*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+3*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+4*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+5*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+6*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+7*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+8*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+9*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+10*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+11*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+12*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+13*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+14*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+15*(SCREEN_WIDTH/2))(%a0)

    movem.l (%sp)+,%d0-%d2/%a0
    rts
.clipped15:
/* image 1 */
    move.l #0x070f0f0f,%d0
    movep.l %d0,(1+0*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+1*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+2*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+3*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+4*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+5*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+6*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+7*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+8*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+9*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+10*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+11*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+12*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+13*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+14*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+15*(SCREEN_WIDTH/2))(%a0)

    movem.l (%sp)+,%d0-%d2/%a0
    rts
.clipped16:
/* image 0 */
    move.l #0x0f0f0f0f,%d0
    movep.l %d0,(1+0*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+1*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+2*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+3*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+4*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+5*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+6*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+7*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+8*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+9*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+10*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+11*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+12*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+13*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+14*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+15*(SCREEN_WIDTH/2))(%a0)

    movem.l (%sp)+,%d0-%d2/%a0
    rts
.clipped17:
/* image 3 */
    move.l #0x010f0f0f,%d0
    movep.l %d0,(1+0*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+1*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+2*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+3*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+4*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+5*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+6*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+7*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+8*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+9*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+10*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+11*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+12*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+13*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+14*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+15*(SCREEN_WIDTH/2))(%a0)
    moveq #0x0f,%d0
    move.w %d0,(8+0*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+1*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+2*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+3*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+4*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+5*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+6*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+7*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+8*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+9*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+10*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+11*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+12*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+13*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+14*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+15*(SCREEN_WIDTH/2))(%a0)

    movem.l (%sp)+,%d0-%d2/%a0
    rts
.clipped18:
/* image 2 */
    move.l #0x030f0f0f,%d0
    movep.l %d0,(1+0*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+1*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+2*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+3*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+4*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+5*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+6*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+7*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+8*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+9*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+10*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+11*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+12*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+13*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+14*(SCREEN_WIDTH/2))(%a0)
    movep.l %d0,(1+15*(SCREEN_WIDTH/2))(%a0)
    moveq #0x0f,%d0
    move.w %d0,(8+0*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+1*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+2*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+3*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+4*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+5*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+6*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+7*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+8*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+9*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+10*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+11*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+12*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+13*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+14*(SCREEN_WIDTH/2))(%a0)
    move.w %d0,(8+15*(SCREEN_WIDTH/2))(%a0)

    movem.l (%sp)+,%d0-%d2/%a0
    rts
.jumpTable:
    .long .clipped1
    .long .clipped2
    .long .clipped3
    .long .clipped4
    .long .clipped5
    .long .clipped6
    .long .clipped7
    .long .clipped8
    .long .clipped9
    .long .clipped10
    .long .clipped11
    .long .clipped12
    .long .clipped13
    .long .clipped14
    .long .clipped15
    .long .clipped16
    .long .clipped17
    .long .clipped18
.done:
    movem.l (%sp)+,%d0-%d2/%a0
    rts
