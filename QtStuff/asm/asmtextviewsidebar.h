#ifndef ASMTEXTVIEWSIDEBAR_H
#define ASMTEXTVIEWSIDEBAR_H

#include "asmtextview.h"

#include <QWidget>

class AsmTextViewSideBar : public QWidget
{
    Q_OBJECT
public:
    explicit AsmTextViewSideBar(ASMTextView* parent = nullptr);

signals:

protected:
    void    paintEvent(QPaintEvent* event);
    QSize   sizeHint() const;

private:
    ASMTextView*    m_asmView;
};

#endif // ASMTEXTVIEWSIDEBAR_H
