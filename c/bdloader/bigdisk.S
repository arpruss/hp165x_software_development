	.text
	.globl _start
_start:	
	cmp.b #'B',0x984152+12
	bne 4f
	move.w #(254*20*2-1),0x9842a6
	move.w #(254*20*2-1),0x9842a8
	move.b #'b',0x984152+12
4:	
	jmp 0x8F42
