#include "romheaderview.h"
#include "ui_romheaderview.h"

RomHeaderView::RomHeaderView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RomHeaderView)
{
    ui->setupUi(this);
}

RomHeaderView::~RomHeaderView()
{
    delete ui;
}

void RomHeaderView::refresh()
{
    auto header_infos = DataHolder::dh()->romHeaderInfo();
}
