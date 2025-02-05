#include <QTextBlock>
#include <QPainter>
#include "mglobal.h"
#include "asmtextview.h"
#include "asm/asmtextviewsidebar.h"


ASMTextView::ASMTextView(QWidget *parent) : QPlainTextEdit(parent)
{
    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(10);

    setFont(font);
    m_sideBar = new AsmTextViewSideBar(this);
    connect(this, &QPlainTextEdit::updateRequest, this, &ASMTextView::updateSideBarArea);
    connect(this, &QPlainTextEdit::blockCountChanged, this, &ASMTextView::updateSideBarAreaWidth);
}


qint32 ASMTextView::blockNumberFromOffset(quint32 offset)
{
    qint32 pos = 0;
    for (const auto& t : lineContent)
    {
        if (t.type == LineContentType::Instruction && t.instruction.offset == offset)
            return pos;
        pos++;
    }
    return -1;
}

void ASMTextView::setAsm(QList<DisasmInstruction> myAsm)
{
    //QTextCursor cursor(document());
    for (const auto& instruction : myAsm)
    {
        quint32 operand = instruction.operand[0] + instruction.operand[1] * 0x100 + instruction.operand[2] * 0x10000;
        if (instruction.advancedStrinfigy.isEmpty())
            textCursor().insertText(instruction.basicStrinfigy + "\n");
        else
            textCursor().insertText(instruction.advancedStrinfigy + "\n");
        if (instruction.addressing == PC_RELATIVE || instruction.addressing == PC_RELATIVE_WORD)
        {
            BranchingStuff bs;
            bs.startOffset = instruction.offset;
            qint16 pcRel = instruction.addressing == PC_RELATIVE ? (qint8) operand : (qint16) operand;
            bs.pointedOffset = instruction.offset + pcRel + 2;
            m_branching[instruction.offset] = bs;
        }
        if (instruction.opcode == S_INSTR_JMP_ADDRESS_WORD)
        {
            BranchingStuff bs;
            bs.startOffset = instruction.offset;
            bs.pointedOffset = (instruction.offset >> 16) + operand;
            m_branching[instruction.offset] = bs;
        }
        LineContent lc;
        lc.type = LineContentType::Instruction;
        lc.instruction = instruction;
        lineContent.append(lc);
    }
    for (const auto& bs : m_branching)
    {
        const auto bn = blockNumberFromOffset(bs.pointedOffset);
        if (bn  == -1)
            continue;
        qDebug() << " Offset " << sHex(bs.startOffset, 3) << " to " << sHex(bs.pointedOffset, 3) << " - " << bn << "/" << lineContent.size();
        QTextBlock b = document()->findBlockByLineNumber(bn);
        QTextCursor cur = textCursor();
        cur.setPosition(b.position());
        LineContent lc;
        lc.label.offset = bs.pointedOffset;
        lc.label.name = QString(".label_%1").arg(sHex(bs.pointedOffset & 0x00FFFF, 2));
        cur.insertText(lc.label.name + "\n");
        lineContent.insert(bn, lc);
    }
    QTextCursor tc(document());
    tc.movePosition(QTextCursor::Start);
    setTextCursor(tc);
    //instructions = myAsm;
}

int ASMTextView::sideBarAreaWidth()
{
    return fontMetrics().horizontalAdvance("o", 1) * 25;
}

void ASMTextView::sideBarPaintEvent(QPaintEvent *event)
{
    QPainter painter(m_sideBar);
    QColor bg(Qt::lightGray);
    bg = bg.lighter(120);
    quint8 wChar = fontMetrics().horizontalAdvance("o", 1);
    QRect newRect = event->rect();
    newRect.setWidth(newRect.width() - wChar);
    painter.fillRect(newRect, bg);


    //qDebug() << "Width of a char " << wChar;
    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();
    DisasmInstruction inst;
    painter.setPen(QColor(Qt::black).lighter());
    painter.drawLine(m_sideBar->width() - wChar, event->rect().top(), m_sideBar->width() - wChar, event->rect().bottom());
    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top() && blockNumber < lineContent.size())
        {
            if (lineContent.at(blockNumber).type == LineContentType::Instruction)
            {
                inst = lineContent.at(blockNumber).instruction;
                const QString pcText = QString("%1:%2").arg(sHex(inst.offset >> 16, 1)).arg(sHex(inst.offset & 0x00FFFF, 2));
                const QString opcodeText = sHex(inst.opcode);
                const quint32 operand = inst.operand[0] + inst.operand[1] * 0x100 + inst.operand[2] * 0x10000;
                const QString operandText = inst.length == 1 ? "" : sHex(operand, (inst.length - 1));
                painter.setPen(Qt::black);
                painter.drawText(0, top, fontMetrics().horizontalAdvance(pcText), fontMetrics().height(),
                                 Qt::AlignLeft, pcText);
                painter.setPen(Qt::darkGreen);
                painter.drawText(wChar * 10, top, fontMetrics().horizontalAdvance(opcodeText), fontMetrics().height(),
                                Qt::AlignLeft, opcodeText);
                painter.setPen(QColor(Qt::darkGreen).darker());
                painter.drawText(wChar * 13, top, fontMetrics().horizontalAdvance(operandText), fontMetrics().height(),
                                 Qt::AlignLeft, operandText);
            }
        }
        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
    quint32 firstBlockNumber = firstVisibleBlock().blockNumber();
    auto lcFirst = lineContent.at(firstBlockNumber);
    while (lcFirst.type != LineContentType::Instruction)
        lcFirst = lineContent.at(++firstBlockNumber);
    quint32 firstVisibleOffset = lcFirst.instruction.offset;
    quint32 lastVisibleOffset = inst.offset;
    quint8 dOffset = 1;
    QList<QColor> colors = {Qt::darkGreen, Qt::darkBlue, Qt::darkRed, Qt::darkCyan};
    QMap<quint32, quint8> offsetDOffset;
    for (const auto& branch : qAsConst(m_branching))
    {
        quint8 maxOffsetFound = 0;
        auto plop = offsetDOffset.keys();
        std::sort(plop.begin(), plop.end());
        for (auto f : plop)
        {
            if (branch.startOffset > branch.pointedOffset) // up
            {
                if (f > branch.pointedOffset && maxOffsetFound < offsetDOffset[f])
                    maxOffsetFound = offsetDOffset[f];
                if (f > branch.startOffset)
                    break;
            }
            if (branch.startOffset < branch.pointedOffset) // down
            {
                if (f > branch.startOffset && maxOffsetFound < offsetDOffset[f])
                    maxOffsetFound = offsetDOffset[f];
                if (f > branch.pointedOffset)
                    break;
            }
        }
        dOffset = maxOffsetFound + 1;
        offsetDOffset[branch.startOffset] = dOffset;
        offsetDOffset[branch.pointedOffset] = dOffset;
        if (   (branch.startOffset < firstVisibleOffset && branch.pointedOffset < firstVisibleOffset)
            || (branch.startOffset > lastVisibleOffset && branch.pointedOffset > lastVisibleOffset)
            )
            continue;
        QTextBlock startBlock = document()->findBlockByLineNumber(blockNumberFromOffset(branch.startOffset));
        QTextBlock pointedBlock = document()->findBlockByLineNumber(blockNumberFromOffset(branch.pointedOffset));
        qDebug() << "Start " << startBlock.text() << " - Pointed " << pointedBlock.text();
        const int startTop = (int) blockBoundingGeometry(startBlock).translated(contentOffset()).top();
        const int pointedTop = (int) blockBoundingGeometry(pointedBlock).translated(contentOffset()).top();
        const int midStart = startTop + blockBoundingRect(startBlock).height() / 2;
        const int midPointed = pointedTop + blockBoundingRect(pointedBlock).height() / 2;
        painter.setPen(colors[branch.startOffset % colors.size()]);
        painter.drawLine(m_sideBar->width() - wChar, midStart, m_sideBar->width() - wChar * (dOffset + 1), midStart);
        painter.drawLine(m_sideBar->width() - wChar * (dOffset + 1), midStart, m_sideBar->width() - wChar * (dOffset + 1), midPointed);
        painter.drawLine(m_sideBar->width() - wChar, midPointed, m_sideBar->width() - wChar * (dOffset + 1), midPointed);
        painter.drawLine(m_sideBar->width() - wChar, midPointed, m_sideBar->width() - wChar - 4 , midPointed + 4);
        painter.drawLine(m_sideBar->width() - wChar, midPointed, m_sideBar->width() - wChar - 4 , midPointed - 4);
    }
}

void ASMTextView::resizeEvent(QResizeEvent *ev)
{
    QPlainTextEdit::resizeEvent(ev);

    QRect cr = contentsRect();
    m_sideBar->setGeometry(QRect(cr.left(), cr.top(), sideBarAreaWidth(), cr.height()));
}

void ASMTextView::updateSideBarAreaWidth(int)
{
    setViewportMargins(sideBarAreaWidth(), 0, 0, 0);
}

void ASMTextView::updateSideBarArea(const QRect &rect, int dy)
{
    if (dy)
        m_sideBar->scroll(0, dy);
    else
        m_sideBar->update(0, rect.y(), m_sideBar->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateSideBarAreaWidth(0);
}
