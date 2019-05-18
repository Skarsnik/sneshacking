; SD2SNES Savestate code
; by acmlm, total, Myria
;

;org !SS_HOOK
;	jml ss_start

org $C084a3
  JSL ss_start ; size 4
  NOP ; size 1
  NOP ; size 1
	
org !SS_CODE
print "Savestate Bank Starting at: ", pc
; These can be modified to do game-specific things before and after saving and loading
; Both A and X/Y are 16-bit here

pre_load_state:
	rts

post_load_state:
	rts
; end of post_load_state

pre_save_state: 
	rts
	
post_save_state:
	rts
	
; These restored registers are game-specific and needs to be updated for different games
register_restore_return:

    ;LDA #$A1 : STA $4200
    ;LDA #$0F : STA $2100
	;lda #$00A1
	sep #$20
    lda #$81
	sta $4200
    sta $0E6B
	;sta $4200		; Reset interrupts register
	
	;jmp $80802D
	lda #$0F
	sta $2100		; Reset PPU Control register
	rep #$20

; Code to run before returning back to the game
ss_exit:	
	lda !SS_INPUT_CUR
	sta !SS_INPUT_PREV
	jsl start_practice_stuff
	plp
	plb
	rtl

; Code to run just after hijacking
ss_start:
	and $0104 ;
    sta $0104 ; what we replaced
	
	phb
	php
	%ai16()

; Savestate code starts here -- no more customization should be needed below here
save_state_code:	
	lda !SS_INPUT_CUR
	eor !SS_INPUT_PREV
	sta !SS_INPUT_NEW
	
	lda !SS_INPUT_CUR	
	;bit !SS_INPUT_COMPARE
	;beq ss_exit

	and !SS_INPUT_NEW
	beq ss_exit
	
	lda !SS_INPUT_CUR
	cmp !SS_INPUT_SAVE	
	beq save_state
	
	cmp !SS_INPUT_LOAD
	bne ss_exit
	jmp load_state
	
save_state:
	jsr pre_save_state
	pea $0000
	plb
	plb
	
	; Store DMA registers to SRAM
	%a8();
	ldy #$0000
	tyx
	
save_dma_regs:
	lda $4300, x
	sta !SRAM_DMA_BANK, x
	inx
	iny
	cpy #$000B
	bne save_dma_regs
	cpx #$007B
	beq save_dma_regs_done
	inx #5
	ldy #$0000
	bra save_dma_regs

save_dma_regs_done:
	%ai16()
	ldx #save_write_table

run_vm:
	pea $0000
	plb
	plb
	jmp vm

save_write_table:
	; Turn PPU off
	dw $1000|$2100, $80
	dw $1000|$4200, $00
	; Single address, B bus -> A bus.  B address = reflector to WRAM ($2180).
	dw $0000|$4310, $8080  ; direction = B->A, byte reg, B addr = $2180
	
	; Copy WRAM 7E0000-7E1FFF to SRAM A16000-A17FFF.
	dw $0000|$4312, $6000  ; A addr = $xx0000
	dw $0000|$4314, $00A1  ; A addr = $71xxxx, size = $xx00
	dw $0000|$4316, $0020  ; size = $80xx ($8000), unused bank reg = $00.
	dw $0000|$2181, $0000  ; WRAM addr = $xx0000
	dw $1000|$2183, $00    ; WRAM addr = $7Exxxx  (bank is relative to $7E)
	dw $1000|$420B, $02    ; Trigger DMA on channel 1

	; Copy WRAM 7E2000-7E3FFF to SRAM A26000-A27FFF.
	dw $0000|$4312, $6000  ; A addr = $xx0000
	dw $0000|$4314, $00A2  ; A addr = $71xxxx, size = $xx00
	dw $0000|$4316, $0020  ; size = $80xx ($8000), unused bank reg = $00.
	dw $0000|$2181, $2000  ; WRAM addr = $xx0000
	dw $1000|$2183, $00    ; WRAM addr = $7Exxxx  (bank is relative to $7E)
	dw $1000|$420B, $02    ; Trigger DMA on channel 1

	; Copy WRAM 7E4000-7E5FFF to SRAM A36000-A37FFF.
	dw $0000|$4312, $6000  ; A addr = $xx0000
	dw $0000|$4314, $00A3  ; A addr = $71xxxx, size = $xx00
	dw $0000|$4316, $0020  ; size = $80xx ($8000), unused bank reg = $00.
	dw $0000|$2181, $4000  ; WRAM addr = $xx0000
	dw $1000|$2183, $00    ; WRAM addr = $7Exxxx  (bank is relative to $7E)
	dw $1000|$420B, $02    ; Trigger DMA on channel 1

	; Copy WRAM 7E6000-7E7FFF to SRAM A46000-A47FFF.
	dw $0000|$4312, $6000  ; A addr = $xx0000
	dw $0000|$4314, $00A4  ; A addr = $71xxxx, size = $xx00
	dw $0000|$4316, $0020  ; size = $80xx ($8000), unused bank reg = $00.
	dw $0000|$2181, $6000  ; WRAM addr = $xx0000
	dw $1000|$2183, $00    ; WRAM addr = $7Exxxx  (bank is relative to $7E)
	dw $1000|$420B, $02    ; Trigger DMA on channel 1

	; Copy WRAM 7E8000-7E9FFF to SRAM A56000-A57FFF.
	dw $0000|$4312, $6000  ; A addr = $xx0000
	dw $0000|$4314, $00A5  ; A addr = $71xxxx, size = $xx00
	dw $0000|$4316, $0020  ; size = $80xx ($8000), unused bank reg = $00.
	dw $0000|$2181, $8000  ; WRAM addr = $xx0000
	dw $1000|$2183, $00    ; WRAM addr = $7Exxxx  (bank is relative to $7E)
	dw $1000|$420B, $02    ; Trigger DMA on channel 1

	; Copy WRAM 7EA000-7EBFFF to SRAM A66000-A67FFF.
	dw $0000|$4312, $6000  ; A addr = $xx0000
	dw $0000|$4314, $00A6  ; A addr = $71xxxx, size = $xx00
	dw $0000|$4316, $0020  ; size = $80xx ($8000), unused bank reg = $00.
	dw $0000|$2181, $A000  ; WRAM addr = $xx0000
	dw $1000|$2183, $00    ; WRAM addr = $7Exxxx  (bank is relative to $7E)
	dw $1000|$420B, $02    ; Trigger DMA on channel 1

	; Copy WRAM 7EC000-7EDFFF to SRAM A76000-A77FFF.
	dw $0000|$4312, $6000  ; A addr = $xx0000
	dw $0000|$4314, $00A7  ; A addr = $71xxxx, size = $xx00
	dw $0000|$4316, $0020  ; size = $80xx ($8000), unused bank reg = $00.
	dw $0000|$2181, $C000  ; WRAM addr = $xx0000
	dw $1000|$2183, $00    ; WRAM addr = $7Exxxx  (bank is relative to $7E)
	dw $1000|$420B, $02    ; Trigger DMA on channel 1

	; Copy WRAM 7EE000-7EFFFF to SRAM A86000-A87FFF.
	dw $0000|$4312, $6000  ; A addr = $xx0000
	dw $0000|$4314, $00A8  ; A addr = $71xxxx, size = $xx00
	dw $0000|$4316, $0020  ; size = $80xx ($8000), unused bank reg = $00.
	dw $0000|$2181, $E000  ; WRAM addr = $xx0000
	dw $1000|$2183, $00    ; WRAM addr = $7Exxxx  (bank is relative to $7E)
	dw $1000|$420B, $02    ; Trigger DMA on channel 1

	
	; Copy WRAM 7F0000-7F1FFF to SRAM A96000-A97FFF.
	dw $0000|$4312, $6000  ; A addr = $xx0000
	dw $0000|$4314, $00A9  ; A addr = $71xxxx, size = $xx00
	dw $0000|$4316, $0020  ; size = $80xx ($8000), unused bank reg = $00.
	dw $0000|$2181, $0000  ; WRAM addr = $xx0000
	dw $1000|$2183, $01    ; WRAM addr = $7Exxxx  (bank is relative to $7E)
	dw $1000|$420B, $02    ; Trigger DMA on channel 1

	; Copy WRAM 7F2000-7F3FFF to SRAM AA6000-AA7FFF.
	dw $0000|$4312, $6000  ; A addr = $xx0000
	dw $0000|$4314, $00AA  ; A addr = $71xxxx, size = $xx00
	dw $0000|$4316, $0020  ; size = $80xx ($8000), unused bank reg = $00.
	dw $0000|$2181, $2000  ; WRAM addr = $xx0000
	dw $1000|$2183, $01    ; WRAM addr = $7Exxxx  (bank is relative to $7E)
	dw $1000|$420B, $02    ; Trigger DMA on channel 1

	; Copy WRAM 7F4000-7F5FFF to SRAM AB6000-AB7FFF.
	dw $0000|$4312, $6000  ; A addr = $xx0000
	dw $0000|$4314, $00AB  ; A addr = $71xxxx, size = $xx00
	dw $0000|$4316, $0020  ; size = $80xx ($8000), unused bank reg = $00.
	dw $0000|$2181, $4000  ; WRAM addr = $xx0000
	dw $1000|$2183, $01    ; WRAM addr = $7Exxxx  (bank is relative to $7E)
	dw $1000|$420B, $02    ; Trigger DMA on channel 1

	; Copy WRAM 7F6000-7F7FFF to SRAM AC6000-AC7FFF.
	dw $0000|$4312, $6000  ; A addr = $xx0000
	dw $0000|$4314, $00AC  ; A addr = $71xxxx, size = $xx00
	dw $0000|$4316, $0020  ; size = $80xx ($8000), unused bank reg = $00.
	dw $0000|$2181, $6000  ; WRAM addr = $xx0000
	dw $1000|$2183, $01    ; WRAM addr = $7Exxxx  (bank is relative to $7E)
	dw $1000|$420B, $02    ; Trigger DMA on channel 1

	; Copy WRAM 7F8000-7F9FFF to SRAM AD6000-AD7FFF.
	dw $0000|$4312, $6000  ; A addr = $xx0000
	dw $0000|$4314, $00AD  ; A addr = $71xxxx, size = $xx00
	dw $0000|$4316, $0020  ; size = $80xx ($8000), unused bank reg = $00.
	dw $0000|$2181, $8000  ; WRAM addr = $xx0000
	dw $1000|$2183, $01    ; WRAM addr = $7Exxxx  (bank is relative to $7E)
	dw $1000|$420B, $02    ; Trigger DMA on channel 1

	; Copy WRAM 7FA000-7FBFFF to SRAM AE6000-AE7FFF.
	dw $0000|$4312, $6000  ; A addr = $xx0000
	dw $0000|$4314, $00AE  ; A addr = $71xxxx, size = $xx00
	dw $0000|$4316, $0020  ; size = $80xx ($8000), unused bank reg = $00.
	dw $0000|$2181, $A000  ; WRAM addr = $xx0000
	dw $1000|$2183, $01    ; WRAM addr = $7Exxxx  (bank is relative to $7E)
	dw $1000|$420B, $02    ; Trigger DMA on channel 1

	; Copy WRAM 7FC000-7FDFFF to SRAM AF6000-AF7FFF.
	dw $0000|$4312, $6000  ; A addr = $xx0000
	dw $0000|$4314, $00AF  ; A addr = $71xxxx, size = $xx00
	dw $0000|$4316, $0020  ; size = $80xx ($8000), unused bank reg = $00.
	dw $0000|$2181, $C000  ; WRAM addr = $xx0000
	dw $1000|$2183, $01    ; WRAM addr = $7Exxxx  (bank is relative to $7E)
	dw $1000|$420B, $02    ; Trigger DMA on channel 1

	; Copy WRAM 7FE000-7FFFFF to SRAM B06000-B07FFF.
	dw $0000|$4312, $6000  ; A addr = $xx0000
	dw $0000|$4314, $00BA  ; A addr = $71xxxx, size = $xx00
	dw $0000|$4316, $0020  ; size = $80xx ($8000), unused bank reg = $00.
	dw $0000|$2181, $E000  ; WRAM addr = $xx0000
	dw $1000|$2183, $01    ; WRAM addr = $7Exxxx  (bank is relative to $7E)
	dw $1000|$420B, $02    ; Trigger DMA on channel 1
	

	
	; Address pair, B bus -> A bus.  B address = VRAM read ($2139).
	dw $0000|$4310, $3981  ; direction = B->A, word reg, B addr = $2139
	dw $1000|$2115, $0000  ; VRAM address increment mode.

	; Copy VRAM 0000-1FFF to SRAM B16000-B17FFF.
	dw $0000|$2116, $0000  ; VRAM address >> 1.
	dw $9000|$2139, $0000  ; VRAM dummy read.
	dw $0000|$4312, $6000  ; A addr = $xx0000
	dw $0000|$4314, $00B1  ; A addr = $75xxxx, size = $xx00
	dw $0000|$4316, $0020  ; size = $80xx ($0000), unused bank reg = $00.
	dw $1000|$420B, $02    ; Trigger DMA on channel 1

	; Copy VRAM 2000-3FFF to SRAM B26000-B27FFF.
	dw $0000|$2116, $1000  ; VRAM address >> 1.
	dw $9000|$2139, $0000  ; VRAM dummy read.
	dw $0000|$4312, $6000  ; A addr = $xx0000
	dw $0000|$4314, $00B2  ; A addr = $75xxxx, size = $xx00
	dw $0000|$4316, $0020  ; size = $80xx ($0000), unused bank reg = $00.
	dw $1000|$420B, $02    ; Trigger DMA on channel 1

	; Copy VRAM 4000-5FFF to SRAM B36000-B37FFF.
	dw $0000|$2116, $2000  ; VRAM address >> 1.
	dw $9000|$2139, $0000  ; VRAM dummy read.
	dw $0000|$4312, $6000  ; A addr = $xx0000
	dw $0000|$4314, $00B3  ; A addr = $75xxxx, size = $xx00
	dw $0000|$4316, $0020  ; size = $80xx ($0000), unused bank reg = $00.
	dw $1000|$420B, $02    ; Trigger DMA on channel 1

	; Copy VRAM 6000-7FFF to SRAM B46000-B47FFF.
	dw $0000|$2116, $3000  ; VRAM address >> 1.
	dw $9000|$2139, $0000  ; VRAM dummy read.
	dw $0000|$4312, $6000  ; A addr = $xx0000
	dw $0000|$4314, $00B4  ; A addr = $75xxxx, size = $xx00
	dw $0000|$4316, $0020  ; size = $80xx ($0000), unused bank reg = $00.
	dw $1000|$420B, $02    ; Trigger DMA on channel 1
	
	; Copy VRAM 8000-9FFF to SRAM B56000-B57FFF.
	dw $0000|$2116, $4000  ; VRAM address >> 1.
	dw $9000|$2139, $0000  ; VRAM dummy read.
	dw $0000|$4312, $6000  ; A addr = $xx0000
	dw $0000|$4314, $00B5  ; A addr = $75xxxx, size = $xx00
	dw $0000|$4316, $0020  ; size = $80xx ($0000), unused bank reg = $00.
	dw $1000|$420B, $02    ; Trigger DMA on channel 1

	; Copy VRAM A000-BFFF to SRAM B66000-B67FFF.
	dw $0000|$2116, $5000  ; VRAM address >> 1.
	dw $9000|$2139, $0000  ; VRAM dummy read.
	dw $0000|$4312, $6000  ; A addr = $xx0000
	dw $0000|$4314, $00B6  ; A addr = $75xxxx, size = $xx00
	dw $0000|$4316, $0020  ; size = $80xx ($0000), unused bank reg = $00.
	dw $1000|$420B, $02    ; Trigger DMA on channel 1

	; Copy VRAM C000-DFFF to SRAM B76000-B77FFF.
	dw $0000|$2116, $6000  ; VRAM address >> 1.
	dw $9000|$2139, $0000  ; VRAM dummy read.
	dw $0000|$4312, $6000  ; A addr = $xx0000
	dw $0000|$4314, $00B7  ; A addr = $75xxxx, size = $xx00
	dw $0000|$4316, $0020  ; size = $80xx ($0000), unused bank reg = $00.
	dw $1000|$420B, $02    ; Trigger DMA on channel 1

	; Copy VRAM E000-FFFF to SRAM B86000-B87FFF.
	dw $0000|$2116, $7000  ; VRAM address >> 1.
	dw $9000|$2139, $0000  ; VRAM dummy read.
	dw $0000|$4312, $6000  ; A addr = $xx0000
	dw $0000|$4314, $00B8  ; A addr = $75xxxx, size = $xx00
	dw $0000|$4316, $0020  ; size = $80xx ($0000), unused bank reg = $00.
	dw $1000|$420B, $02    ; Trigger DMA on channel 1

	; Copy CGRAM 000-1FF to SRAM B96000-B961FF.
	dw $1000|$2121, $00    ; CGRAM address
	dw $0000|$4310, $3B80  ; direction = B->A, byte reg, B addr = $213B
	dw $0000|$4312, $6000  ; A addr = $xx2000
	dw $0000|$4314, $00B9  ; A addr = $77xxxx, size = $xx00
	dw $0000|$4316, $0002  ; size = $02xx ($0200), unused bank reg = $00.
	dw $1000|$420B, $02    ; Trigger DMA on channel 1
	; Done
	dw $0000, save_return

save_return:
	pea $0000
	plb
	plb
	
	%ai16()
	tsa
	sta !SRAM_SAVED_SP
	jsr post_save_state
	jmp register_restore_return
	
	
load_state:
	jsr pre_load_state
	pea $0000
	plb
	plb
	
	%a8()
	ldx #load_write_table
	jmp run_vm

load_write_table:
	; Disable HDMA
	dw $1000|$420C, $00
	; Turn PPU off
	dw $1000|$2100, $80
	dw $1000|$4200, $00
	; Single address, A bus -> B bus.  B address = reflector to WRAM ($2180).
	dw $0000|$4310, $8000  ; direction = A->B, B addr = $2180
	
	; Copy WRAM 7E0000-7E1FFF to SRAM A16000-A17FFF.
	dw $0000|$4312, $6000  ; A addr = $xx0000
	dw $0000|$4314, $00A1  ; A addr = $71xxxx, size = $xx00
	dw $0000|$4316, $0020  ; size = $80xx ($8000), unused bank reg = $00.
	dw $0000|$2181, $0000  ; WRAM addr = $xx0000
	dw $1000|$2183, $00    ; WRAM addr = $7Exxxx  (bank is relative to $7E)
	dw $1000|$420B, $02    ; Trigger DMA on channel 1

	; Copy WRAM 7E2000-7E3FFF to SRAM A26000-A27FFF.
	dw $0000|$4312, $6000  ; A addr = $xx0000
	dw $0000|$4314, $00A2  ; A addr = $71xxxx, size = $xx00
	dw $0000|$4316, $0020  ; size = $80xx ($8000), unused bank reg = $00.
	dw $0000|$2181, $2000  ; WRAM addr = $xx0000
	dw $1000|$2183, $00    ; WRAM addr = $7Exxxx  (bank is relative to $7E)
	dw $1000|$420B, $02    ; Trigger DMA on channel 1

	; Copy WRAM 7E4000-7E5FFF to SRAM A36000-A37FFF.
	dw $0000|$4312, $6000  ; A addr = $xx0000
	dw $0000|$4314, $00A3  ; A addr = $71xxxx, size = $xx00
	dw $0000|$4316, $0020  ; size = $80xx ($8000), unused bank reg = $00.
	dw $0000|$2181, $4000  ; WRAM addr = $xx0000
	dw $1000|$2183, $00    ; WRAM addr = $7Exxxx  (bank is relative to $7E)
	dw $1000|$420B, $02    ; Trigger DMA on channel 1

	; Copy WRAM 7E6000-7E7FFF to SRAM A46000-A47FFF.
	dw $0000|$4312, $6000  ; A addr = $xx0000
	dw $0000|$4314, $00A4  ; A addr = $71xxxx, size = $xx00
	dw $0000|$4316, $0020  ; size = $80xx ($8000), unused bank reg = $00.
	dw $0000|$2181, $6000  ; WRAM addr = $xx0000
	dw $1000|$2183, $00    ; WRAM addr = $7Exxxx  (bank is relative to $7E)
	dw $1000|$420B, $02    ; Trigger DMA on channel 1

	; Copy WRAM 7E8000-7E9FFF to SRAM A56000-A57FFF.
	dw $0000|$4312, $6000  ; A addr = $xx0000
	dw $0000|$4314, $00A5  ; A addr = $71xxxx, size = $xx00
	dw $0000|$4316, $0020  ; size = $80xx ($8000), unused bank reg = $00.
	dw $0000|$2181, $8000  ; WRAM addr = $xx0000
	dw $1000|$2183, $00    ; WRAM addr = $7Exxxx  (bank is relative to $7E)
	dw $1000|$420B, $02    ; Trigger DMA on channel 1

	; Copy WRAM 7EA000-7EBFFF to SRAM A66000-A67FFF.
	dw $0000|$4312, $6000  ; A addr = $xx0000
	dw $0000|$4314, $00A6  ; A addr = $71xxxx, size = $xx00
	dw $0000|$4316, $0020  ; size = $80xx ($8000), unused bank reg = $00.
	dw $0000|$2181, $A000  ; WRAM addr = $xx0000
	dw $1000|$2183, $00    ; WRAM addr = $7Exxxx  (bank is relative to $7E)
	dw $1000|$420B, $02    ; Trigger DMA on channel 1

	; Copy WRAM 7EC000-7EDFFF to SRAM A76000-A77FFF.
	dw $0000|$4312, $6000  ; A addr = $xx0000
	dw $0000|$4314, $00A7  ; A addr = $71xxxx, size = $xx00
	dw $0000|$4316, $0020  ; size = $80xx ($8000), unused bank reg = $00.
	dw $0000|$2181, $C000  ; WRAM addr = $xx0000
	dw $1000|$2183, $00    ; WRAM addr = $7Exxxx  (bank is relative to $7E)
	dw $1000|$420B, $02    ; Trigger DMA on channel 1

	; Copy WRAM 7EE000-7EFFFF to SRAM A86000-A87FFF.
	dw $0000|$4312, $6000  ; A addr = $xx0000
	dw $0000|$4314, $00A8  ; A addr = $71xxxx, size = $xx00
	dw $0000|$4316, $0020  ; size = $80xx ($8000), unused bank reg = $00.
	dw $0000|$2181, $E000  ; WRAM addr = $xx0000
	dw $1000|$2183, $00    ; WRAM addr = $7Exxxx  (bank is relative to $7E)
	dw $1000|$420B, $02    ; Trigger DMA on channel 1

	
	; Copy WRAM 7F0000-7F1FFF to SRAM A96000-A97FFF.
	dw $0000|$4312, $6000  ; A addr = $xx0000
	dw $0000|$4314, $00A9  ; A addr = $71xxxx, size = $xx00
	dw $0000|$4316, $0020  ; size = $80xx ($8000), unused bank reg = $00.
	dw $0000|$2181, $0000  ; WRAM addr = $xx0000
	dw $1000|$2183, $01    ; WRAM addr = $7Exxxx  (bank is relative to $7E)
	dw $1000|$420B, $02    ; Trigger DMA on channel 1

	; Copy WRAM 7F2000-7F3FFF to SRAM AA6000-AA7FFF.
	dw $0000|$4312, $6000  ; A addr = $xx0000
	dw $0000|$4314, $00AA  ; A addr = $71xxxx, size = $xx00
	dw $0000|$4316, $0020  ; size = $80xx ($8000), unused bank reg = $00.
	dw $0000|$2181, $2000  ; WRAM addr = $xx0000
	dw $1000|$2183, $01    ; WRAM addr = $7Exxxx  (bank is relative to $7E)
	dw $1000|$420B, $02    ; Trigger DMA on channel 1

	; Copy WRAM 7F4000-7F5FFF to SRAM AB6000-AB7FFF.
	dw $0000|$4312, $6000  ; A addr = $xx0000
	dw $0000|$4314, $00AB  ; A addr = $71xxxx, size = $xx00
	dw $0000|$4316, $0020  ; size = $80xx ($8000), unused bank reg = $00.
	dw $0000|$2181, $4000  ; WRAM addr = $xx0000
	dw $1000|$2183, $01    ; WRAM addr = $7Exxxx  (bank is relative to $7E)
	dw $1000|$420B, $02    ; Trigger DMA on channel 1

	; Copy WRAM 7F6000-7F7FFF to SRAM AC6000-AC7FFF.
	dw $0000|$4312, $6000  ; A addr = $xx0000
	dw $0000|$4314, $00AC  ; A addr = $71xxxx, size = $xx00
	dw $0000|$4316, $0020  ; size = $80xx ($8000), unused bank reg = $00.
	dw $0000|$2181, $6000  ; WRAM addr = $xx0000
	dw $1000|$2183, $01    ; WRAM addr = $7Exxxx  (bank is relative to $7E)
	dw $1000|$420B, $02    ; Trigger DMA on channel 1

	; Copy WRAM 7F8000-7F9FFF to SRAM AD6000-AD7FFF.
	dw $0000|$4312, $6000  ; A addr = $xx0000
	dw $0000|$4314, $00AD  ; A addr = $71xxxx, size = $xx00
	dw $0000|$4316, $0020  ; size = $80xx ($8000), unused bank reg = $00.
	dw $0000|$2181, $8000  ; WRAM addr = $xx0000
	dw $1000|$2183, $01    ; WRAM addr = $7Exxxx  (bank is relative to $7E)
	dw $1000|$420B, $02    ; Trigger DMA on channel 1

	; Copy WRAM 7FA000-7FBFFF to SRAM AE6000-AE7FFF.
	dw $0000|$4312, $6000  ; A addr = $xx0000
	dw $0000|$4314, $00AE  ; A addr = $71xxxx, size = $xx00
	dw $0000|$4316, $0020  ; size = $80xx ($8000), unused bank reg = $00.
	dw $0000|$2181, $A000  ; WRAM addr = $xx0000
	dw $1000|$2183, $01    ; WRAM addr = $7Exxxx  (bank is relative to $7E)
	dw $1000|$420B, $02    ; Trigger DMA on channel 1

	; Copy WRAM 7FC000-7FDFFF to SRAM AF6000-AF7FFF.
	dw $0000|$4312, $6000  ; A addr = $xx0000
	dw $0000|$4314, $00AF  ; A addr = $71xxxx, size = $xx00
	dw $0000|$4316, $0020  ; size = $80xx ($8000), unused bank reg = $00.
	dw $0000|$2181, $C000  ; WRAM addr = $xx0000
	dw $1000|$2183, $01    ; WRAM addr = $7Exxxx  (bank is relative to $7E)
	dw $1000|$420B, $02    ; Trigger DMA on channel 1

	; Copy WRAM 7FE000-7FFFFF to SRAM B06000-B07FFF.
	dw $0000|$4312, $6000  ; A addr = $xx0000
	dw $0000|$4314, $00BA  ; A addr = $71xxxx, size = $xx00
	dw $0000|$4316, $0020  ; size = $80xx ($8000), unused bank reg = $00.
	dw $0000|$2181, $E000  ; WRAM addr = $xx0000
	dw $1000|$2183, $01    ; WRAM addr = $7Exxxx  (bank is relative to $7E)
	dw $1000|$420B, $02    ; Trigger DMA on channel 1
	
	
	; Address pair, A bus -> B bus.  B address = VRAM write ($2118).
	dw $0000|$4310, $1801  ; direction = A->B, B addr = $2118
	dw $1000|$2115, $0000  ; VRAM address increment mode.
	
	; Copy VRAM 0000-1FFF to SRAM B16000-B17FFF.
	dw $0000|$2116, $0000  ; VRAM address >> 1.
	dw $0000|$4312, $6000  ; A addr = $xx0000
	dw $0000|$4314, $00B1  ; A addr = $75xxxx, size = $xx00
	dw $0000|$4316, $0020  ; size = $80xx ($0000), unused bank reg = $00.
	dw $1000|$420B, $02    ; Trigger DMA on channel 1

	; Copy VRAM 2000-3FFF to SRAM B26000-B27FFF.
	dw $0000|$2116, $1000  ; VRAM address >> 1.
	dw $0000|$4312, $6000  ; A addr = $xx0000
	dw $0000|$4314, $00B2  ; A addr = $75xxxx, size = $xx00
	dw $0000|$4316, $0020  ; size = $80xx ($0000), unused bank reg = $00.
	dw $1000|$420B, $02    ; Trigger DMA on channel 1

	; Copy VRAM 4000-5FFF to SRAM B36000-B37FFF.
	dw $0000|$2116, $2000  ; VRAM address >> 1.
	dw $0000|$4312, $6000  ; A addr = $xx0000
	dw $0000|$4314, $00B3  ; A addr = $75xxxx, size = $xx00
	dw $0000|$4316, $0020  ; size = $80xx ($0000), unused bank reg = $00.
	dw $1000|$420B, $02    ; Trigger DMA on channel 1

	; Copy VRAM 6000-7FFF to SRAM B46000-B47FFF.
	dw $0000|$2116, $3000  ; VRAM address >> 1.
	dw $0000|$4312, $6000  ; A addr = $xx0000
	dw $0000|$4314, $00B4  ; A addr = $75xxxx, size = $xx00
	dw $0000|$4316, $0020  ; size = $80xx ($0000), unused bank reg = $00.
	dw $1000|$420B, $02    ; Trigger DMA on channel 1
	
	; Copy VRAM 8000-9FFF to SRAM B56000-B57FFF.
	dw $0000|$2116, $4000  ; VRAM address >> 1.
	dw $0000|$4312, $6000  ; A addr = $xx0000
	dw $0000|$4314, $00B5  ; A addr = $75xxxx, size = $xx00
	dw $0000|$4316, $0020  ; size = $80xx ($0000), unused bank reg = $00.
	dw $1000|$420B, $02    ; Trigger DMA on channel 1

	; Copy VRAM A000-BFFF to SRAM B66000-B67FFF.
	dw $0000|$2116, $5000  ; VRAM address >> 1.
	dw $0000|$4312, $6000  ; A addr = $xx0000
	dw $0000|$4314, $00B6  ; A addr = $75xxxx, size = $xx00
	dw $0000|$4316, $0020  ; size = $80xx ($0000), unused bank reg = $00.
	dw $1000|$420B, $02    ; Trigger DMA on channel 1

	; Copy VRAM C000-DFFF to SRAM B76000-B77FFF.
	dw $0000|$2116, $6000  ; VRAM address >> 1.
	dw $0000|$4312, $6000  ; A addr = $xx0000
	dw $0000|$4314, $00B7  ; A addr = $75xxxx, size = $xx00
	dw $0000|$4316, $0020  ; size = $80xx ($0000), unused bank reg = $00.
	dw $1000|$420B, $02    ; Trigger DMA on channel 1

	; Copy VRAM E000-FFFF to SRAM B86000-B87FFF.
	dw $0000|$2116, $7000  ; VRAM address >> 1.
	dw $0000|$4312, $6000  ; A addr = $xx0000
	dw $0000|$4314, $00B8  ; A addr = $75xxxx, size = $xx00
	dw $0000|$4316, $0020  ; size = $80xx ($0000), unused bank reg = $00.
	dw $1000|$420B, $02    ; Trigger DMA on channel 1	
	
	; Copy SRAM B96000-B961FF to CGRAM 000-1FF.
	dw $1000|$2121, $00    ; CGRAM address
	dw $0000|$4310, $2200  ; direction = A->B, byte reg, B addr = $2122
	dw $0000|$4312, $6000  ; A addr = $xx2000
	dw $0000|$4314, $00B9  ; A addr = $77xxxx, size = $xx00
	dw $0000|$4316, $0002  ; size = $02xx ($0200), unused bank reg = $00.
	dw $1000|$420B, $02    ; Trigger DMA on channel 1
	; Done
	dw $0000, load_return

load_return:
	%ai16()
	lda !SRAM_SAVED_SP
	tas
	
	pea $0000
	plb
	plb
	
	; rewrite inputs so that holding load won't keep loading, as well as rewriting saving input to loading input
	lda !SS_INPUT_CUR
	eor !SS_INPUT_SAVE
	ora !SS_INPUT_LOAD
	sta !SS_INPUT_CUR
	sta !SS_INPUT_NEW
	sta !SS_INPUT_PREV
	
	%a8()
	ldx #$0000
	txy
	
load_dma_regs:
	lda !SRAM_DMA_BANK, x
	sta $4300, x
	inx
	iny
	cpy #$000B
	bne load_dma_regs
	cpx #$007B
	beq load_dma_regs_done
	inx #5
	ldy #$0000
	jmp load_dma_regs

load_dma_regs_done:
	; Restore registers and return.
	%ai16()
	jsr post_load_state
	jmp register_restore_return

vm:
	; Data format: xx xx yy yy
	; xxxx = little-endian address to write to .vm's bank
	; yyyy = little-endian value to write
	; If xxxx has high bit set, read and discard instead of write.
	; If xxxx has bit 12 set ($1000), byte instead of word.
	; If yyyy has $DD in the low half, it means that this operation is a byte
	; write instead of a word write.  If xxxx is $0000, end the VM.
	rep #$30
	; Read address to write to
	lda.l !SS_BANK, x
	beq vm_done
	tay
	inx
	inx
	; Check for byte mode
	bit.w #$1000
	beq vm_word_mode
	and.w #$EFFF
	tay
	sep #$20
vm_word_mode:
	; Read value
	lda.l !SS_BANK, x
	inx
	inx
vm_write:
	; Check for read mode (high bit of address)
	cpy.w #$8000
	bcs vm_read
	sta $0000, y
	bra vm
vm_read:
	; "Subtract" $8000 from y by taking advantage of bank wrapping.
	lda $8000, y
	bra vm

vm_done:
	; A, X and Y are 16-bit at exit.
	; Return to caller.  The word in the table after the terminator is the
	; code address to return to.
	jmp ($0002,x)
	
print "Savestate Bank Ending at: ", pc