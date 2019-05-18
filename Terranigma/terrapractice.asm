!MAP_ID = $7E047E
!MONEY = $7E0694
!PREVIOUS_MAP_ID = $7FF200
!MAP_FRAME_COUNT = $7FF202

; $7f0caa
;; 9cd

!INIT = $7FF204

org $FF0600

start_practice_stuff:

  PHP
  PHX
  PHY
  
  
  CMP !INIT : BEQ .init
  JMP .normal  
  .init
    STZ !MAP_FRAME_COUNT
    LDA #$0001
	STA !INIT
  .normal
  
  LDA !MAP_ID
  CMP !PREVIOUS_MAP_ID : BEQ .endif
   LDA !MAP_FRAME_COUNT
   ;STA !MONEY
   LDA #$0000
   STA !MAP_FRAME_COUNT
  .endif
  LDA !MAP_ID
  STA !PREVIOUS_MAP_ID
  LDA !MAP_FRAME_COUNT : INA : STA !MAP_FRAME_COUNT
  PLY
  PLX
  PLP
  RTL

