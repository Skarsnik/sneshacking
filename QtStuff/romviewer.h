#ifndef ROMVIEWER_H
#define ROMVIEWER_H

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

private:
    Ui::RomViewer *ui;
};

#endif // ROMVIEWER_H
