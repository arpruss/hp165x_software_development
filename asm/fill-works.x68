*-----------------------------------------------------------
* Title      :
* Written by :
* Date       :
* Description:
*-----------------------------------------------------------
    ORG    $984500
START:                  ; first instruction of program
    move.l #$201000,A3
    
    move.l #$600000, A1
    move.l #(592*368), D1
    move.w #$000F, D2 ;; FF00 produces stripes
    move.w #$FF00,D3 ; draw black
    move.w d3,(A3)
loop0:
    move.b d2,(A1)
    add   #2,A1
    sub.l #1,D1
    bne loop0
    
    move.l #200000,D4
pause:
    sub.l #1,D4
    bne   pause    

    move.l #$600000, A1
    move.l #(592*(368)), D1
    move.w #$000A,D2 ;;  FF00 produces stripes 
    move.w #$0002,d5 ; 0 not work ;; 1 not work ;; 2 works
    move.w d5,(a3)
loop1:
    move.w d2,(A1)
    add   #2,A1
    sub.l #1,D1
    bne loop1    

    move.l #200000,D4
pause2:
    sub.l #1,D4
    bne   pause2    

END:
    bra END
;    reset
;    jmp $0

try:    
    dc.l 0

    END    START        ; last line of source

    





*~Font name~Courier New~
*~Font size~10~
*~Tab type~1~
*~Tab size~4~
