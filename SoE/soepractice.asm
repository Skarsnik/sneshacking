
!MAP_ID = $ADB
!PREVIOUS_MAP_ID = $7FF200
!MAP_FRAME_COUNT = $7FF202
!INIT = $7FF204
!PREV_MAP_COUNTER = $7FF206
!REFRESH_HUD = $7FF208
!DEBUG_ADDR = $7FF250
!DOG_MAX_HP = $7E0A7F
!DOG_HP = $7E4F61

!HUD_WRAM_START = $7FF000
!HUD_WRAM_4_START = $F000
!HUD_VRAM_TOP = $0b4E

;8EAD
org $C08CAD
db $4C,"e_",$44,"og",$00

; this is just where the game clear the bg3 vram
org $CCAFE2
jsl stuff 

; This does a bunch of DMA and refresh the hud
;org $CF899C
;CF/899C:        A90200          lda #$0002 
;CF/899F:        0C290B          tsb $0B29
 ; JSL after_dma
;  NOP : NOP

  
;;  ; 	A		B	  Y      X        L      R      	>		<		v		^	Start	  select
  ;; DW #$0080, #$8000, #$4000, #$0040, #$0020, #$0010, #$0100, #$0200, #$0400, #$0800, #$1000, #$2000,
  
;; let do what we want
org $F10000

;8cafd0 lda #$c800             A:cf06 X:cf06 Y:0000 S:1fd7 D:0000 DB:8f nvmxdiZc V: 88 H: 488 F:36
;8cafd3 sta $26       [000026] A:c800 X:cf06 Y:0000 S:1fd7 D:0000 DB:8f Nvmxdizc V: 88 H: 506 F:36
;8cafd5 lda #$007f             A:c800 X:cf06 Y:0000 S:1fd7 D:0000 DB:8f Nvmxdizc V: 88 H: 574 F:36
;8cafd8 sta $28       [000028] A:007f X:cf06 Y:0000 S:1fd7 D:0000 DB:8f nvmxdizc V: 88 H: 592 F:36
;8cafda lda #$0800             A:007f X:cf06 Y:0000 S:1fd7 D:0000 DB:8f nvmxdizc V: 88 H: 620 F:36
;8cafdd sta $2e       [00002e] A:0800 X:cf06 Y:0000 S:1fd7 D:0000 DB:8f nvmxdizc V: 88 H: 638 F:36
;8cafdf ldx #$0700
stuff:
 php
 jsl $808650
 pha
 ;lda !REFRESH_HUD
 ;CMP #$0000 : BEQ .endstuff
 lda #$0000
 STA !REFRESH_HUD
 lda #!HUD_WRAM_4_START
 sta $26
 lda #!HUD_VRAM_TOP
 sta $2e
 ldx #$0008
 jsl $808650
 .endstuff
 pla
 plb
 rtl

 
start_practice_stuff:
  and $0104 ;
  sta $0104 ; what we replaced
  
  PHP
  PHX
  PHY
 
  lda $0104
  CMP #$3800 : BNE .noatlas
    JSR apply_atlas_inv
  .noatlas
  LDA #$0000
  CMP !INIT : BEQ .init
  JMP .normal  
  .init
    STZ !MAP_FRAME_COUNT
    LDA #$0001
	STA !INIT
  .normal
  LDA !MAP_ID
  CMP !PREVIOUS_MAP_ID : BEQ .endif
    ;LDA !MAP_FRAME_COUNT
	;STA !DOG_MAX_HP
	
	; These map id are title/first map id
	;STA !DEBUG_ADDR
	CMP.w #97 : BEQ .endif
	CMP.w #49 : BEQ .endif
	CMP.w #02  : BEQ .endif
	CMP.w #50 : BEQ .endif
	CMP.w #03  : BEQ .endif
	CMP.w #70 : BEQ .endif
	CMP #$0000 : BEQ .endif
	JSR update_hud
	LDA #$0001
	STA !REFRESH_HUD
	LDA #$0000
    STA !MAP_FRAME_COUNT
  .endif
  LDA !MAP_ID
  STA !PREVIOUS_MAP_ID
  LDA !MAP_FRAME_COUNT : INA : STA !MAP_FRAME_COUNT
  PLY
  PLX
  PLP
  lda $0104
  RTL



apply_atlas_inv:
	lda #64500
    sta $7E0A3F
    rts

!DMA_ARG_SIZE = $70
!DMA_ARG_ADDR = $72
!DMA_ARG_DEST = $74

  
update_hud:
  ;lda #$2BF0
  ;ldx #$0002
  ;sta !HUD_WRAM_START
  ;sta !HUD_WRAM_START, x
  
  ;Frame counter
  STA !DEBUG_ADDR
  
  ;Saving $05
  lda $05
  sta $72
  lda $06
  sta $74
  
  lda !MAP_FRAME_COUNT
  CMP.w #10000
  ;Probably need something better
  BMI .nooverflow
    lda.w #9999
  .nooverflow
 
  jsl $80870C ; this divise A and decompose it to $02, $03... until #FF
  ;STA !DEBUG_ADDR
  %a8()
  ldx #$0000
  ldy #$0000
  .loopclear
    lda #$00
    sta !HUD_WRAM_START, x
	INX
	CPX #$0008 : BNE .loopclear
  
  ldx #$0000
  .loopfindmax
    lda $02, x
    CMP #$FF : BEQ .endloopfm 
    INX
	INY
	INY
	JMP .loopfindmax
  .endloopfm
  sty $70
  lda #$08
  sbc $70
  tay
  ldx #$0000
  .loop
    lda #$FF
    CMP $02, x : BEQ .endloop
    lda #$EB
    adc $02, x
	stx $70 ; save x
	tyx
	sta !HUD_WRAM_START, x
	INX
	lda #$2B
	sta !HUD_WRAM_START, x
	INY
	INY
	ldx $70 ; restaure x
	INX
	JMP .loop
  .endloop
  lda $72
  sta $05
  lda $74
  sta $06
  %a16()
  
  STA !DEBUG_ADDR
  ldx #!HUD_WRAM_4_START
  stx !DMA_ARG_ADDR
  lda #$0008
  sta !DMA_ARG_SIZE
  lda #!HUD_VRAM_TOP
  sta !DMA_ARG_DEST
  jsr dma_vram
  rts
  
  ; This add to the dma table too (look cleaner)
; A and registers must be 8 bits
; X -> size
; Y -> vregister
; $28 -> bank
; $26 -> src addr
; $2E -> dest (vram)
; 
 
  lda #!HUD_WRAM_4_START
  sta $26
  lda #!HUD_VRAM_TOP
  sta $2E
  %a8()
  ldx #$0008
  ldy #$0080
  lda #$7F
  sta $28
  jsl $8086E3
  %ai16()
  rts


dma_vram:
  %a8()
  lda #$80
  sta $2115
  %a16()
  lda !DMA_ARG_DEST
  sta $2116
  lda !DMA_ARG_ADDR
  sta $4302
  lda !DMA_ARG_SIZE
  sta $4305
  %a8()
  lda #$7F
  sta $4304
  lda #$01
  sta $4300
  lda #$18
  sta $4301
  lda #$01
  sta $420B
  %a16()
  rts
  
  
  