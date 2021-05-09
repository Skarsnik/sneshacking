#ifndef ROMVIEWER_H
#define ROMVIEWER_H

#include "rom.h"

#include <QWidget>

namespace Ui {
class RomViewer;
}

class RomViewer : public QWidget
{
    Q_OBJECT

public:
    explicit RomViewer(QWidget *parent = nullptr);
    ~RomViewer();

    void setRom(Rom rom);
private slots:
    void on_banksView_clicked(const QModelIndex &index);

private:
    Ui::RomViewer *ui;
    Rom         m_rom;
};

#endif // ROMVIEWER_H
