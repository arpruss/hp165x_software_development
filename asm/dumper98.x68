*-----------------------------------------------------------
* Title      :
* Written by :
* Date       :
* Description:
*-----------------------------------------------------------
    ORG    $984500
START:                  ; first instruction of program

    move.l #$201000, a1
    move.l #saved, a2
    move.l (a1),d1
    move.l d1,(a2)
    
    move.l #saved, a1
    move.l #4,d2

LOOP:
    move.b (a1),d1
    jsr print_byte
    add #1,a1
    sub #1,d2
    bne LOOP
EEEE:
    bra EEEE        
    
saved:
    dc.l $12345678

print_byte:
    move.l A1,-(SP)
    move.l D1,-(SP)
    move.l D2,-(SP)

    move.l D1,-(SP)
    
    lsr.b #4, D1
    jsr print_nybble
    move.l (SP)+,D1

    and.b #$F, D1
    jsr print_nybble
    
    move.l (SP)+,D2
    move.l (SP)+,D1
    move.l (SP)+,A1
    rts
    
print_nybble: ; print D1 in hex
    and.b #$F,D1
    cmp.b #10,D1
    bcc bigger_than_9
    add.b #'0',D1
write:
    move.b D1,nybble_message
    pea (40).w
    move.l x,D1
    move.l D1,-(SP)
    add.l #8,D1
    move.l D1,x
    jsr $0000eae4 ; set coordinates
    addq.l #8, SP
    pea nybble_message
    jsr $0000eaf6 ; draw text
    addq.l #4, SP
    rts
bigger_than_9:
    add.b #('A'-10),D1
    bra write    
    
x:  
    dc.l 2
y:
    dc.l 16
    
nybble_message:
    dc.b 'z',00    

EEE:
    bra EEE    
    
hello_world:
    dc.b 'Hello World!',00    

;        pea        ($1).w
;        pea        ($1e).w
;        pea        ($7a).w
;        pea        ($66).w
;        pea        ($173).w
;        jsr $00992fc0
;        lea        ($18 ,SP ),SP    
;    move.w     #$fe ,-(SP )
;    jsr        $0000eaae
;    addq.l     #2 ,SP

    move.w     #$4 ,-(SP )
    jsr        $0000eaba
    addq.l     #2 ,SP

    move.w #$FFFF,d1
    move.w d1,$700000
    move.w d1,$740000

    move.l #$600000, d1
    clr.l d2
loop0:
    movea.l d1,a1
    add.l d2,a1
    move.b #$07,(a1)
    add.l #$128,d1
    add.l #1,d2
    cmp.l #368,d2
    bne loop0

    move.w #$0000,d1
    move.b d1,$700000
    move.w #$0000,d1
    move.b d1,$740000

    move.l #$600010, d1
    clr.l d2
loop1:
    movea.l d1,a1
    add.l d2,a1
    move.b #$07,(a1)
    add.l #$128,d1
    add.l #1,d2
    cmp.l #368,d2
    bne loop1

    move.w #$FFFF,d1
    move.b d1,$700000
    move.w #$0000,d1
    move.b d1,$740000

    move.l #$600020, d1
    clr.l d2
loop2:
    movea.l d1,a1
    add.l d2,a1
    move.b #$07,(a1)
    add.l #$128,d1
    add.l #1,d2
    cmp.l #368,d2
    bne loop2

    move.w #$0000,d1
    move.b d1,$700000
    move.w #$FFFF,d1
    move.b d1,$740000

    move.l #$600030, d1
    clr.l d2
loop3:
    movea.l d1,a1
    add.l d2,a1
    move.b #$07,(a1)
    add.l #$128,d1
    add.l #1,d2
    cmp.l #368,d2
    bne loop3

END:
    bra END

try:    
    dc.l 0

    END    START        ; last line of source





*~Font name~Courier New~
*~Font size~10~
*~Tab type~1~
*~Tab size~4~
