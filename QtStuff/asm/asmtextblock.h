#ifndef ASMTEXTBLOCK_H
#define ASMTEXTBLOCK_H

#include <QTextBlock>
#include "asmline.h"

class ASMTextBlock : public QTextBlock
{
public:
    ASMTextBlock();
    void    setAsmLine(ASMLine asmLine);
    QString text() const;


private:
    ASMLine line;
};

#endif // ASMTEXTBLOCK_H
