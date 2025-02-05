#include "asmtextblock.h"

ASMTextBlock::ASMTextBlock() {}

void ASMTextBlock::setAsmLine(ASMLine asmLine)
{
    line = asmLine;
}

// BKHHHH   OP OPERAND   : lda $F2

QString ASMTextBlock::text() const
{
    return QString("%1%2    %3 %4  : %5 %6")
        .arg(line.offset >> 16, 2, 16, QChar('0'))
        .arg(line.offset & 0x00FFFF, 4, 16, QChar('0'))
        .arg(line.opcode, 2, 16, QChar('0'))
        .arg(line.operand, 4, 16, QChar('0'))
        .arg(line.instruction)
        .arg(line.operandText);
}
