#ifndef ASMTEXTVIEW_H
#define ASMTEXTVIEW_H

#include <QPlainTextEdit>
//#include "asm/asmtextviewsidebar.h"
#include "asm/disasmhandler.h"

enum class LineContentType
{
    Instruction,
    LocalLabel,
    EmptyLine
};

struct Label
{
    QString name;
    quint32 offset;
};

struct LineContent
{
    LineContentType         type;
    DisasmInstruction       *instruction;
    Label                   label;
};

class ASMTextView : public QPlainTextEdit
{
public:
    ASMTextView(QWidget* parent = nullptr);
    void    setAsm(DisasmThing dThing);
    int     sideBarAreaWidth();
    void    sideBarPaintEvent(QPaintEvent* event);
    void    resizeEvent(QResizeEvent* ev);

private:
    QWidget* m_sideBar;
    QList<LineContent> lineContent;
    DisasmThing        disasmThing;
    QMap<snesAddress, quint8>   displayOffsetOfOffset;

    qint32  blockNumberFromOffset(quint32 offset);
    void    updateSideBarAreaWidth(int);
    void    updateSideBarArea(const QRect&, int);


};

#endif // ASMTEXTVIEW_H
