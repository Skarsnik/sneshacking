#include "romviewer.h"
#include "ui_romviewer.h"

#include <QStandardItemModel>
#include <QHexView/qhexview.h>
#include <QDebug>
#include <document/buffer/qmemoryrefbuffer.h>

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

void    RomViewer::setRom(Rom rom)
{
    m_rom = rom;
    QStandardItemModel* bankModel = new QStandardItemModel();
    unsigned int i = 0;
    foreach (Bank bank, m_rom.banks)
    {
        QStandardItem *item = new QStandardItem(bank.name);
        item->setData(i, Qt::UserRole);
        bankModel->appendRow(item);
        i++;
    }
    ui->banksView->setModel(bankModel);
}

void RomViewer::on_banksView_clicked(const QModelIndex &index)
{
    int bankNumber = ui->banksView->model()->data(index, Qt::UserRole).toInt();
    qDebug() << m_rom.banks[bankNumber].data.size();
    QHexDocument* document = QHexDocument::fromMemory<QMemoryRefBuffer>(m_rom.banks[bankNumber].data);
    QHexView* hexview = new QHexView();
    hexview->setDocument(document);
    ui->bankTabsWidget->addTab(hexview, m_rom.banks[bankNumber].name);

}
