#ifndef ROMHEADERVIEW_H
#define ROMHEADERVIEW_H

#include <QWidget>

namespace Ui {
class RomHeaderView;
}

class RomHeaderView : public QWidget
{
    Q_OBJECT

public:
    explicit RomHeaderView(QWidget *parent = nullptr);
    ~RomHeaderView();
    void    refresh();

private:
    Ui::RomHeaderView *ui;
};

#endif // ROMHEADERVIEW_H
