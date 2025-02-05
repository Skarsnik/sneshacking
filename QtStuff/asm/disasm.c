
#include "disasm.h"
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

extern const snes_asm_instruction_definition snes_asm_instuction_list[0x100];

snes_asm_instruction_node*    disassemble_str(const uint8_t* bytecode, size_t bytecode_length, uint32_t* pc, bool stop_on_return)
{
    bool accumulator_16 = true;
    bool register_16 = true;
    bool first = true;
    snes_asm_instruction_node* list_instruction = NULL;
    snes_asm_instruction_node* toret = NULL;

    size_t  pos = 0;
    while (pos < bytecode_length)
    {
        uint8_t opcode = bytecode[pos];

        snes_asm_instruction_node* new_node = (snes_asm_instruction_node*) malloc(sizeof(snes_asm_instruction_node));
        new_node->next = NULL;
        if (list_instruction == NULL && toret == NULL)
        {
            list_instruction = new_node;
            toret = new_node;
        }

        new_node->instruction.opcode = opcode;
        new_node->instruction.offset = *pc;

        memset(new_node->instruction.operand, 0, 3);
        const snes_asm_instruction_definition* def = &snes_asm_instuction_list[opcode];
        if (def->addressing != IMPLIED && pos + 1 == bytecode_length)
        {
            fprintf(stderr, "Error : reached the end of the data - operand missing for opcode %s\n", def->opcode);
            break;
        }
        new_node->instruction.addressing = def->addressing;
        uint8_t length_op = def->length;
        if (def->addressing == VALUE_A && accumulator_16)
            length_op++;
        if (def->addressing == VALUE_REGISTER && register_16)
            length_op++;
        if (opcode == 0xE2) // sep, set flag = 8 bits mode
        {
            if ((bytecode[pos + 1] & 0b00100000) == 0b00100000)
                accumulator_16 = false;
            if ((bytecode[pos + 1] & 0b00010000) == 0b00010000)
                register_16 = false;
            printf("Setting a flag (this set to 8 bit mode) : %08b - %d %d\n", bytecode[pos + 1], accumulator_16, register_16);
        }
        if (opcode == 0xC2) // rep
        {
            if ((bytecode[pos + 1] & 0b00100000) == 0b00100000)
                accumulator_16 = true;
            if ((bytecode[pos + 1] & 0b00010000) == 0b00010000)
                register_16 = true;
        }
        //printf("Opcode & lenght : %02X -  %d\n", opcode, length_op);
        pos++;
        unsigned int i = 0;
        for (; i < length_op - 1 && pos < bytecode_length; i++)
        {
            new_node->instruction.operand[i] = bytecode[pos];
            pos++;
        }
        new_node->instruction.length = length_op;
        *pc += length_op;
        if (i != length_op - 1 && pos == bytecode_length) // Part of the operand is missing
        {
            fprintf(stderr, "Error : reached the end of the data : part of the operand missing for %s\n", def->opcode);
            break;
        }
        //printf("Operand : %02X%02X%02X\n", new_node->instruction.operand[0], new_node->instruction.operand[1], new_node->instruction.operand[2]);
        if (!first)
        {
            list_instruction->next = new_node;
            list_instruction = new_node;
        } else {
            first = false;
        }
        if ((opcode == 0x6B || opcode == 0x60 || opcode == 0x40) && stop_on_return)
        {
            break;
        }
    }
    list_instruction->next = NULL;
    return toret;
}
