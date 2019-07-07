#include "romviewer.h"
#include "ui_romviewer.h"

RomViewer::RomViewer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RomViewer)
{
    ui->setupUi(this);
}

RomViewer::~RomViewer()
{
    delete ui;
}
