#ifndef WRAMENTRYDISPLAY_H
#define WRAMENTRYDISPLAY_H

#include "wrammap.h"

#include <QWidget>

namespace Ui {
class WRamEntryDisplay;
}

class WRamEntryDisplay : public QWidget
{
    Q_OBJECT

public:
    explicit WRamEntryDisplay(QWidget *parent = nullptr);
    ~WRamEntryDisplay();
    void     setWramData(QByteArray* data);
    void     setWRamMapEntry(Dictionnary* entry);

private:
    QByteArray*          wramData;
    Ui::WRamEntryDisplay *ui;
};

#endif // WRAMENTRYDISPLAY_H
