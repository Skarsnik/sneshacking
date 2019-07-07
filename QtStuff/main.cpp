#include "romviewer.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RomViewer w;
    w.show();

    return a.exec();
}
