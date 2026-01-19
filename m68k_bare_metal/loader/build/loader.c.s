#NO_APP
	.file	"loader.c"
	.text
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"num names %d\n"
	.text
	.align	2
	.globl	getFiles
	.type	getFiles, @function
getFiles:
	lea (-32,%sp),%sp
	movem.l #8240,-(%sp)
	clr.l numNames
	pea 5.w
	clr.l -(%sp)
	jsr setTextXY
	move.l numNames,%d0
	addq.l #8,%sp
	moveq #15,%d1
	lea printf_,%a3
	cmp.l %d0,%d1
	jlt .L3
	moveq #0,%d2
	lea printf_,%a3
	lea getDirEntry,%a2
.L2:
	pea 12(%sp)
	move.l %d2,-(%sp)
	jsr (%a2)
	addq.l #8,%sp
	moveq #-1,%d1
	cmp.l %d0,%d1
	jeq .L11
.L6:
	cmp.w #-16383,%d0
	jeq .L4
	move.l numNames,%d0
	addq.l #1,%d2
	moveq #15,%d1
	cmp.l %d0,%d1
	jge .L2
.L3:
	move.l %d0,-(%sp)
	pea .LC0
	jsr (%a3)
	addq.l #8,%sp
	movem.l (%sp)+,#3076
	lea (32,%sp),%sp
	rts
.L4:
	pea 12(%sp)
	jsr (%a3)
	move.l numNames,%d0
	move.l %d0,%a0
	add.l %d0,%a0
	add.l %d0,%a0
	add.l %a0,%a0
	add.l %a0,%a0
	sub.l %d0,%a0
	add.l #names,%a0
	clr.b 10(%a0)
	move.b 16(%sp),(%a0)+
	move.b 17(%sp),(%a0)+
	move.b 18(%sp),(%a0)+
	move.b 19(%sp),(%a0)+
	move.b 20(%sp),(%a0)+
	move.b 21(%sp),(%a0)+
	move.b 22(%sp),(%a0)+
	move.b 23(%sp),(%a0)+
	move.b 24(%sp),(%a0)+
	move.b 25(%sp),(%a0)
	addq.l #1,%d0
	move.l %d0,numNames
	addq.l #1,%d2
	addq.l #4,%sp
	moveq #15,%d1
	cmp.l %d0,%d1
	jlt .L3
	pea 12(%sp)
	move.l %d2,-(%sp)
	jsr (%a2)
	addq.l #8,%sp
	moveq #-1,%d1
	cmp.l %d0,%d1
	jne .L6
.L11:
	move.l numNames,%d0
	move.l %d0,-(%sp)
	pea .LC0
	jsr (%a3)
	addq.l #8,%sp
	movem.l (%sp)+,#3076
	lea (32,%sp),%sp
	rts
	.size	getFiles, .-getFiles
	.section	.rodata.str1.1
.LC1:
	.string	"No disc in drive..."
.LC2:
	.string	"                   "
.LC3:
	.string	"Scanning..."
.LC4:
	.string	"No files found..."
.LC5:
	.string	"                 "
	.text
	.align	2
	.globl	scan
	.type	scan, @function
scan:
	movem.l #60,-(%sp)
	move.w #0,9963268
	lea setTextXY,%a3
	lea putText,%a2
	lea getFiles,%a4
	lea reload,%a5
	move.w 2158592,%d0
	btst #3,%d0
	jne .L25
.L13:
	clr.l -(%sp)
	clr.l -(%sp)
	jsr (%a3)
	pea .LC3
	jsr (%a2)
	jsr (%a4)
	lea (12,%sp),%sp
	tst.l numNames
	jne .L17
.L27:
	pea 1.w
	clr.l -(%sp)
	jsr (%a3)
	pea .LC4
	jsr (%a2)
	lea (12,%sp),%sp
	move.w 2158592,%d0
	btst #3,%d0
	jeq .L13
.L25:
	clr.l -(%sp)
	clr.l -(%sp)
	jsr (%a3)
	pea .LC1
	jsr (%a2)
	lea (12,%sp),%sp
.L15:
	move.w 2158592,%d0
	btst #3,%d0
	jeq .L26
.L16:
	move.w 9963268,%d0
	move.w #0,9963268
	cmp.w #260,%d0
	jne .L15
	jsr (%a5)
	move.w 2158592,%d0
	btst #3,%d0
	jne .L16
.L26:
	clr.l -(%sp)
	clr.l -(%sp)
	jsr (%a3)
	pea .LC2
	jsr (%a2)
	lea (12,%sp),%sp
	clr.l -(%sp)
	clr.l -(%sp)
	jsr (%a3)
	pea .LC3
	jsr (%a2)
	jsr (%a4)
	lea (12,%sp),%sp
	tst.l numNames
	jeq .L27
.L17:
	pea 1.w
	clr.l -(%sp)
	jsr (%a3)
	pea .LC5
	jsr (%a2)
	lea (12,%sp),%sp
	movem.l (%sp)+,#15360
	rts
	.size	scan, .-scan
	.section	.rodata.str1.1
.LC6:
	.string	"Choose program to execute:\n\n"
.LC7:
	.string	" [%X] %s\n"
	.text
	.align	2
	.globl	menu
	.type	menu, @function
menu:
	movem.l #8240,-(%sp)
	jsr drawBlack
	jsr fillScreen
	clr.l -(%sp)
	clr.l -(%sp)
	jsr setTextXY
	pea .LC6
	jsr putText
	lea (12,%sp),%sp
	tst.l numNames
	jle .L29
	lea names,%a2
	moveq #0,%d2
	lea printf_,%a3
.L30:
	move.l %a2,-(%sp)
	move.l %d2,-(%sp)
	pea .LC7
	jsr (%a3)
	addq.l #1,%d2
	lea (11,%a2),%a2
	lea (12,%sp),%sp
	cmp.l numNames.l,%d2
	jlt .L30
.L29:
	move.w 9963268,%d0
	move.w #0,9963268
	cmp.w #260,%d0
	jeq .L28
	lea parseKey,%a2
.L32:
	move.w 2158592,%d1
	btst #3,%d1
	jne .L28
	move.w %d0,-(%sp)
	clr.w -(%sp)
	jsr (%a2)
	ext.w %d0
	move.w %d0,%a0
	lea (-48,%a0),%a1
	addq.l #4,%sp
	moveq #9,%d0
	cmp.l %a1,%d0
	jcc .L44
	lea (-65,%a0),%a1
	moveq #5,%d0
	cmp.l %a1,%d0
	jcc .L45
.L34:
	move.w 9963268,%d0
	move.w #0,9963268
	cmp.w #260,%d0
	jne .L32
.L28:
	movem.l (%sp)+,#3076
	rts
.L45:
	lea (-55,%a0),%a0
	cmp.l numNames.l,%a0
	jge .L34
	move.l %a0,%d0
	add.l %a0,%d0
	add.l %a0,%d0
	add.l %d0,%d0
	add.l %d0,%d0
	sub.l %a0,%d0
	add.l #names,%d0
	move.l %d0,-(%sp)
	jsr loadAndRun
	addq.l #4,%sp
	move.w 9963268,%d0
	move.w #0,9963268
	cmp.w #260,%d0
	jne .L32
	jra .L28
.L44:
	cmp.l numNames.l,%a1
	jge .L34
	move.l %a1,%d0
	add.l %a1,%d0
	add.l %a1,%d0
	add.l %d0,%d0
	add.l %d0,%d0
	sub.l %a1,%d0
	add.l #names,%d0
	move.l %d0,-(%sp)
	jsr loadAndRun
	addq.l #4,%sp
	move.w 9963268,%d0
	move.w #0,9963268
	cmp.w #260,%d0
	jne .L32
	jra .L28
	.size	menu, .-menu
	.section	.text.startup,"ax",@progbits
	.align	2
	.globl	main
	.type	main, @function
main:
	movem.l #60,-(%sp)
	clr.l -(%sp)
	jsr setTextBlackOnWhite
	addq.l #4,%sp
	lea drawBlack,%a5
	lea fillScreen,%a4
	lea scan,%a3
	lea menu,%a2
.L47:
	jsr (%a5)
	jsr (%a4)
	jsr (%a3)
	jsr (%a2)
	jsr (%a5)
	jsr (%a4)
	jsr (%a3)
	jsr (%a2)
	jra .L47
	.size	main, .-main
	.globl	mode
	.section	.bss
	.align	2
	.type	mode, @object
	.size	mode, 4
mode:
	.zero	4
	.globl	numNames
	.align	2
	.type	numNames, @object
	.size	numNames, 4
numNames:
	.zero	4
	.globl	names
	.type	names, @object
	.size	names, 176
names:
	.zero	176
	.ident	"GCC: (Tranaptic-2023/06/16-13:17:25) 13.1.0"
