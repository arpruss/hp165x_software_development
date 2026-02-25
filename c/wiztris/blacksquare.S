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
    move.l #0x010f0f0f,%d0
    movep.l %d0,0x0001(%a0)
    movep.l %d0,0x0129(%a0)
    movep.l %d0,0x0251(%a0)
    movep.l %d0,0x0379(%a0)
    movep.l %d0,0x04a1(%a0)
    movep.l %d0,0x05c9(%a0)
    movep.l %d0,0x06f1(%a0)
    movep.l %d0,0x0819(%a0)
    movep.l %d0,0x0941(%a0)
    movep.l %d0,0x0a69(%a0)
    movep.l %d0,0x0b91(%a0)
    movep.l %d0,0x0cb9(%a0)
    movep.l %d0,0x0de1(%a0)
    movep.l %d0,0x0f09(%a0)
    movep.l %d0,0x1031(%a0)
    movep.l %d0,0x1159(%a0)
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
    move.l #0x030f0f0f,%d0
    movep.l %d0,0x0001(%a0)
    movep.l %d0,0x0129(%a0)
    movep.l %d0,0x0251(%a0)
    movep.l %d0,0x0379(%a0)
    movep.l %d0,0x04a1(%a0)
    movep.l %d0,0x05c9(%a0)
    movep.l %d0,0x06f1(%a0)
    movep.l %d0,0x0819(%a0)
    movep.l %d0,0x0941(%a0)
    movep.l %d0,0x0a69(%a0)
    movep.l %d0,0x0b91(%a0)
    movep.l %d0,0x0cb9(%a0)
    movep.l %d0,0x0de1(%a0)
    movep.l %d0,0x0f09(%a0)
    movep.l %d0,0x1031(%a0)
    movep.l %d0,0x1159(%a0)
    moveq #0x0f,%d0
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
.image0:
/* image 0 */
    move.l #0x0f0f0f0f,%d0
    movep.l %d0,0x0001(%a0)
    movep.l %d0,0x0129(%a0)
    movep.l %d0,0x0251(%a0)
    movep.l %d0,0x0379(%a0)
    movep.l %d0,0x04a1(%a0)
    movep.l %d0,0x05c9(%a0)
    movep.l %d0,0x06f1(%a0)
    movep.l %d0,0x0819(%a0)
    movep.l %d0,0x0941(%a0)
    movep.l %d0,0x0a69(%a0)
    movep.l %d0,0x0b91(%a0)
    movep.l %d0,0x0cb9(%a0)
    movep.l %d0,0x0de1(%a0)
    movep.l %d0,0x0f09(%a0)
    movep.l %d0,0x1031(%a0)
    movep.l %d0,0x1159(%a0)
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
    move.l #0x070f0f0f,%d0
    movep.l %d0,0x0001(%a0)
    movep.l %d0,0x0129(%a0)
    movep.l %d0,0x0251(%a0)
    movep.l %d0,0x0379(%a0)
    movep.l %d0,0x04a1(%a0)
    movep.l %d0,0x05c9(%a0)
    movep.l %d0,0x06f1(%a0)
    movep.l %d0,0x0819(%a0)
    movep.l %d0,0x0941(%a0)
    movep.l %d0,0x0a69(%a0)
    movep.l %d0,0x0b91(%a0)
    movep.l %d0,0x0cb9(%a0)
    movep.l %d0,0x0de1(%a0)
    movep.l %d0,0x0f09(%a0)
    movep.l %d0,0x1031(%a0)
    movep.l %d0,0x1159(%a0)
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
