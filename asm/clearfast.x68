    ORG    $984500
    include hpdefs.x68
    
START:                  
    jsr ClearToBlack
    move #0,D0
    jsr PrintDWord
    jsr WaitForSelect
    move.w #$0004,SCREEN_MEMORY_CONTROL
    jsr     ClearToWhite_0
    
    jsr WaitForSelect
    jsr ROM_RELOAD

    include utilities.x68

    END    START        ; last line of source




*~Font name~Courier New~
*~Font size~10~
*~Tab type~1~
*~Tab size~4~
