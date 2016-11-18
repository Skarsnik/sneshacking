
!MAP_ID = $ADB
!PREVIOUS_MAP_ID = $7FF200
!MAP_FRAME_COUNT = $7FF202
!COPY_MAP_FRAME_COUNT = $7FF220

!MNI_MAP_FRAME_COUNT = $7FF222
!MNI_SS_LOAD_FRAME_COUNT = $7FF224
!MNI_GLOBAL_FRAME_COUNT = $7FF226

!INIT = $7FF204
!PREV_MAP_COUNTER = $7FF206
!REFRESH_HUD = $7FF208
!DEBUG_ADDR = $7FF250

!HUD_WRAM_FRAME_COUNTER = $7FF000
!HUD_WRAM_FRAME_COUNTER_SEC = $7FF010
!HUD_WRAM_START = $7FF000
!HUD_WRAM_FRAME_COUNTER_4_START = $F000
!HUD_WRAM_FRAME_COUNTER_SEC_4_START = $F010
!HUD_VRAM_FRAME_COUNTER_START = $0b4E
!HUD_VRAM_FRAME_COUNTER_SEC_START = $0b6D

;8EAD
org $C08CAD
db $4C,"e_",$44,"og",$00


; This is the start of the MNI routine
;C0/8247:	A90000  	lda #$0000
;C0/824A:	5B      	tcd

org $C08247
 jsl mni_begin

;CF/8826:	AF61227E	lda $7E2261
org $CF8826
  JSL after_hud_refresh
  
;;  ; 	A		B	  Y      X        L      R      	>		<		v		^	Start	  select
  ;; DW #$0080, #$8000, #$4000, #$0040, #$0020, #$0010, #$0100, #$0200, #$0400, #$0800, #$1000, #$2000,
  
;; let do what we want
org $F10000

mni_begin:
  lda !MNI_GLOBAL_FRAME_COUNT
  INA
  sta !MNI_GLOBAL_FRAME_COUNT
  
  lda !MNI_MAP_FRAME_COUNT
  INA
  sta !MNI_MAP_FRAME_COUNT
  
  lda !MNI_SS_LOAD_FRAME_COUNT
  INA
  sta !MNI_SS_LOAD_FRAME_COUNT
  
  lda #$0000
  tcd
rtl

after_hud_refresh:
 ;jsl $808650
 ;lda #$0002
 ;tsb $0B29
 php 
 pha
 ;lda #$0000
 ;STA !REFRESH_HUD
 lda #!HUD_WRAM_FRAME_COUNTER_4_START
 sta $26
 lda #!HUD_VRAM_FRAME_COUNTER_START
 sta $2e
 ldx #$0008
 jsl $808650
 ;.endstuff
 
 
 lda #!HUD_WRAM_FRAME_COUNTER_SEC_4_START
 sta $26
 lda #!HUD_VRAM_FRAME_COUNTER_SEC_START
 sta $2e
 ldx #$000a
 jsl $808650
 
 
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

  ; the decompose routine erase 02->06
  ; need to save them 
  jsr save_01_06
  ;Frame counter
  
  %a8()
  ldx #$0000
  ldy #$0000
  .loopclear
    lda #$00
    sta !HUD_WRAM_FRAME_COUNTER, x
	INX
	CPX #$0008 : BNE .loopclear
  %a16()
  lda #!HUD_WRAM_FRAME_COUNTER_4_START
  sta $F0
  lda #$0008
  sta $7A
  lda !MAP_FRAME_COUNT
  sta !COPY_MAP_FRAME_COUNT
  CMP.w #10000
  ;Probably need something better
  BMI .nooverflow
    lda.w #9999
  .nooverflow
  jsr draw_numbers

  ; Frame counter human readable time
  ; init to 00>00
  lda #$2bec
  ldx #$0000
  .loopzero
    sta !HUD_WRAM_FRAME_COUNTER_SEC, X
    INX
	INX
	CPX #$000A : BNE .loopzero
  ldx #$0004
  lda #$2bf6
  sta !HUD_WRAM_FRAME_COUNTER_SEC, X
  ;STA !DEBUG_ADDR
  ;$4204/4205 is the 16 bit dividend, $4206 is the 8bit divisor, the
  ;quotient will be put in $4214, and the remainder in $4216/4217.
  LDA !MAP_FRAME_COUNT
  STA $4204
  %a8()
  LDA #60
  STA $4206 
  %a16()
  lda #!HUD_WRAM_FRAME_COUNTER_SEC_4_START
  sta $F0
  lda #$0004
  sta $7A
  lda $4216
  sta $7C
  lda #$0000
  %a8()
  LDA $4214 ; result
  %a16()
  jsr draw_numbers
  
  lda #!HUD_WRAM_FRAME_COUNTER_SEC_4_START
  clc
  adc #$0006
  sta $F0
  lda #$0004
  sta $7A
  LDA $7C; reste
  ;sta !DEBUG_ADDR
  jsr draw_numbers
  
  jsr restore_01_06
  rts
  
  
; Use A for the number to draw
; $F0 for the WRAM addr to put it
; $7A for the size in WRAM (number lenght x 2)
draw_numbers:

  jsl $80870C ; this divise A and decompose it to $02, $03... until #FF
  ;STA !DEBUG_ADDR
  %a8()
  lda #$7F
  sta $F2
  ldy #$0000
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
  lda $7A
  sbc $70
  tay
  ldx #$0000
  .loop
    lda #$FF
    CMP $02, x : BEQ .endloop
    lda #$EC
	clc
    adc $02, x
	;stx $70 ; save x
	;tyx
	sta [$F0], Y
	INY
	lda #$2B
	sta [$F0], Y
	INY
	;ldx $70 ; restaure x
	INX
	JMP .loop
  .endloop
  %a16()
  rts
    
