#ifndef PROJECT_H
#define PROJECT_H

#include "asm/disasmhandler.h"
#include "mytype.h"
#include <QObject>

struct Label
{
    QString             name;
    snesAddress         offset;
    DisasmInstruction*  instruction;
};

struct BranchingInstruction
{
    snesAddress startOffset;
    snesAddress pointedOffset;
    QString     context;
    DisasmInstruction*  startInstruction;
    DisasmInstruction*  pointedInstruction = nullptr;
};


struct ASMRoutine
{
    QString     name;
    snesAddress offset;
    snesAddress endOffset;
    QString     comment;

    QMap<snesAddress, QString>                  localDefinition;
    QMap<snesAddress, Label*>                   localLabels;
    QList<DisasmInstruction*>                   instructions;
    QMap<snesAddress, BranchingInstruction*>    branchingInstructions;
};

class Project : public QObject
{
    Q_OBJECT
public:
    explicit Project(QObject *parent = nullptr);

signals:
};

#endif // PROJECT_H
