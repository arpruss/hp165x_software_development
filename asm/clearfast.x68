    ORG    $984500
    include hpdefs.x68
    
START:                  
    jsr ClearToBlack
    jsr WaitForSelect
    jsr ClearToWhite
    jsr WaitForSelect
    jsr ROM_RELOAD

    include utilities.x68

    END    START        ; last line of source



*~Font name~Courier New~
*~Font size~10~
*~Tab type~1~
*~Tab size~4~
