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
    move.l #($600000+(592*376/2)), A0 ; 592*376/2 is divisible by 64
    move.l  #$600000, A1
ClearToWhite_1:
    movem.l D0-D7,-(A0) ; clear 16 display words at once, decrementing A0 by 32
    cmp.l A1,A0
    bne ClearToWhite_1
    
    movem.l (SP)+,D0-D7/A0-A1 ; restore all registers used
    rts

ClearToBlack:
    move.w #WRITE_BLACK,SCREEN_MEMORY_CONTROL
    bra ClearToWhite_0

PrintByte:
    move.l A1,-(SP)
    move.l D1,-(SP)
    move.l D2,-(SP)

    move.l D1,-(SP)
    
    lsr.b #4, D1
    jsr PrintNybble
    move.l (SP)+,D1

    and.b #$F, D1
    jsr PrintNybble
    
    move.l (SP)+,D2
    move.l (SP)+,D1
    move.l (SP)+,A1
    rts
    
PrintNybble: ; print D1 in hex
    and.b #$F,D1
    cmp.b #10,D1
    bcc PrintNybble_bigger
    add.b #'0',D1
PrintNybble_write:
    move.b D1,PrintNybble_message
    move.l PrintByte_y,D1
    move.l D1,-(SP)
    move.l PrintByte_x,D1
    move.l D1,-(SP)
    add.l #8,D1
    cmp.l #(576-8),D1
    bcc PrintNybble_newline
PrintNybble_set_coordinates:    
    move.l D1,PrintByte_x
    jsr ROM_SET_COORDINATES ; set coordinates
    addq.l #8, SP
    pea PrintNybble_message
    jsr ROM_DRAW_TEXT ; draw text
    addq.l #4, SP
    rts
PrintNybble_bigger:
    add.b #('A'-10),D1
    bra PrintNybble_write    
PrintNybble_newline:
    move.l PrintByte_y,D1
    add.l #12,D1    
    move.l D1,PrintByte_y
    move.l #0,D1
    bra PrintNybble_set_coordinates
PrintByte_x:  
    dc.l 0
PrintByte_y:  
    dc.l 1
    
PrintNybble_message:
    dc.b 'z',00    
    
WaitForSelect:
    jsr ROM_GET_KEY
    cmp.w #$2001,D1
    bne WaitForSelect
    rts


;    END START



*~Font name~Courier New~
*~Font size~10~
*~Tab type~1~
*~Tab size~4~
