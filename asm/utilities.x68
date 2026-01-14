;    ORG    $984500
;START:                  ; first instruction of program

    include hpdefs.x68
    
ClearToWhite:
    move.w #WRITE_WHITE,SCREEN_MEMORY_CONTROL
ClearToWhite_0:
    movem.l D0-D7/A0-A1,-(SP) ; save all registers used
    move.l #$000F000F, D0
    move.l D0,D1
    move.l D0,D2
    move.l D0,D3
    move.l D0,D4
    move.l D0,D5
    move.l D0,D6
    move.l D0,D7
    move.l #(SCREEN+(592*384/2)), A0 ; 592*384/2 is divisible by 64
    move.l  #SCREEN, A1
ClearToWhite_1:
    movem.l D0-D7,-(A0) ; clear 16 display words at once, decrementing A0 by 32
    cmp.l A1,A0
    bge ClearToWhite_1
    
    movem.l (SP)+,D0-D7/A0-A1 ; restore all registers used
    rts

ClearToBlack:
    move.w #WRITE_BLACK,SCREEN_MEMORY_CONTROL
    bra ClearToWhite_0
    
PrintDWord: ; print dword in D1
    movem.l D0-D1,-(SP)
    move.l D1,D0
    lsr.l #8,D1
    lsr.l #8,D1
    lsr.l #8,D1
    bsr PrintByte
    move.l D0,D1
    lsr.l #8,D1
    lsr.l #8,D1
    bsr PrintByte
    move.l D0,D1
    lsr.l #8,D1
    bsr PrintByte
    move.l D0,D1
    bsr PrintByte
    movem.l (SP)+,D0-D1
    rts

PrintWord: ; print word in D1
    move.l D0,-(SP)
    move.w D1,D0
    lsr.w #8,D1
    bsr PrintByte
    move.w D0,D1
    bsr PrintByte
    move.l (SP)+,D0
    rts

PrintByte:
    movem.l D0-D2/A1,-(SP)

    move.l D1,-(SP)
    
    lsr.b #4, D1
    jsr PrintNybble
    move.l (SP)+,D1

    and.b #$F, D1
    jsr PrintNybble
    
    movem.l (SP)+,D0-D2/A1
    rts
    
PrintChar: ; print character in D1
    movem.l D0-D2,-(SP)    
PrintChar_write:
    cmp.b #$0A,D1
    beq PrintChar_newline
    move.b D1,Print_message
    
    move.l Print_y,D1
    move.l D1,-(SP)
    move.l Print_x,D1
    move.l D1,-(SP)
    jsr ROM_SET_COORDINATES ; set coordinates
    addq.l #8, SP

    pea Print_message
    jsr ROM_DRAW_TEXT ; draw text
    addq.l #4, SP
    
    move.l Print_x,D1
    addq.l #8,D1
    cmp.l #(576-8),D1
    bcc PrintChar_newline
PrintChar_1:
    move.l D1,Print_x
PrintChar_exit:    
    movem.l (SP)+,D0-D2
    rts

PrintChar_newline:
    move.l Print_y,D1
    add.l #12,D1    
    move.l D1,Print_y
    clr.l D1
    bra PrintChar_1

PrintNybble: ; print D1 in hex ; clobbers D1
    and.b #$F,D1
    cmp.b #10,D1
    bcc PrintNybble_bigger
    add.b #'0',D1
    bra PrintChar
PrintNybble_bigger:
    add.b #('A'-10),D1
    bra PrintChar

Print_x:  
    dc.l 0
Print_y:  
    dc.l 0
    
Print_message:
    dc.b 'z',00    
    
WaitForSelect:
    jsr ROM_GET_KEY
    cmp.w #$2001,D1
    bne WaitForSelect
    rts

SetBold:
    pea 1
    jsr ROM_SET_BOLD
    add.l #4, SP
    rts

ResetBold:
    clr.l -(SP)
    jsr ROM_SET_BOLD
    add.l #4, SP
    rts
    
Beep:
    move.b #BUZZER_ON,BUZZER
    move.l #16,D0
    move.l #$1C,D7
    trap #0
    move.b #BUZZER_OFF,BUZZER
    rts

;    END START

     





*~Font name~Courier New~
*~Font size~10~
*~Tab type~1~
*~Tab size~4~
