*-----------------------------------------------------------
* Title      :
* Written by :
* Date       :
* Description:
*-----------------------------------------------------------
    ORG    $984500
START:                  ; first instruction of program
    move.l #$201000,A3

bigloop:
    move.w #$3CF0,(A3)

    move.l #$600000, d1
    clr.l d2
loop0:
    movea.l d1,a1
    add.l d2,a1
    move.b #$00,(a1)
    add.l #$128,d1
    add.l #1,d2
    cmp.l #368,d2
    bne loop0

    move.w #$01FF,(A3)

    move.l #$600010, d1
    clr.l d2
loop1:
    movea.l d1,a1
    add.l d2,a1
    move.b #$00,(a1)
    add.l #$128,d1
    add.l #1,d2
    cmp.l #368,d2
    bne loop1

    move.w #$FEFF,(A3) ; 0EFF doesn't work

    move.l #$600020, d1
    clr.l d2
loop2:
    movea.l d1,a1
    add.l d2,a1
    move.b #$00,(a1)
    add.l #$128,d1
    add.l #1,d2
    cmp.l #368,d2
    bne loop2

    move.w #$FF00,(A3) ; F000 works, 2000 works

    move.l #$600030, d1
    clr.l d2
loop3:
    movea.l d1,a1
    add.l d2,a1
    move.b #$11,(a1)
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
