#include "asmtextviewsidebar.h"

AsmTextViewSideBar::AsmTextViewSideBar(ASMTextView *parent)
    : QWidget{parent}
{
    m_asmView = parent;
}

void AsmTextViewSideBar::paintEvent(QPaintEvent *event)
{
    m_asmView->sideBarPaintEvent(event);
}

QSize AsmTextViewSideBar::sizeHint() const
{
    return QSize(m_asmView->sideBarAreaWidth(), 0);
}
