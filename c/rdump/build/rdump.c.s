#NO_APP
	.file	"rdump.c"
	.text
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"%08x %08x %08x %08x"
	.section	.text.startup,"ax",@progbits
	.align	2
	.globl	main
	.type	main, @function
main:
	lea (-80,%sp),%sp
	movem.l #60,-(%sp)
	clr.l -(%sp)
	jsr setTextBlackOnWhite
	jsr drawBlack
	jsr fillScreen
	addq.l #4,%sp
	lea (16,%sp),%a2
	lea sprintf_,%a5
	lea setTextXY,%a4
	lea putText,%a3
.L2:
	move.l 9963276,-(%sp)
	move.l 9963272,-(%sp)
	move.l 9963268,-(%sp)
	move.l 9963264,-(%sp)
	pea .LC0
	move.l %a2,-(%sp)
	jsr (%a5)
	clr.l -(%sp)
	clr.l -(%sp)
	jsr (%a4)
	lea (28,%sp),%sp
	move.l %a2,(%sp)
	jsr (%a3)
	addq.l #4,%sp
	move.l 9963276,-(%sp)
	move.l 9963272,-(%sp)
	move.l 9963268,-(%sp)
	move.l 9963264,-(%sp)
	pea .LC0
	move.l %a2,-(%sp)
	jsr (%a5)
	clr.l -(%sp)
	clr.l -(%sp)
	jsr (%a4)
	lea (28,%sp),%sp
	move.l %a2,(%sp)
	jsr (%a3)
	addq.l #4,%sp
	jra .L2
	.size	main, .-main
	.ident	"GCC: (Tranaptic-2023/06/16-13:17:25) 13.1.0"
