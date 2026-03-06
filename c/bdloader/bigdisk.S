	.text
	.globl _start
_start:	
	cmp.b #0x43,0x984152+2
	bne 1f
	move.w #(254*20*2-1),0x9842a6
	move.w #(254*20*2-1),0x9842a8
	move.b #0x41,0x984152+2
1:	
	jmp 0x8F42
