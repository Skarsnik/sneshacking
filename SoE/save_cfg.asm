; Savestate code variables
!SS_BANK = $F00000
!SS_CODE = $F00000

!SS_HOOK = $00823A
!SS_INPUT_CUR = $104
!SS_INPUT_NEW = $F0
!SS_INPUT_PREV = $F2
!SS_INPUT_SAVE = #$1010	; start + R
!SS_INPUT_LOAD = #$1020 ; start + L
;!SS_INPUT_COMPARE = #$6000 ; select + y

!SRAM_DMA_BANK = $B96200

!SRAM_SAVED_SP = $B97004
!SRAM_VM_RETURN = $B97006

!SRAM_SIZE = $080000

; Game-specific patches for save-state code

;org $C0FFD8
;	db $08		; set sram size to 256kb

; End of game-specific patches	