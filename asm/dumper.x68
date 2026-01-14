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
     clr.w LAST_KEY

TOP:  
     move.l #0, Print_x
     move.l #0, Print_y
     move.l position, D1
     bsr PrintDWord

     move.l #400, Print_x
     move.l new_position, D1
     bsr PrintDWord

     move.b #$A, D1
     bsr PrintChar
  
     move.l position, A1
     move.l #29-1,D3
loop:
     bsr Dump32
     dbra D3,loop

key_loop:
     move.w LAST_KEY,D0
     clr.w LAST_KEY
     cmp.w #$0104,D0 ; stop
     beq DONE
     cmp.w #$1040,D0 ; updown
     beq up
     cmp.w #$2040,D0 ; leftright
     beq down
     cmp.w #$2001,D0
     beq select
     cmp.w #$0102,D0
     beq run
     cmp.w #$0240,D0
     beq key0
     cmp.w #$0220,D0
     beq key1
     cmp.w #$0420,D0
     beq key2
     cmp.w #$0820,D0
     beq key3
     cmp.w #$0210,D0
     beq key4
     cmp.w #$0410,D0
     beq key5
     cmp.w #$0810,D0
     beq key6
     cmp.w #$0208,D0
     beq key7
     cmp.w #$0408,D0
     beq key8
     cmp.w #$0808,D0
     beq key9
     cmp.w #$0204,D0
     beq keyA
     cmp.w #$0404,D0
     beq keyB
     cmp.w #$0804,D0
     beq keyC
     cmp.w #$0202,D0
     beq keyD
     cmp.w #$0402,D0
     beq keyE
     cmp.w #$0802,D0
     beq keyF
resume:
     move.b continuous,D0
     cmp.b #0,D0
     beq key_loop
     bra TOP
up:
     sub.l #(29*32),position     
     bra TOP

down:
     add.l #(29*32),position     
     bra TOP
select:
     move.l new_position,position
     move.b #1,continuous
     bra TOP
run:
     move.b continuous,D0
     eori.b #1,D0
     move.b D0,continuous
     bra resume
     
key0:
     move.l #0,D0
     bra update_position
key1:
     move.l #1,D0
     bra update_position
key2:
     move.l #2,D0
     bra update_position
key3:
     move.l #3,D0
     bra update_position
key4:
     move.l #4,D0
     bra update_position
key5:
     move.l #5,D0
     bra update_position
key6:
     move.l #6,D0
     bra update_position
key7:
     move.l #7,D0
     bra update_position
key8:
     move.l #8,D0
     bra update_position
key9:
     move.l #9,D0
     bra update_position
keyA:
     move.l #$A,D0
     bra update_position
keyB:
     move.l #$B,D0
     bra update_position
keyC:
     move.l #$C,D0
     bra update_position
keyD:
     move.l #$D,D0
     bra update_position
keyE:
     move.l #$E,D0
     bra update_position
keyF:
     move.l #$F,D0
update_position:
     move.b #0,continuous
     move.l new_position,D1
     lsl.l #4,D1
     add.l D0,D1
     move.l D1,new_position
     move.l #400, Print_x
     move.l #00, Print_y
     move.l new_position, D1
     bsr PrintDWord
     bra key_loop

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
    
position: dc.l $0000
new_position: dc.l $0
continuous: dc.b $1
            dc.b 00 ; align

    
    include utilities.x68
    END    START        ; last line of source











*~Font name~Courier New~
*~Font size~10~
*~Tab type~1~
*~Tab size~4~
