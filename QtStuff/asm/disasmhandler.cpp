#include "disasmhandler.h"
#include "mglobal.h"

extern "C" const snes_asm_instruction_definition snes_asm_instuction_list[0x100];


inline QString operandString(const snes_asm_instruction& instruction, quint32 operand, quint32 pc)
{
    switch (instruction.addressing)
    {
    case ACCUMULATOR:
        return "A";
    case IMPLIED:
        return "";
    case VALUE:
        return "";
    case VALUE_A:
    case VALUE_REGISTER:
    case VALUE_BYTE:
    case VALUE_WORD:
        return QString("#$%1").arg(sHex(operand, instruction.length - 1));
    case RELATIVE:
    case RELATIVE_LONG:
    case ADDRESS_WORD_INDEXED_X:
    case ADDRESS_LONG_INDEXED:
    case ADDRESS_INDEXED_X:
        return QString("$%1, x").arg(sHex(operand, instruction.length - 1));
    case ADDRESS_WORD_INDEXED_Y:
    case ADDRESS_INDEXED_Y:
        return QString("$%1, y").arg(sHex(operand, instruction.length - 1));
    case ADDRESS_WORD_INDIRECT:
    case ADDRESS_INDIRECT:
    case ADDRESS_LONG_INDIRECT:
        return QString("($%1)").arg(sHex(operand, instruction.length - 1));
    case ADDRESS_WORD_INDEXED_INDIRECT:
    case ADDRESS_INDEXED_INDIRECT:
        return QString("($%1, x)").arg(sHex(operand, instruction.length - 1));
    case ADDRESS_INDIRECT_INDEXED:
        return QString("($%1), x").arg(sHex(operand, instruction.length - 1));
    case ADDRESS_INDIRECT_LONG:
        return QString("[$%1]").arg(sHex(operand, instruction.length - 1));
    case ADDRESS_INDIRECT_INDEXED_LONG:
        return QString("[$%1], x").arg(sHex(operand, instruction.length - 1));
    case ADDRESS:
    case ADDRESS_WORD:
    case ADDRESS_LONG:
        return QString("$%1").arg(sHex(operand, instruction.length - 1));
    case STACK_RELATIVE:
        return QString("$%1, S").arg(sHex(operand, instruction.length - 1));
    case STACK_RELATIVE_INDIRECT_INDEXED:
        return QString("($%1, S), y").arg(sHex(operand, instruction.length - 1));
    case BLOCK_MOVE:
        return QString("$%1, %2").arg(sHex(operand >> 8, 1), sHex(operand & 0x00FF, 1));
    case PC_RELATIVE: // offset + signed value
    {
        qint8 signedOperand = operand > 127 ? operand - 256 : operand;
        return QString("%1").arg(sHex((pc + signedOperand + 2) & 0x00FFFF, 1) );
    }
    case PC_RELATIVE_WORD:
        return QString("%1").arg(sHex((pc + (qint16) operand + 2) & 0x00FFFF, 2));
    }
    return "LOLERROR";
}


DisasmInstruction::DisasmInstruction(const snes_asm_instruction& instruction)
{
    opcode = instruction.opcode;
    offset = snesAddress(instruction.offset);
    length = instruction.length;
    operand[0] = instruction.operand[0];
    operand[1] = instruction.operand[1];
    operand[2] = instruction.operand[2];
    addressing = instruction.addressing;

    quint32 operand = instruction.operand[0] + instruction.operand[1] * 0x100 + instruction.operand[2] * 0x10000;
    basicStrinfigy = QString("%1 %2")
                                .arg(snes_asm_instuction_list[instruction.opcode].opcode)
                                .arg(operandString(instruction, operand, instruction.offset));
    // Operation that operate at bit level, bit, and, ora, sep, rep...
    if (   opcode == S_INSTR_AND_VALUE_A
        || opcode == S_INSTR_ORA_VALUE_A
        || opcode == S_INSTR_BIT_VALUE_A)
    {
        advancedStrinfigy = QString("%1 b%2").arg(snes_asm_instuction_list[instruction.opcode].opcode, sBit(operand, length - 1));
    }
    const char* flags = "nvmxdizc";
    if (opcode == S_INSTR_SEP_VALUE_BYTE)
    {
        advancedStrinfigy = "sep P ";
        for (unsigned int i = 0; i < 8; i++)
        {
            if (i == 2 && (operand & 0b00100000) == 0b00100000)
            {
                advancedStrinfigy += "m¹⁶";
                qDebug() << "PIKOOO";
            } else {
            if ((operand & (0b10000000 >> i)) == 0b10000000 >> i)
                advancedStrinfigy += flags[i];
            else
                advancedStrinfigy += "-";
            }
        }
    }
    if (opcode == S_INSTR_REP_VALUE_BYTE)
    {
        advancedStrinfigy = "rep P ";
        for (unsigned int i = 0; i < 8; i++)
        {
            if ((operand & (0b10000000 >> i)) == 0b10000000 >> i)
                advancedStrinfigy += QString(flags[i]).toUpper();
            else
                advancedStrinfigy += "-";
        }
    }
}

DisasmThing DisasmHandler::disassembleRoutine(snesAddress pc)
{
    quint32 mPc = pc;
    auto* instructions = disassemble_str((const uint8_t*)rom->datas(pc, 0x1000).constData(), 0x1000, &mPc, true);
    auto plop = buildStuff(instructions);
    plop.endOffset = snesAddress(mPc);
    return plop;
}

DisasmThing DisasmHandler::buildStuff(snes_asm_instruction_node *instructions)
{
    DisasmThing toret;
    auto list = instructions;
    DisasmInstruction* prevInstr = nullptr;
    while (list)
    {
        auto instruction = new DisasmInstruction(list->instruction);
        toret.instructions.append(new DisasmInstruction(list->instruction));
        quint32 operand = instruction->operand[0] + instruction->operand[1] * 0x100 + instruction->operand[2] * 0x10000;
        if (instruction->addressing == PC_RELATIVE || instruction->addressing == PC_RELATIVE_WORD)
        {
            Branching bs;
            bs.startOffset = instruction->offset;
            qint16 pcRel = instruction->addressing == PC_RELATIVE ? (qint8) operand : (qint16) operand;
            bs.pointedOffset = snesAddress(instruction->offset + pcRel + 2);
            bs.startInstruction = instruction;
            bs.context = instruction->basicStrinfigy;
            toret.localBranchings[instruction->offset] = bs;
        }
        if (instruction->opcode == S_INSTR_JMP_ADDRESS_WORD) // FIXME this can be out of routine
        {
            Branching bs;
            bs.startOffset = instruction->offset;
            bs.pointedOffset = snesAddress((instruction->offset >> 16) + operand);
            bs.startInstruction = instruction;
            toret.localBranchings[instruction->offset] = bs;
        }
        if (instruction->opcode == S_INSTR_JMP_ADDRESS_LONG)
        {
            Branching bs;
            bs.startOffset = instruction->offset;
            bs.pointedOffset = snesAddress((instruction->offset >> 16) + operand);
            bs.startInstruction = instruction;
            toret.globalBranchings[instruction->offset] = bs;
        }
        prevInstr = instruction;
        list = list->next;
    }
    return toret;
}
