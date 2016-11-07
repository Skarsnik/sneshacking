header

hirom

incsrc macros.asm
incsrc soepractice.asm


;8084a3 and $0104 size 3
;8084a6 sta $0104 size 3
org $C084a3
  JSL start_practice_stuff ; size 4
  NOP ; size 1
  NOP ; size 1