;    ORG    $984500
;START:                  ; first instruction of program

    include romdefs.x68
    
ClearToWhite:
    move.l #$600000, A1
    move.l #(592*376/4), D1
    move.w #$000F,D2 
    move.l #$201000,A3
    move.w #$0002,d5
    move.w #$0002,(a3)
ClearToWhite_1:
    move.w d2,(A1)
    add   #2,A1
    sub.l #1,D1
    bne ClearToWhite_1  
    rts

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
    jsr $0000eae4 ; set coordinates
    addq.l #8, SP
    pea PrintNybble_message
    jsr $0000eaf6 ; draw text
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
