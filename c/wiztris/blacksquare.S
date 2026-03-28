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
