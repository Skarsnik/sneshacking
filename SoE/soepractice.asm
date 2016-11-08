
!MAP_ID = $ADB
!PREVIOUS_MAP_ID = $7FF200
!MAP_FRAME_COUNT = $7FF202
!INIT = $7FF204
!PREV_MAP_COUNTER = $7FF206
!REFRESH_HUD = $7FF208
!DEBUG_ADDR = $7FF250

!HUD_WRAM_START = $7FF000
!HUD_WRAM_4_START = $F000
!HUD_VRAM_TOP = $0b4E

;8EAD
org $C08CAD
db $4C,"e_",$44,"og",$00

; this is just where the game clear the bg3 vram
;org $CCAFE2
;jsl stuff 


;org $CF8871


; This does a bunch of DMA for the HUD and refresh the hud
;org $CF899C
;CF/899C:        A90200          lda #$0002 
;CF/899F:        0C290B          tsb $0B29
;  JSL after_hud_refresh
; NOP : NOP

;this is like the last dma/update call of the hud stuff 
;org $CF8819
;NOP : NOP : NOP : NOP

;CF/8826:	AF61227E	lda $7E2261
org $CF8826
  JSL after_hud_refresh
  
;;  ; 	A		B	  Y      X        L      R      	>		<		v		^	Start	  select
  ;; DW #$0080, #$8000, #$4000, #$0040, #$0020, #$0010, #$0100, #$0200, #$0400, #$0800, #$1000, #$2000,
  
;; let do what we want
org $F10000

after_hud_refresh:
 ;jsl $808650
 ;lda #$0002
 ;tsb $0B29
 php 
 pha
 ;lda #$0000
 ;STA !REFRESH_HUD
 lda #!HUD_WRAM_4_START
 sta $26
 lda #!HUD_VRAM_TOP
 sta $2e
 ldx #$0008
 jsl $808650
 ;.endstuff
 pla
 lda $7E2261
 plb
 rtl

 
start_practice_stuff:
  ;and $0104 ;
  ;sta $0104 ; what we replaced
  
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


; 74 ->	7A
save_01_06:
	lda $01
	sta $74
	lda $03
	sta $76
	lda $05
	sta $78
	rts

restore_01_06:
	lda $74
	sta $01
	lda $76
	sta $03
	lda $78
	sta $05
	rts

	
update_hud:
  
  ;Frame counter
  STA !DEBUG_ADDR
  
  jsr save_01_06
  
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
  %a16()
  jsr restore_01_06
  rts
    
