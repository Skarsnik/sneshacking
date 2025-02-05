#pragma once

#include <QString>
#include "qglobal.h"

// C0/9173:	BEA596  	ldx $96A5,Y
// C0/9176:	B92797  	lda $9727,Y
// C0/9179:	9D2797  	sta $9727,X

struct ASMLine {
    quint32     offset;
    quint8      opcode;
    quint32     operand;
    QString     instruction;
    QString     operandText;
};
