     include hpdefs.x68

;; ORG $A09710 (for insertion between end of SYSTEM_ code and beginning of SYSTEM_ data)

;; this code uses RAM in the $983000-983FFF area that appears to be unused
;; it relocates the stack to 983FFE because the call to ebb0 seems to do 
;; poorly with the standard stack position, probably because the ReadDiskHeader
;; call (423c) allocates 256 bytes on the stack

    ORG    $A09710 ;; 984500

buffer equ $983000
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
    
    move.l  SP,savedStack
    move.l  #$983FFE,SP
    
    movem.l D0-D7/A0-A6, -(SP)

    move.w  $20F000,D0
    move.w  D0,D2
    and.w   #8,D2    
    bne     patchError ; no disk
    and.w   #1,D0
    
    bne     runPatch   ; disk hasn't been changed
    
    bsr     ShortBeep
    jsr     $ebb0
    tst.w   D0
    bmi     patchError
    
runPatch:    
    bsr     ShortBeep
    bsr     getFilename
    bsr     screendump
    bsr     ShortBeep

cleanupPatch:    
    clr.w   triggered
    movem.l (SP)+, D0-D7/A0-A6
    move.w  #$FFFF,D1

    move.l  savedStack, SP

    rts    
patchError:
    bsr     LongBeep
    bra     cleanupPatch
    
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

    add.l   #12,SP    

    move.l  (SP)+,D7  ; restore
    
    cmp.l   #(pbmHeaderEnd-pbmHeader),D0
    bne     closeWithError

    
    move.l #SCREEN, A0 ; current position
    move.l #(SCREEN+(SCREEN_WIDTH/4)*2*SCREEN_HEIGHT),A1 ; end of screen
    move.l #SCREEN_MEMORY_CONTROL, A2
    
copyToBuffer:
    move.l #buffer, A4
    move.w #buffer_size-1, D4

copyLoop:
    ;;  Display = (Attr^(OV&OData | ~OV&Data)) & ~(OData&~OV)
READ_DATA  equ $f-$1
READ_ODATA equ $f-$2
READ_OV    equ $f-$4
READ_ATTR  equ $f-$8

READ MACRO ; MODE, DEST, TEMP
    move.w \1,(A2)     
    move.w (A0),\2
    and.w  #$F,\2
    lsl.w  #4,\2
    move.w (2,A0),\3
    and.w  #$F,\3
    or.w   \3,\2
    ENDM
    
READ_LAST MACRO ; MODE, DEST, TEMP
    move.w \1,(A2)     
    move.w (A0)+,\2
    and.w  #$F,\2
    lsl.w  #4,\2
    move.w (A0)+,\3
    and.w  #$F,\3
    or.w   \3,\2
    ENDM
    
    READ   #READ_DATA,  D0, D6
    READ   #READ_ODATA, D1, D6
    READ   #READ_OV,    D2, D6
    READ_LAST   #READ_ATTR,  D3, D6

; output = (D3^(D2&D1 | ~D2&D0)) & (~D1|D2)
; use S5 as temporary
    move.w D2,D5
    not.w  D5       ; current D5 = ~D2
    and.w  D5,D0    ; current D0 = ~D2&D0
    move.w D1,D5    
    and.w  D2,D5    ; current D5 = D2&D1
    or.w   D0,D5    ; current D5 = (D2&D1 | ~D2&D0)
    eor.w  D3,D5    ; current D5 = D3^(D2&D1 | ~D2&D0)
    not.w  D1       ; current D1 = ~D1
    or.w   D1,D2    ; current D2 = ~D1|D2
    and.w  D5,D2    ; current D2 = output
    
    not.w  D2       ; for PBM purposes
    move.b D2,(A4)+
    dbra   D4,copyLoop
    
    movem.l D0-D7/A0-A6, -(SP)
    
    pea     (buffer_size)
    pea     (buffer)
    move.l  D7, -(SP)
    
    jsr     ROM_WRITE_FILE
    
    add.l   #12,SP

    cmp.l   #buffer_size,D0

    movem.l (SP)+, D0-D7/A0-A6
    
    bne     closeWithError    

    cmp.l   A1,A0
    bcs     copyToBuffer
    
closeFile:    
    move.w  $98077e,D0
    lsl.w   #8,D0
    move.b  $98077d,D0
    move.w  D0,SCREEN_MEMORY_CONTROL

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
    move.l  #(3*8854/2),D0
    moveq.l #1,D1
SB:
    sub.l   D1,D0
    bne     SB
    move.b #BUZZER_OFF,BUZZER
    rts
    
LongBeep:
    move.b #BUZZER_ON,BUZZER
    move.l  #(40*8854),D0
    moveq.l #1,D1
LB:
    sub.l   D1,D0
    bne     LB
    move.b #BUZZER_OFF,BUZZER
    rts

triggered:
    dc.w 00
    
statePosition:
    dc.w 00
    
savedStack:
    dc.l 0    

pbmHeader:
    dc.b 'P4',$0A,'592 384',$0A
pbmHeaderEnd:    

filename:
    dc.b 'SCRNSH.PBM',0
    
;    include utilities.x68       

    org  (*+1)&-2
;buffer:
 
    
    END    START        ; last line of source

    


    






*~Font name~Courier New~
*~Font size~10~
*~Tab type~1~
*~Tab size~4~
