    .text
    .align 2
    .globl blacksquare
    .type blacksquare, @function 
blacksquare:  /* void blacksquare(uint16_t x,uint16_t y) */


	movem.l %d0-%d2/%a0,-(%sp)
    move.w 26(%sp),%d0 /* y */
    mulu.w #(592/4),%d0     /* d0.w = y*(WIDTH/4) */
    move.w 22(%sp),%d1      /* x */
    move.w %d1,%d2
    lsr.w #2,%d1           /* d1 = x/4 */
    add.w %d1,%d0          /* d0 = y*(WIDTH/4) + x/4 */
    and.w #0xFFFF,%d0      
    add.l %d0,%d0          /* this may exceed 64k */
	move.l #0x600000,%a0
    add.l %d0,%a0          /* a0 = 0x600000 + (y*WIDTH/4+x/4)*2 */
    btst #0,%d2
    beq .even
    btst #1,%d2
    beq .image1

/* image 3 */
    move.l #0x0001000f,%d0
    move.l %d0,(%a0)
    move.l %d0,0x0128(%a0)
    move.l %d0,0x0250(%a0)
    move.l %d0,0x0378(%a0)
    move.l %d0,0x04a0(%a0)
    move.l %d0,0x05c8(%a0)
    move.l %d0,0x06f0(%a0)
    move.l %d0,0x0818(%a0)
    move.l %d0,0x0940(%a0)
    move.l %d0,0x0a68(%a0)
    move.l %d0,0x0b90(%a0)
    move.l %d0,0x0cb8(%a0)
    move.l %d0,0x0de0(%a0)
    move.l %d0,0x0f08(%a0)
    move.l %d0,0x1030(%a0)
    move.l %d0,0x1158(%a0)
    move.l #0x000f000f,%d0
    move.l %d0,0x0004(%a0)
    move.l %d0,0x012c(%a0)
    move.l %d0,0x0254(%a0)
    move.l %d0,0x037c(%a0)
    move.l %d0,0x04a4(%a0)
    move.l %d0,0x05cc(%a0)
    move.l %d0,0x06f4(%a0)
    move.l %d0,0x081c(%a0)
    move.l %d0,0x0944(%a0)
    move.l %d0,0x0a6c(%a0)
    move.l %d0,0x0b94(%a0)
    move.l %d0,0x0cbc(%a0)
    move.l %d0,0x0de4(%a0)
    move.l %d0,0x0f0c(%a0)
    move.l %d0,0x1034(%a0)
    move.l %d0,0x115c(%a0)
    move.l #0x000f0008,%d0
    move.l %d0,0x0008(%a0)
    move.l %d0,0x0130(%a0)
    move.l %d0,0x0258(%a0)
    move.l %d0,0x0380(%a0)
    move.l %d0,0x04a8(%a0)
    move.l %d0,0x05d0(%a0)
    move.l %d0,0x06f8(%a0)
    move.l %d0,0x0820(%a0)
    move.l %d0,0x0948(%a0)
    move.l %d0,0x0a70(%a0)
    move.l %d0,0x0b98(%a0)
    move.l %d0,0x0cc0(%a0)
    move.l %d0,0x0de8(%a0)
    move.l %d0,0x0f10(%a0)
    move.l %d0,0x1038(%a0)
    move.l %d0,0x1160(%a0)

    movem.l (%sp)+,%d0-%d2/%a0
    rts
.even:
    btst #1,%d2
    beq .image0
/* image 2 */
    move.l #0x0003000f,%d0
    move.l %d0,(%a0)
    move.l %d0,0x0128(%a0)
    move.l %d0,0x0250(%a0)
    move.l %d0,0x0378(%a0)
    move.l %d0,0x04a0(%a0)
    move.l %d0,0x05c8(%a0)
    move.l %d0,0x06f0(%a0)
    move.l %d0,0x0818(%a0)
    move.l %d0,0x0940(%a0)
    move.l %d0,0x0a68(%a0)
    move.l %d0,0x0b90(%a0)
    move.l %d0,0x0cb8(%a0)
    move.l %d0,0x0de0(%a0)
    move.l %d0,0x0f08(%a0)
    move.l %d0,0x1030(%a0)
    move.l %d0,0x1158(%a0)
    move.w %d0,0x0008(%a0)
    move.w %d0,0x0130(%a0)
    move.w %d0,0x0258(%a0)
    move.w %d0,0x0380(%a0)
    move.w %d0,0x04a8(%a0)
    move.w %d0,0x05d0(%a0)
    move.w %d0,0x06f8(%a0)
    move.w %d0,0x0820(%a0)
    move.w %d0,0x0948(%a0)
    move.w %d0,0x0a70(%a0)
    move.w %d0,0x0b98(%a0)
    move.w %d0,0x0cc0(%a0)
    move.w %d0,0x0de8(%a0)
    move.w %d0,0x0f10(%a0)
    move.w %d0,0x1038(%a0)
    move.w %d0,0x1160(%a0)
    move.l #0x000f000f,%d0
    move.l %d0,0x0004(%a0)
    move.l %d0,0x012c(%a0)
    move.l %d0,0x0254(%a0)
    move.l %d0,0x037c(%a0)
    move.l %d0,0x04a4(%a0)
    move.l %d0,0x05cc(%a0)
    move.l %d0,0x06f4(%a0)
    move.l %d0,0x081c(%a0)
    move.l %d0,0x0944(%a0)
    move.l %d0,0x0a6c(%a0)
    move.l %d0,0x0b94(%a0)
    move.l %d0,0x0cbc(%a0)
    move.l %d0,0x0de4(%a0)
    move.l %d0,0x0f0c(%a0)
    move.l %d0,0x1034(%a0)
    move.l %d0,0x115c(%a0)

    movem.l (%sp)+,%d0-%d2/%a0
    rts
.image0:
/* image 0 */
    move.l #0x000f000f,%d0
    move.l %d0,(%a0)
    move.l %d0,0x0004(%a0)
    move.l %d0,0x0128(%a0)
    move.l %d0,0x012c(%a0)
    move.l %d0,0x0250(%a0)
    move.l %d0,0x0254(%a0)
    move.l %d0,0x0378(%a0)
    move.l %d0,0x037c(%a0)
    move.l %d0,0x04a0(%a0)
    move.l %d0,0x04a4(%a0)
    move.l %d0,0x05c8(%a0)
    move.l %d0,0x05cc(%a0)
    move.l %d0,0x06f0(%a0)
    move.l %d0,0x06f4(%a0)
    move.l %d0,0x0818(%a0)
    move.l %d0,0x081c(%a0)
    move.l %d0,0x0940(%a0)
    move.l %d0,0x0944(%a0)
    move.l %d0,0x0a68(%a0)
    move.l %d0,0x0a6c(%a0)
    move.l %d0,0x0b90(%a0)
    move.l %d0,0x0b94(%a0)
    move.l %d0,0x0cb8(%a0)
    move.l %d0,0x0cbc(%a0)
    move.l %d0,0x0de0(%a0)
    move.l %d0,0x0de4(%a0)
    move.l %d0,0x0f08(%a0)
    move.l %d0,0x0f0c(%a0)
    move.l %d0,0x1030(%a0)
    move.l %d0,0x1034(%a0)
    move.l %d0,0x1158(%a0)
    move.l %d0,0x115c(%a0)
    moveq #0x0c,%d0
    move.w %d0,0x0008(%a0)
    move.w %d0,0x0130(%a0)
    move.w %d0,0x0258(%a0)
    move.w %d0,0x0380(%a0)
    move.w %d0,0x04a8(%a0)
    move.w %d0,0x05d0(%a0)
    move.w %d0,0x06f8(%a0)
    move.w %d0,0x0820(%a0)
    move.w %d0,0x0948(%a0)
    move.w %d0,0x0a70(%a0)
    move.w %d0,0x0b98(%a0)
    move.w %d0,0x0cc0(%a0)
    move.w %d0,0x0de8(%a0)
    move.w %d0,0x0f10(%a0)
    move.w %d0,0x1038(%a0)
    move.w %d0,0x1160(%a0)

    movem.l (%sp)+,%d0-%d2/%a0
    rts
.image1:
/* image 1 */
    move.l #0x0007000f,%d0
    move.l %d0,(%a0)
    move.l %d0,0x0128(%a0)
    move.l %d0,0x0250(%a0)
    move.l %d0,0x0378(%a0)
    move.l %d0,0x04a0(%a0)
    move.l %d0,0x05c8(%a0)
    move.l %d0,0x06f0(%a0)
    move.l %d0,0x0818(%a0)
    move.l %d0,0x0940(%a0)
    move.l %d0,0x0a68(%a0)
    move.l %d0,0x0b90(%a0)
    move.l %d0,0x0cb8(%a0)
    move.l %d0,0x0de0(%a0)
    move.l %d0,0x0f08(%a0)
    move.l %d0,0x1030(%a0)
    move.l %d0,0x1158(%a0)
    move.l #0x000f000f,%d0
    move.l %d0,0x0004(%a0)
    move.l %d0,0x012c(%a0)
    move.l %d0,0x0254(%a0)
    move.l %d0,0x037c(%a0)
    move.l %d0,0x04a4(%a0)
    move.l %d0,0x05cc(%a0)
    move.l %d0,0x06f4(%a0)
    move.l %d0,0x081c(%a0)
    move.l %d0,0x0944(%a0)
    move.l %d0,0x0a6c(%a0)
    move.l %d0,0x0b94(%a0)
    move.l %d0,0x0cbc(%a0)
    move.l %d0,0x0de4(%a0)
    move.l %d0,0x0f0c(%a0)
    move.l %d0,0x1034(%a0)
    move.l %d0,0x115c(%a0)
    moveq #0x0e,%d0
    move.w %d0,0x0008(%a0)
    move.w %d0,0x0130(%a0)
    move.w %d0,0x0258(%a0)
    move.w %d0,0x0380(%a0)
    move.w %d0,0x04a8(%a0)
    move.w %d0,0x05d0(%a0)
    move.w %d0,0x06f8(%a0)
    move.w %d0,0x0820(%a0)
    move.w %d0,0x0948(%a0)
    move.w %d0,0x0a70(%a0)
    move.w %d0,0x0b98(%a0)
    move.w %d0,0x0cc0(%a0)
    move.w %d0,0x0de8(%a0)
    move.w %d0,0x0f10(%a0)
    move.w %d0,0x1038(%a0)
    move.w %d0,0x1160(%a0)

    movem.l (%sp)+,%d0-%d2/%a0
    rts
