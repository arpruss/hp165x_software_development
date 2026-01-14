ROM_BEEP equ $ecd0
ROM_RELOAD equ $ece2
ROM_GET_KEY equ $eb38

    ORG    $984500
START:                  ; first instruction of program
    jsr ClearToWhite
LOOP:
    jsr ROM_GET_KEY
    cmp.w 0,D1
    beq LOOP
    move.w D1,-(SP)
    lsr #8,D1
    jsr PrintByte
    move.w (SP)+,D1
    jsr PrintByte
    bra LOOP

    include utilities.X68    

try:    
    dc.l 0

    END    START        ; last line of source







*~Font name~Courier New~
*~Font size~10~
*~Tab type~1~
*~Tab size~4~
