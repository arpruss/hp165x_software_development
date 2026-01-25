     include hpdefs.x68

;; ORG $A09710 (for insertion between end of SYSTEM_ code and beginning of SYSTEM_ data)

    ORG    $A09710 ;; 984500

buffer_size equ $200 ; divides into screen byte size
file_type equ $C999
START:                  ; first instruction of program
    jsr     ROM_GET_KEY
    tst.w   triggered
    beq     maybePatch
return:    
    rts
maybePatch:
    tst.w   statePosition
    bne     notState0
    cmp.w   #$110,D1 ; don't care
    bne     return
increment:    
    add.w   #1,statePosition
    bra     return
notState0:    
    cmp.w   #1,statePosition
    beq     state1
    cmp.w   #2,statePosition
    beq     state2
    cmp.w   #$120,D1 ; clear
    beq     patch
clearState:
    clr.w   statePosition
    rts
state1:
    cmp.w   #$120,D1 ; clear
    bne     clearState
    bra     increment
state2:
    cmp.w   #$110,D1 ; don't care
    bne     clearState
    bra     increment 
patch:
    clr.w   statePosition
    move.w  #1,triggered
    
    movem.l D0-D7/A0-A6, -(SP)
    move.w  $20F000,D0
    move.w  D0,D2
    and.w   #8,D2    
    bne     skip ; no disk
    and.w   #1,D0
    bne     runPatch ; disk hasn't been changed

;    bra     skip ; don't know how to refresh disk   
;    jsr     $ec04
;    move.w  D0,D3
;    jsr     $ec10
;    tst.w   D3
;    beq     skip

;    pea     (0)
;    jsr     $eb62
;    add     #4,SP
;    jsr     $ebb0
;    tst.w   D0
;    bne     skip
    
runPatch:    
    bsr     getFilename
    bsr     ShortBeep
    bsr     screendump
    bsr     ShortBeep
skip:    
    clr.w   triggered
    movem.l (SP)+, D0-D7/A0-A6
    move.w  #$FFFF,D1
    rts    
    
clearTriggered:
    clr.w   triggered
nopatch:
    rts
    
screendump:
    pea.l   (2)
    pea.l   (file_type)
    pea.l   filename
    jsr     ROM_OPEN_FILE
    add.l   #12,SP
    tst.w   D0
    bmi     quickExitError

    move.l  D0,D7 ; file number
    
    move.l  D7, -(SP)   ; save 
    
    pea     (pbmHeaderEnd-pbmHeader)
    pea     (pbmHeader)
    move.l  D7, -(SP)
    
    jsr     ROM_WRITE_FILE

;    movem.l D0-D7/A0-A6, -(SP)
;    move.l  D0,D1
;    bsr     PrintWord
;    pea     filename
;    jsr     ROM_DRAW_TEXT
;    add     #4,SP
;    movem.l (SP)+,D0-D7/A0-A6
    
    add.l   #12,SP    

    move.l  (SP)+,D7  ; restore
    
;    cmp.l   #(pbmHeaderEnd-pbmHeader),D0
;    beq     closeWithError
    
    move.l #SCREEN, A0 ; current position
    move.l #(SCREEN+(SCREEN_WIDTH/4)*2*SCREEN_HEIGHT),A1 ; end of screen
    move.w #$e,D2 ; read data
    move.w #$7,D3 ; read attr
    move.l #SCREEN_MEMORY_CONTROL, A2
    
copyToBuffer:
    move.l #buffer, A4
    move.w #buffer_size-1, D4
copyLoop:
    move.w D2,(A2) ; read data
    move.w (A0),D0 ; get data
    move.w D3,(A2) ; read attr
    move.w (A0)+,D6
    eor.w  D6,D0
    and.w  #$f,D0
    lsl.w  #4,D0
    move.w D2,(A2) ; read data
    move.w (A0),D5 ; get data
    move.w D3,(A2) ; read attr
    move.w (A0)+,D6
    eor.w  D6,D5   
    and.w  #$f,D5
    or.w   D5,D0   ; D0 = data xor attr (ignoring the overlay planes)
    not.w  D0
    
    move.b D0,(A4)+
    dbra   D4,copyLoop
    
    movem.l D0-D7/A0-A6, -(SP)
    
    pea     (buffer_size)
    pea     (buffer)
    move.l  D7, -(SP)
    
    jsr     ROM_WRITE_FILE
    
    add.l   #12,SP

    movem.l (SP)+, D0-D7/A0-A6
    
    cmp.l   A1,A0
    bcs     copyToBuffer
    
    move.w  $98077e,D0
    lsl.w   #8,D0
    move.b  $98077d,D0
    move.w  D0,SCREEN_MEMORY_CONTROL

closeFile:    
    move.l  D7, -(SP)    
    jsr     ROM_CLOSE_FILE    
    add.l   #4,SP

quickExit:    
    rts    
quickExitError:
    bsr     LongBeep
    rts
closeWithError:
    bsr     LongBeep
    bra     closeFile

getFilename:    
    moveq.l #0,D2  ; biggest number
    moveq.l #0,D1  ; index
dirLoop:    
    move.l  D2,-(SP)
    move.l  D1,-(SP)

    pea     buffer ; direntry
    move.l  D1,-(SP)
    jsr     $ebce ; get dir entry
    add.l   #8,SP
    
    move.l  (SP)+,D1
    move.l  (SP)+,D2

    cmp.l   #-1,D0 ;; cmp.l ???? TODO
    beq     dirDone
    addq.l  #1,D1

    cmp.w   #file_type,D0
    bne     dirLoop
    
    move.l  buffer, D7
    cmp.l   #'SCRN', D7
    bne     dirLoop
    move.l  buffer+6, D7
    cmp.l   #'.PBM', D7
    bne     dirLoop
    
    move.l  #(buffer+4),A0
    bsr     parseNumber2
    and.l   #$FFFF,D0

    cmp.l   D0,D2
    bcc     dirLoop
    move.w  D0,D2
    bra     dirLoop    
dirDone:
    move.l  D2,-(SP)
    bsr     ShortBeep
    move.l  (SP)+,D2
    move.l  D2,D0
    addq.l  #1,D0 ; new number

    move.l  #(filename+4),A0
    bra     formatNumber2
    
parseNumber2: ; parse 2-digit number at A0 into D0.w, clobbering D7
    moveq.l #0,D7
    move.b  (A0)+,D0
    sub.w   #'0',D0
    and.l   #$ff,D0
    mulu.w  #10,D0
    move.b  (A0),D7
    add.w   D7,D0
    sub.w   #'0',D0
    rts

formatNumber2: ; format D0.w as a 2-digit number at A0
    divu.w  #10,D0
    add.w   #'0',D0
    move.b  D0,(A0)+
    lsr.l   #8,D0
    lsr.l   #8,D0
    add.w   #'0',D0
    move.b  D0,(A0)
    rts
    
ShortBeep:
    move.b #BUZZER_ON,BUZZER
    move.l #2,D0
    move.l #$1C,D7
    trap #0
    move.b #BUZZER_OFF,BUZZER
    rts

LongBeep:
    move.b #BUZZER_ON,BUZZER
    move.l #55,D0
    move.l #$1C,D7
    trap #0
    move.b #BUZZER_OFF,BUZZER
    rts

triggered:
    dc.w 00
    
statePosition:
    dc.w 00
    
pbmHeader:
    dc.b 'P4',$0A,'592 384',$0A
pbmHeaderEnd:    

filename:
    dc.b 'SCRNSH.PBM',0


filenameBad:
    dc.b 'NONEXIST',0
    include utilities.x68       

    org  (*+1)&-2
buffer:
 
    
    END    START        ; last line of source

    


    


*~Font name~Courier New~
*~Font size~10~
*~Tab type~1~
*~Tab size~4~
