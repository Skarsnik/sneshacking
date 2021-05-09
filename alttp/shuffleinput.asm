lorom

; $04CB is free ram, let use 12x2 bytes for our changes
!TABLE_ICHANGE = $04CB
!CPY_ORIGINAL_INPUT = $7C

; $80-81 copy of input
!CPY_INPUT = $80
!ROOM_COUNTER = $2B2

; - $82[0x1]: Copy of $10
; - $83[0x1]: Copy of $11
; $7C arg?
; $7E set to initialise once
!TAB_INIT = $7E


macro a8()
    SEP #$20
endmacro

macro a16()
    REP #$20
endmacro

macro i8()
    SEP #$10
endmacro

macro i16()
    REP #$10
endmacro

macro ai8()
    SEP #$30
endmacro

macro ai16()
    REP #$30
endmacro


; This is the readjoypad routine
org $0003D1

  STZ $4016 
  ; Storing the state of Joypad 1 to $00-$01
  LDA $4218 : STA $00
  STA $7D
  LDA $4219 : STA $01
  STA $7C
  
  ; added, we don't want to do stuff on empty input
  ORA $00 : BEQ .a
    JSL change_input
  .a
  
  ; $F2 has the pure joypad data
  LDA $00 : STA $F2 : TAY 
        
  ; $FA at this point contains the joypad data from the last frame.
  ; This is intended to avoid flooding in processing commands.
  ; Send this "button masked" reading here.
  ; Hence $F2 and $FA contain pure joypad readings from this frame now.
  EOR $FA : AND $F2 : STA $F6 : STY $FA
        
  ; Essentially the same procedure as above, but for the other half of JP1.
  LDA $01 : STA $F0 : TAY
  EOR $F8 : AND $F0 : STA $F4 : STY $F8
        
  RTS
  
  ;  J2 is skipped, hey free space code! it's here to have an idea of the extra size we can use here
        
  ;LDA $421A : STA $00
  ;LDA $421B : STA $01
        
  ;LDA $00 : STA $F3 : TAY
  ;EOR $FB : AND $F3 : STA $F7 : STY $FB
        
  ;LDA $01 : STA $F1 : TAY
  ;EOR $F9 : AND $F1 : STA $F5 : STY $F9
        
  ;RTS


org $008056
    JSL gamemode_hook


; == INPUT DISPLAY STUFF	
; Hook into subroutine that transfers hud tiles to vram
org $028068
    JSL load_tile_gfx_hook


	
	
; Game Mode hook
org $1BB1E0

; This change input according to the table of change
change_input:
  PHP
  %i16()
  %a8()
  LDA $00 : STA $8E
  LDA $01 : STA $8F
  %a16()
  LDA $8E
  PHB : PHK : PLB
  BEQ .end ; if input is 0 do nothing
  LDX #$0000
  STZ !CPY_INPUT
  .loop
    LDA $8E
    LDY.w table_input_bitmask, X ; debug stuff
    SEP #$02
    BIT.w table_input_bitmask, X : BEQ .endif
      LDA !TABLE_ICHANGE, X
      ADC !TABLE_ICHANGE, X
      TAY
      LDA table_input_bitmask, Y
      ORA !CPY_INPUT
      STA !CPY_INPUT
    .endif
    INX
    INX
    CPX #$0018 : BNE .loop
  .afterloop
  ; let's prevent up + down and left + right
  LDA !CPY_INPUT
  AND #$0C00 : CMP #$0C00 : BEQ .del_ud
  LDA !CPY_INPUT
  AND #$0300 : CMP #$0300 : BEQ .del_lr
  JMP .hello
  .del_ud
    EOR #$0400
    JMP .ch_i
  .del_lr
    EOR #$0100
  .ch_i
    STA !CPY_INPUT
  .hello
  %a8()
  LDX #$0001
  LDA !CPY_INPUT
  STA $00
  LDA !CPY_INPUT, X
  STA $01
  .end
  PLB
  PLP
  %a8()
  %i8()
  RTL

; ==== GAME HOOK ===
gamemode_hook:
  PHP
  %i16()
  %a16()
  LDA #$0000
  CMP !TAB_INIT : BEQ .init_table
  JMP .normal
  .init_table
    STZ !ROOM_COUNTER
    LDA #$0000
    STA $82
    LDX #$0000
    LDY #$0000
    .loop
    TXA
    STA !TABLE_ICHANGE, Y
    INX
    INY
    INY
    CPX #$000C : BNE .loop
  LDA #$0001
  STA !TAB_INIT
  .normal
  JSR transition_detection
  JSR input_display
  JMP end_of_gamemode_hook

end_of_gamemode_hook:
  PLP
  JSL $0080B5 ; GameModes
  RTL

shuffle_dpad:
  LDA #$0006
  STA $00
  LDA #$0009
  STA $02
  JSR shuffle_table_input
  RTS

shuffle_button_noSS:
  LDA #$0000
  STA $00
  LDA #$0005
  STA $02
  JSR shuffle_table_input
  RTS

shuffle_all_noSS:
  LDA #$0000
  STA $00
  LDA #$0009
  STA $02
  JSR shuffle_table_input
  RTS

; index are wrong here   

;$4204/4205 is the 16 bit dividend, $4206 is the 8bit divisor, the
;quotient will be put in $4214, and the remainder in $4216/4217.
	
; 00 02 table range
!TAB_START = $00
!TAB_STOP = $02
!RANGE_SIZE = $04
shuffle_table_input:
  print "Shuffle table code start at: ", pc
  LDX !TAB_STOP
  LDA !TAB_STOP
  SBC !TAB_START
  INA
  STA !RANGE_SIZE
  .loop
    STX $06
    LDA $0FA0 ; RNG value
  ADC $1A
  ; modulo
  STA $4204
  ; (original calc should be range - (top - pos) but let avoid neg)
  LDA !TAB_STOP
  CLC
  SBC $06
  STA $08
  LDA !RANGE_SIZE
  CLC
  SBC $08
  ;LDA !RANGE_SIZE = OLD code with neg, meh
  ;SBC !TAB_STOP
  ; ADC $06
  %a8()
  STA $4206 ; need to wait 8 cycles for the register to do its work
  %a16()
  NOP : NOP : NOP
  LDA $4216
  ADC $00
  STA $06 : ADC $06 : TAY; double this shit for index
  TXA : STA $06 :	ADC $06 : PHX : TAX ; double X
  LDA !TABLE_ICHANGE, X
  STA $06
  LDA !TABLE_ICHANGE, Y
  STA !TABLE_ICHANGE, X
  LDA $06
  STA !TABLE_ICHANGE, Y
  PLX
  DEX
  LDA $00
  DEA
  STA $06
  CPX $06 : BNE .loop
  RTS


table_input_bitmask:
  ; 	A		B	  Y      X        L      R      	>		<		v		^	Start	  select
  DW #$0080, #$8000, #$4000, #$0040, #$0020, #$0010, #$0100, #$0200, #$0400, #$0800, #$1000, #$2000,
  
  
  
;; From helg practice romhack  
transition_detection:
  ; Transition detection {{{

  %ai8()
  LDA $10 : CMP $82 : BNE .gamemode_changed
  LDA $11 : CMP $83 : BNE .submode_changed
  RTS

  .gamemode_changed
    LDA $82

    CMP #$05 : BEQ .gamemode_load_game
    CMP #$07 : BEQ .gamemode_dungeon
    CMP #$09 : BEQ .gamemode_overworld
    CMP #$0B : BEQ .gamemode_overworld ; "Special" overworld (?)
    CMP #$13 : BEQ .gamemode_victory ; LW?
    CMP #$16 : BEQ .gamemode_victory ; DW?

    JMP .end

  .gamemode_victory
    LDA $10

    ; Just killed a boss. Loading overworld.
    CMP #$08 : BEQ .shuffle_input

    JMP .end

  .gamemode_load_game
    LDA $10

    ; Link in bed
    CMP #$07 : BEQ .shuffle_input

    JMP .end

  .gamemode_dungeon
    LDA $10

  ; Dungeon -> Overworld
    CMP #$0F : BEQ .shuffle_input

  ; Caught by Wall Master
    CMP #$11 : BEQ .shuffle_input

    JMP .end

  .gamemode_overworld
    LDA $10

    ; OW (special) -> OW
    CMP #$09 : BEQ .shuffle_input

    ; OW -> OW (special)
    CMP #$0B : BEQ .shuffle_input

    ; Overworld -> Dungeon
    CMP #$0F : BEQ .shuffle_input

    ; Fall in hole
    CMP #$11 : BEQ .shuffle_input

    JMP .end

  .submode_changed
    LDA $10

    ; Dungeon
    CMP #$07 : BEQ .submode_dungeon

    ; Overworld
    CMP #$09 : BEQ .submode_overworld

    JMP .end

  .submode_overworld
    LDA $11

    ; Normal transition
    CMP #$01 : BEQ .room_or_map_changed

    ; Transition into Dark Woods
    CMP #$0D : BEQ .shuffle_input

    ; Mirror
    CMP #$23 : BEQ .shuffle_input

    ; Whirlpool
    CMP #$2E : BEQ .shuffle_input

    JMP .end

  .submode_dungeon
    LDA $11

    ; Normal transition intra-room
    CMP #$01 : BEQ .room_or_map_changed

    ; Normal transition inter-room
    CMP #$02 : BEQ .room_or_map_changed

    ; Transition upwards
    CMP #$06 : BEQ .shuffle_input

    ; Transition upwards
    CMP #$07 : BEQ .shuffle_input

    ; Walking up straight inter-room staircase
    CMP #$12 : BEQ .shuffle_input

    ; Walking down straight inter-room staircase
    CMP #$13 : BEQ .shuffle_input

    ; Transition inter-room staircase
    CMP #$0E : BEQ .shuffle_input

    JMP .end
  ; This is for shuffling input only every 3 normal transitions
  .shuffle_input
    STZ !ROOM_COUNTER
    ; difficulty are choosen from the file slot, it start at 02
    LDA $701FFE
    CMP #$02 : BEQ .baby_shuffle
    CMP #$04 : BEQ .normal_shuffle
    CMP #$06 : BEQ .hard_shuffle
    JMP .end 
    .baby_shuffle
      %ai16()
      JSR shuffle_button_noSS
      JMP .end
    .normal_shuffle
      %ai16()
      JSR shuffle_dpad
      JSR shuffle_button_noSS
    	JMP .end
    .hard_shuffle
      %ai16()
  	  JSR shuffle_all_noSS
      JMP .end
  .room_or_map_changed
    LDA !ROOM_COUNTER
	  INA
    STA !ROOM_COUNTER
    CMP #$03 : BNE .end
    STZ !ROOM_COUNTER
    JMP .shuffle_input
    .end
    %a8()
    ; Persist new game mode/submode.
    LDA $10 : STA $82
    LDA $11 : STA $83
    RTS
    ; }}}


print "1bb1e0 section ends at ", pc, ". Max is 1bb800"	
incsrc inputdisplay.asm
print "1bb1e0 section ends at ", pc, ". Max is 1bb800"	
