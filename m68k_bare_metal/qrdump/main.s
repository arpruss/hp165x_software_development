#NO_APP
	.file	"main.c"
	.text
	.align	2
	.globl	main
	.type	main, @function
main:
	move.l #6291456,%a0
	move.w #17,(%a0)
	move.l #2101248,%a1
	move.w #19,(%a1)
	move.w #-256,(%a1)
.L2:
	move.w #15,(%a0)+
	cmp.l #6514048,%a0
	jne .L2
.L3:
	jra .L3
	.size	main, .-main
	.ident	"GCC: (Tranaptic-2023/06/16-13:17:25) 13.1.0"
