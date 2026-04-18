    include hpdefs.x68    

    ORG    $984500
START:                  ; first instruction of program
    jsr ClearToWhite
LOOP:
    jsr ROM_GET_KEY
    cmp.w #$FFFF,D1
    beq LOOP
    move.w D1,-(SP)
    lsr #8,D1
    jsr PrintByte
    move.w (SP)+,D1
    jsr PrintByte
    bra LOOP

    include utilities.x68    

try:    
    dc.l 0

    END    START        ; last line of source







