#ifndef DISASMHANDLER_H
#define DISASMHANDLER_H

#include <QString>
#include "qglobal.h"
#include "disasm.h"

struct DisasmInstruction
{
    DisasmInstruction() {};
    DisasmInstruction(const snes_asm_instruction &instruction);
    quint8  opcode;
    quint32 offset;
    quint8  length;
    quint8  operand[3];
    QString basicStrinfigy;
    QString advancedStrinfigy;

    enum snes_asm_simplified_addressing addressing;
};

class DisasmHandler
{
public:
    DisasmHandler();
};

#endif // DISASMHANDLER_H
