#include "romviewer.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RomViewer w;
    w.show();
    w.setRom(Rom::openRomFile("F:/Emulation/Secret of Evermore/Secret of Evermore.smc"));

    return a.exec();
}
