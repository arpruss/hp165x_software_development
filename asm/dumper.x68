*-----------------------------------------------------------
* Title      :
* Written by :
* Date       :
* Description:
*-----------------------------------------------------------
    ORG    $984500
    
    include hpdefs.x68
    
START:                  ; first instruction of program
     jsr ClearToBlack
     jsr ClearToWhite
     move.w #WRITE_BLACK,SCREEN_MEMORY_CONTROL
     bsr ResetBold

TOP:  
     clr.w LAST_KEY
     move.l #0, Print_x
     move.l #0, Print_y
     move.l position, D1
     bsr PrintDWord
     move.b #$A, D1
     bsr PrintChar
  
     move.l position, a1
     move.l #29-1,D3
loop:
     bsr Dump32
     dbra D3,loop

     cmp.w #$0104,LAST_KEY ; stop
     beq DONE
     cmp.w #$1040,LAST_KEY ; updown
     beq up
     cmp.w #$2040,LAST_KEY ; leftright
     beq down
     bra TOP

up:
     sub.l #(29*32),position     
     bra TOP

down:
     add.l #(29*32),position     
     bra TOP

DONE:     
     jsr ROM_RELOAD
  
Dump32:
    move A1,D1
    bsr PrintWord
    move.b #32,D1
    bsr PrintChar
    move.l #31,D2
Dump32_1:
    move.b (A1)+,D1
    bsr PrintByte
    cmp #16,D2
    bne not16
    move.b #32,D1
    bsr PrintChar
not16:    
    dbra D2,Dump32_1
    
    move.b #$A,D1
    bsr PrintChar
    rts
    
position: dc.l $980000    
    
    include utilities.x68
stackpointer dc.l 0
    END    START        ; last line of source









*~Font name~Courier New~
*~Font size~10~
*~Tab type~1~
*~Tab size~4~
