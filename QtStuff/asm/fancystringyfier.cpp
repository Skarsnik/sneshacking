#include "disasm.h"
#include "mglobal.h"
#include "fancystringyfier.h"

extern "C" const snes_asm_instruction_definition snes_asm_instuction_list[0x100];

FancyStringyfier::FancyStringyfier(DefinitionHandler& defh) : definitions(defh) {};

void FancyStringyfier::fancier(DisasmInstruction &instr)
{
    const quint32 operand = instr.operandValue;

    // Operation that operate at bit level, bit, and, ora, sep, rep...
    if (   instr.opcode == S_INSTR_AND_VALUE_A
        || instr.opcode == S_INSTR_ORA_VALUE_A
        || instr.opcode == S_INSTR_BIT_VALUE_A)
    {
        instr.advancedStrinfigy = QString("%1 b%2").arg(snes_asm_instuction_list[instr.opcode].opcode, sBit(operand, instr.length - 1));
    }
    const char* flags = "nvaxdizc";
    if (instr.opcode == S_INSTR_SEP_VALUE_BYTE)
    {
        instr.advancedStrinfigy = "sep P ";
        for (unsigned int i = 0; i < 8; i++)
        {
            if (i == 2 && (operand & 0b00100000) == 0b00100000)
            {
                instr.advancedStrinfigy += "A";
            } else {
                if ((operand & (0b10000000 >> i)) == 0b10000000 >> i)
                    instr.advancedStrinfigy += flags[i];
                else
                    instr.advancedStrinfigy += "-";
            }
        }
    }
    if (instr.opcode == S_INSTR_REP_VALUE_BYTE)
    {
        instr.advancedStrinfigy = "rep P ";
        for (unsigned int i = 0; i < 8; i++)
        {
            if ((operand & (0b10000000 >> i)) == 0b10000000 >> i)
                instr.advancedStrinfigy += QString(flags[i]).toUpper();
            else
                instr.advancedStrinfigy += "-";
        }
    }
    //TODO could maybe merge this in OperandString in disasmhandler.cpp
    if (instr.addressing >= RELATIVE && instr.addressing <= ADDRESS_LONG)
    {
        qDebug() << "Checking for operand name" << instr.basicStrinfigy << sHex(operand, 3);
        auto where = definitions.whereIs(snesAddress(operand));
        QString operandName = "";
        switch (where)
        {
        case AddressLocation::WRAM:
        {
            operandName = definitions.wramMapEntry(snesAddress(operand)).name;
            break;
        }
        case AddressLocation::REGISTER:
        {
            operandName = definitions.snesRegister(snesAddress(operand)).name;
        }
        default:
            ;
        }
        qDebug() << "Operand name" << operandName;
        if (operandName == "")
            return ;
        instr.advancedStrinfigy = snes_asm_instuction_list[instr.opcode].opcode;
        instr.advancedStrinfigy += " ";
        switch(instr.addressing)
        {
            case RELATIVE_LONG:
            case ADDRESS_WORD_INDEXED_X:
            case ADDRESS_LONG_INDEXED:
            case ADDRESS_INDEXED_X:
                instr.advancedStrinfigy += QString("%1, x").arg(operandName);
            case ADDRESS_WORD_INDEXED_Y:
            case ADDRESS_INDEXED_Y:
                instr.advancedStrinfigy += QString("%1, y").arg(operandName);
            case ADDRESS_WORD_INDIRECT:
            case ADDRESS_INDIRECT:
            case ADDRESS_LONG_INDIRECT:
                instr.advancedStrinfigy += QString("(%1)").arg(operandName);
            case ADDRESS_WORD_INDEXED_INDIRECT:
            case ADDRESS_INDEXED_INDIRECT:
                instr.advancedStrinfigy += QString("(%1, x)").arg(operandName);
            case ADDRESS_INDIRECT_INDEXED:
                instr.advancedStrinfigy += QString("(%1), x").arg(operandName);
            case ADDRESS_INDIRECT_LONG:
                instr.advancedStrinfigy += QString("[%1]").arg(operandName);
            case ADDRESS_INDIRECT_INDEXED_LONG:
                instr.advancedStrinfigy += QString("[%1], x").arg(operandName);
            case ADDRESS:
            case ADDRESS_WORD:
            case ADDRESS_LONG:
                instr.advancedStrinfigy += operandName;
            default:
                break;
        }
    }
}
