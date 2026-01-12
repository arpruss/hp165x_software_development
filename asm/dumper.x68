*-----------------------------------------------------------
* Title      :
* Written by :
* Date       :
* Description:
*-----------------------------------------------------------
    ORG    $984500
    
    include hpdefs.x68
    
START:                  ; first instruction of program

    jsr ClearToWhite

    move.l #($12f44), a1
    move.l #800,d2

LOOP:
    move.b (a1),d1
    jsr PrintByte
    add #1,a1
    sub #1,d2
    bne LOOP
    
    jsr WaitForSelect
    jsr ROM_RELOAD
    
    include utilities.x68
    
    END    START        ; last line of source





*~Font name~Courier New~
*~Font size~10~
*~Tab type~1~
*~Tab size~4~
