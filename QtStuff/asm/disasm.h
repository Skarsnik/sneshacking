#ifndef DISASM_H
#define DISASM_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

enum snes_asm_simplified_addressing {
    ACCUMULATOR,
    IMPLIED,
    VALUE,
    VALUE_A,
    VALUE_REGISTER,
    VALUE_BYTE,
    VALUE_WORD,
    RELATIVE,
    RELATIVE_LONG,
    ADDRESS,
    ADDRESS_INDEXED_X,
    ADDRESS_INDEXED_Y,
    ADDRESS_INDIRECT,
    ADDRESS_INDEXED_INDIRECT,
    ADDRESS_INDIRECT_INDEXED,
    ADDRESS_INDIRECT_LONG,
    ADDRESS_INDIRECT_INDEXED_LONG,
    ADDRESS_WORD,
    ADDRESS_WORD_INDEXED_X,
    ADDRESS_WORD_INDEXED_Y,
    ADDRESS_WORD_INDIRECT,
    ADDRESS_WORD_INDEXED_INDIRECT,
    ADDRESS_LONG,
    ADDRESS_LONG_INDEXED,
    ADDRESS_LONG_INDIRECT,
    STACK_RELATIVE,
    STACK_RELATIVE_INDIRECT_INDEXED,
    BLOCK_MOVE,
    PC_RELATIVE,
    PC_RELATIVE_WORD,
};

typedef struct  {
    char*       opcode;
    uint8_t     length;
    enum snes_asm_simplified_addressing addressing;
} snes_asm_instruction_definition;

typedef struct {
    uint32_t    offset;
    uint8_t     opcode;
    uint8_t     operand[3]; // In the original byte code order
    uint8_t     length; // Total length
    enum snes_asm_simplified_addressing addressing;
} snes_asm_instruction;

typedef struct snes_asm_instruction_node_s {
    snes_asm_instruction                    instruction;
    struct snes_asm_instruction_node_s*     next;
} snes_asm_instruction_node;


snes_asm_instruction_node*    disassemble_str(const uint8_t* bytecode, size_t bytecode_length, uint32_t* pc, bool stop_on_return);


#define S_INSTR_ADC_ADDRESS 101
#define S_INSTR_ADC_ADDRESS_INDEXED_INDIRECT 97
#define S_INSTR_ADC_ADDRESS_INDEXED_X 117
#define S_INSTR_ADC_ADDRESS_INDIRECT 114
#define S_INSTR_ADC_ADDRESS_INDIRECT_INDEXED 113
#define S_INSTR_ADC_ADDRESS_INDIRECT_INDEXED_LONG 119
#define S_INSTR_ADC_ADDRESS_INDIRECT_LONG 103
#define S_INSTR_ADC_ADDRESS_LONG 111
#define S_INSTR_ADC_ADDRESS_LONG_INDEXED 127
#define S_INSTR_ADC_ADDRESS_WORD 109
#define S_INSTR_ADC_ADDRESS_WORD_INDEXED_X 125
#define S_INSTR_ADC_ADDRESS_WORD_INDEXED_Y 121
#define S_INSTR_ADC_STACK_RELATIVE 99
#define S_INSTR_ADC_STACK_RELATIVE_INDIRECT_INDEXED 115
#define S_INSTR_ADC_VALUE_A 105
#define S_INSTR_AND_ADDRESS 37
#define S_INSTR_AND_ADDRESS_INDEXED_INDIRECT 33
#define S_INSTR_AND_ADDRESS_INDEXED_X 53
#define S_INSTR_AND_ADDRESS_INDIRECT 50
#define S_INSTR_AND_ADDRESS_INDIRECT_INDEXED 49
#define S_INSTR_AND_ADDRESS_INDIRECT_INDEXED_LONG 55
#define S_INSTR_AND_ADDRESS_INDIRECT_LONG 39
#define S_INSTR_AND_ADDRESS_LONG 47
#define S_INSTR_AND_ADDRESS_LONG_INDEXED 63
#define S_INSTR_AND_ADDRESS_WORD 45
#define S_INSTR_AND_ADDRESS_WORD_INDEXED_X 61
#define S_INSTR_AND_ADDRESS_WORD_INDEXED_Y 57
#define S_INSTR_AND_STACK_RELATIVE 35
#define S_INSTR_AND_STACK_RELATIVE_INDIRECT_INDEXED 51
#define S_INSTR_AND_VALUE_A 41
#define S_INSTR_ASL_ACCUMULATOR 10
#define S_INSTR_ASL_ADDRESS 6
#define S_INSTR_ASL_ADDRESS_INDEXED_X 22
#define S_INSTR_ASL_ADDRESS_WORD 14
#define S_INSTR_ASL_ADDRESS_WORD_INDEXED_X 30
#define S_INSTR_BCC_PC_RELATIVE 144
#define S_INSTR_BCS_PC_RELATIVE 176
#define S_INSTR_BEQ_PC_RELATIVE 240
#define S_INSTR_BIT_ADDRESS 36
#define S_INSTR_BIT_ADDRESS_INDEXED_X 52
#define S_INSTR_BIT_ADDRESS_WORD 44
#define S_INSTR_BIT_ADDRESS_WORD_INDEXED_X 60
#define S_INSTR_BIT_VALUE_A 137
#define S_INSTR_BMI_PC_RELATIVE 48
#define S_INSTR_BNE_PC_RELATIVE 208
#define S_INSTR_BPL_PC_RELATIVE 16
#define S_INSTR_BRA_PC_RELATIVE 128
#define S_INSTR_BRK_ADDRESS 0
#define S_INSTR_BRL_PC_RELATIVE_WORD 130
#define S_INSTR_BVC_PC_RELATIVE 80
#define S_INSTR_BVS_PC_RELATIVE 112
#define S_INSTR_CLC_IMPLIED 24
#define S_INSTR_CLD_IMPLIED 216
#define S_INSTR_CLI_IMPLIED 88
#define S_INSTR_CLV_IMPLIED 184
#define S_INSTR_CMP_ADDRESS 197
#define S_INSTR_CMP_ADDRESS_INDEXED_INDIRECT 193
#define S_INSTR_CMP_ADDRESS_INDEXED_X 213
#define S_INSTR_CMP_ADDRESS_INDIRECT 210
#define S_INSTR_CMP_ADDRESS_INDIRECT_INDEXED 209
#define S_INSTR_CMP_ADDRESS_INDIRECT_INDEXED_LONG 215
#define S_INSTR_CMP_ADDRESS_INDIRECT_LONG 199
#define S_INSTR_CMP_ADDRESS_LONG 207
#define S_INSTR_CMP_ADDRESS_LONG_INDEXED 223
#define S_INSTR_CMP_ADDRESS_WORD 205
#define S_INSTR_CMP_ADDRESS_WORD_INDEXED_X 221
#define S_INSTR_CMP_ADDRESS_WORD_INDEXED_Y 217
#define S_INSTR_CMP_STACK_RELATIVE 195
#define S_INSTR_CMP_STACK_RELATIVE_INDIRECT_INDEXED 211
#define S_INSTR_CMP_VALUE_A 201
#define S_INSTR_COP_ADDRESS 2
#define S_INSTR_CPX_ADDRESS 228
#define S_INSTR_CPX_ADDRESS_WORD 236
#define S_INSTR_CPX_VALUE_REGISTER 224
#define S_INSTR_CPY_ADDRESS 196
#define S_INSTR_CPY_ADDRESS_WORD 204
#define S_INSTR_CPY_VALUE_REGISTER 192
#define S_INSTR_DEC_ACCUMULATOR 58
#define S_INSTR_DEC_ADDRESS 198
#define S_INSTR_DEC_ADDRESS_INDEXED_X 214
#define S_INSTR_DEC_ADDRESS_WORD 206
#define S_INSTR_DEC_ADDRESS_WORD_INDEXED_X 222
#define S_INSTR_DEX_IMPLIED 202
#define S_INSTR_DEY_IMPLIED 136
#define S_INSTR_EOR_ADDRESS 69
#define S_INSTR_EOR_ADDRESS_INDEXED_INDIRECT 65
#define S_INSTR_EOR_ADDRESS_INDEXED_X 85
#define S_INSTR_EOR_ADDRESS_INDIRECT 82
#define S_INSTR_EOR_ADDRESS_INDIRECT_INDEXED 81
#define S_INSTR_EOR_ADDRESS_INDIRECT_INDEXED_LONG 87
#define S_INSTR_EOR_ADDRESS_INDIRECT_LONG 71
#define S_INSTR_EOR_ADDRESS_LONG 79
#define S_INSTR_EOR_ADDRESS_LONG_INDEXED 95
#define S_INSTR_EOR_ADDRESS_WORD 77
#define S_INSTR_EOR_ADDRESS_WORD_INDEXED_X 93
#define S_INSTR_EOR_ADDRESS_WORD_INDEXED_Y 89
#define S_INSTR_EOR_STACK_RELATIVE 67
#define S_INSTR_EOR_STACK_RELATIVE_INDIRECT_INDEXED 83
#define S_INSTR_EOR_VALUE_A 73
#define S_INSTR_INC_ACCUMULATOR 26
#define S_INSTR_INC_ADDRESS 230
#define S_INSTR_INC_ADDRESS_INDEXED_X 246
#define S_INSTR_INC_ADDRESS_WORD 238
#define S_INSTR_INC_ADDRESS_WORD_INDEXED_X 254
#define S_INSTR_INX_IMPLIED 232
#define S_INSTR_INY_IMPLIED 200
#define S_INSTR_JMP_ADDRESS_LONG 92
#define S_INSTR_JMP_ADDRESS_LONG_INDIRECT 220
#define S_INSTR_JMP_ADDRESS_WORD 76
#define S_INSTR_JMP_ADDRESS_WORD_INDEXED_INDIRECT 124
#define S_INSTR_JMP_ADDRESS_WORD_INDIRECT 108
#define S_INSTR_JSR_ADDRESS_LONG 34
#define S_INSTR_JSR_ADDRESS_WORD 32
#define S_INSTR_JSR_ADDRESS_WORD_INDEXED_INDIRECT 252
#define S_INSTR_LDA_ADDRESS 165
#define S_INSTR_LDA_ADDRESS_INDEXED_INDIRECT 161
#define S_INSTR_LDA_ADDRESS_INDEXED_X 181
#define S_INSTR_LDA_ADDRESS_INDIRECT 178
#define S_INSTR_LDA_ADDRESS_INDIRECT_INDEXED 177
#define S_INSTR_LDA_ADDRESS_INDIRECT_INDEXED_LONG 183
#define S_INSTR_LDA_ADDRESS_INDIRECT_LONG 167
#define S_INSTR_LDA_ADDRESS_LONG 175
#define S_INSTR_LDA_ADDRESS_LONG_INDEXED 191
#define S_INSTR_LDA_ADDRESS_WORD 173
#define S_INSTR_LDA_ADDRESS_WORD_INDEXED_X 189
#define S_INSTR_LDA_ADDRESS_WORD_INDEXED_Y 185
#define S_INSTR_LDA_STACK_RELATIVE 163
#define S_INSTR_LDA_STACK_RELATIVE_INDIRECT_INDEXED 179
#define S_INSTR_LDA_VALUE_A 169
#define S_INSTR_LDX_ADDRESS 166
#define S_INSTR_LDX_ADDRESS_INDEXED_Y 182
#define S_INSTR_LDX_ADDRESS_WORD 174
#define S_INSTR_LDX_ADDRESS_WORD_INDEXED_Y 190
#define S_INSTR_LDX_VALUE_REGISTER 162
#define S_INSTR_LDY_ADDRESS 164
#define S_INSTR_LDY_ADDRESS_INDEXED_X 180
#define S_INSTR_LDY_ADDRESS_WORD 172
#define S_INSTR_LDY_ADDRESS_WORD_INDEXED_X 188
#define S_INSTR_LDY_VALUE_REGISTER 160
#define S_INSTR_LSR_ACCUMULATOR 74
#define S_INSTR_LSR_ADDRESS 70
#define S_INSTR_LSR_ADDRESS_INDEXED_X 86
#define S_INSTR_LSR_ADDRESS_WORD 78
#define S_INSTR_LSR_ADDRESS_WORD_INDEXED_X 94
#define S_INSTR_MVN_BLOCK_MOVE 84
#define S_INSTR_MVP_BLOCK_MOVE 68
#define S_INSTR_NOP_IMPLIED 234
#define S_INSTR_ORA_ADDRESS 5
#define S_INSTR_ORA_ADDRESS_INDEXED_INDIRECT 1
#define S_INSTR_ORA_ADDRESS_INDEXED_X 21
#define S_INSTR_ORA_ADDRESS_INDIRECT 18
#define S_INSTR_ORA_ADDRESS_INDIRECT_INDEXED 17
#define S_INSTR_ORA_ADDRESS_INDIRECT_INDEXED_LONG 23
#define S_INSTR_ORA_ADDRESS_INDIRECT_LONG 7
#define S_INSTR_ORA_ADDRESS_LONG 15
#define S_INSTR_ORA_ADDRESS_LONG_INDEXED 31
#define S_INSTR_ORA_ADDRESS_WORD 13
#define S_INSTR_ORA_ADDRESS_WORD_INDEXED_X 29
#define S_INSTR_ORA_ADDRESS_WORD_INDEXED_Y 25
#define S_INSTR_ORA_STACK_RELATIVE 3
#define S_INSTR_ORA_STACK_RELATIVE_INDIRECT_INDEXED 19
#define S_INSTR_ORA_VALUE_A 9
#define S_INSTR_PEA_ADDRESS_WORD 244
#define S_INSTR_PEI_ADDRESS_INDIRECT 212
#define S_INSTR_PER_PC_RELATIVE_WORD 98
#define S_INSTR_PHA_IMPLIED 72
#define S_INSTR_PHB_IMPLIED 139
#define S_INSTR_PHD_IMPLIED 11
#define S_INSTR_PHK_IMPLIED 75
#define S_INSTR_PHP_IMPLIED 8
#define S_INSTR_PHX_IMPLIED 218
#define S_INSTR_PHY_IMPLIED 90
#define S_INSTR_PLA_IMPLIED 104
#define S_INSTR_PLB_IMPLIED 171
#define S_INSTR_PLD_IMPLIED 43
#define S_INSTR_PLP_IMPLIED 40
#define S_INSTR_PLX_IMPLIED 250
#define S_INSTR_PLY_IMPLIED 122
#define S_INSTR_REP_VALUE_BYTE 194
#define S_INSTR_ROL_ACCUMULATOR 42
#define S_INSTR_ROL_ADDRESS 38
#define S_INSTR_ROL_ADDRESS_INDEXED_X 54
#define S_INSTR_ROL_ADDRESS_WORD 46
#define S_INSTR_ROL_ADDRESS_WORD_INDEXED_X 62
#define S_INSTR_ROR_ACCUMULATOR 106
#define S_INSTR_ROR_ADDRESS 102
#define S_INSTR_ROR_ADDRESS_INDEXED_X 118
#define S_INSTR_ROR_ADDRESS_WORD 110
#define S_INSTR_ROR_ADDRESS_WORD_INDEXED_X 126
#define S_INSTR_RTI_IMPLIED 64
#define S_INSTR_RTL_IMPLIED 107
#define S_INSTR_RTS_IMPLIED 96
#define S_INSTR_SBC_ADDRESS 229
#define S_INSTR_SBC_ADDRESS_INDEXED_INDIRECT 225
#define S_INSTR_SBC_ADDRESS_INDEXED_X 245
#define S_INSTR_SBC_ADDRESS_INDIRECT 242
#define S_INSTR_SBC_ADDRESS_INDIRECT_INDEXED 241
#define S_INSTR_SBC_ADDRESS_INDIRECT_INDEXED_LONG 247
#define S_INSTR_SBC_ADDRESS_INDIRECT_LONG 231
#define S_INSTR_SBC_ADDRESS_LONG 239
#define S_INSTR_SBC_ADDRESS_LONG_INDEXED 255
#define S_INSTR_SBC_ADDRESS_WORD 237
#define S_INSTR_SBC_ADDRESS_WORD_INDEXED_X 253
#define S_INSTR_SBC_ADDRESS_WORD_INDEXED_Y 249
#define S_INSTR_SBC_STACK_RELATIVE 227
#define S_INSTR_SBC_STACK_RELATIVE_INDIRECT_INDEXED 243
#define S_INSTR_SBC_VALUE_A 233
#define S_INSTR_SEC_IMPLIED 56
#define S_INSTR_SED_IMPLIED 248
#define S_INSTR_SEI_IMPLIED 120
#define S_INSTR_SEP_VALUE_BYTE 226
#define S_INSTR_STA_ADDRESS 133
#define S_INSTR_STA_ADDRESS_INDEXED_INDIRECT 129
#define S_INSTR_STA_ADDRESS_INDEXED_X 149
#define S_INSTR_STA_ADDRESS_INDIRECT 146
#define S_INSTR_STA_ADDRESS_INDIRECT_INDEXED 145
#define S_INSTR_STA_ADDRESS_INDIRECT_INDEXED_LONG 151
#define S_INSTR_STA_ADDRESS_INDIRECT_LONG 135
#define S_INSTR_STA_ADDRESS_LONG 143
#define S_INSTR_STA_ADDRESS_LONG_INDEXED 159
#define S_INSTR_STA_ADDRESS_WORD 141
#define S_INSTR_STA_ADDRESS_WORD_INDEXED_X 157
#define S_INSTR_STA_ADDRESS_WORD_INDEXED_Y 153
#define S_INSTR_STA_STACK_RELATIVE 131
#define S_INSTR_STA_STACK_RELATIVE_INDIRECT_INDEXED 147
#define S_INSTR_STP_IMPLIED 219
#define S_INSTR_STX_ADDRESS 134
#define S_INSTR_STX_ADDRESS_INDEXED_Y 150
#define S_INSTR_STX_ADDRESS_WORD 142
#define S_INSTR_STY_ADDRESS 132
#define S_INSTR_STY_ADDRESS_INDEXED_X 148
#define S_INSTR_STY_ADDRESS_WORD 140
#define S_INSTR_STZ_ADDRESS 100
#define S_INSTR_STZ_ADDRESS_INDEXED_X 116
#define S_INSTR_STZ_ADDRESS_WORD 156
#define S_INSTR_STZ_ADDRESS_WORD_INDEXED_X 158
#define S_INSTR_TAX_IMPLIED 170
#define S_INSTR_TAY_IMPLIED 168
#define S_INSTR_TCD_IMPLIED 91
#define S_INSTR_TCS_IMPLIED 27
#define S_INSTR_TDC_IMPLIED 123
#define S_INSTR_TRB_ADDRESS 20
#define S_INSTR_TRB_ADDRESS_WORD 28
#define S_INSTR_TSB_ADDRESS 4
#define S_INSTR_TSB_ADDRESS_WORD 12
#define S_INSTR_TSC_IMPLIED 59
#define S_INSTR_TSX_IMPLIED 186
#define S_INSTR_TXA_IMPLIED 138
#define S_INSTR_TXS_IMPLIED 154
#define S_INSTR_TXY_IMPLIED 155
#define S_INSTR_TYA_IMPLIED 152
#define S_INSTR_TYX_IMPLIED 187
#define S_INSTR_WAI_IMPLIED 203
#define S_INSTR_XBA_IMPLIED 235
#define S_INSTR_XCE_IMPLIED 251


#ifdef __cplusplus
}
#endif
#endif // DISASM_H
