#pragma once

#include <QMap>
#include <QString>
#include "qglobal.h"
#include "disasm.h"
#include "rom.h"
#include "mytype.h"

struct DisasmInstruction
{
    DisasmInstruction() {};
    DisasmInstruction(const snes_asm_instruction &instruction);
    quint8      opcode;
    snesAddress offset;
    quint8      length;
    quint8      operand[3];
    QString     basicStrinfigy;
    QString     advancedStrinfigy;

    enum snes_asm_simplified_addressing addressing;
};

struct Branching
{
    snesAddress startOffset;
    snesAddress pointedOffset;
    QString context;
    DisasmInstruction*  startInstruction;
    DisasmInstruction*  pointedInstruction = nullptr;
};

struct DisasmThing
{
    QList<DisasmInstruction*>       instructions;
    QMap<snesAddress, Branching>    localBranchings;
    QMap<snesAddress, Branching>    globalBranchings;
    snesAddress                     endOffset;
};

class DisasmHandler
{
public:
    DisasmHandler() {};
    DisasmThing    disassembleRoutine(snesAddress pc);
    DisasmThing    disassemble(snesAddress pc, quint32 length);

    Rom*    rom;

private:
    DisasmThing buildStuff(snes_asm_instruction_node* instructions);
};

