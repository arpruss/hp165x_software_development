	.text
	.globl _start
_start:	
	tst.b 0x984152+13
	beq 1f
	move.w #(254*20*2-1),0x9842a6
	move.w #(254*20*2-1),0x9842a8
	clr.b 0x984152+13
1:	
	jmp 0x8F42
