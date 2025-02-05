#include "wramentrydisplay.h"
#include "qendian.h"
#include "ui_wramentrydisplay.h"

WRamEntryDisplay::WRamEntryDisplay(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WRamEntryDisplay)
{
    ui->setupUi(this);
}

WRamEntryDisplay::~WRamEntryDisplay()
{
    delete ui;
}

void WRamEntryDisplay::setWramData(QByteArray *data)
{
    wramData = data;
}

static qint32 dataToSigned(QByteArray entryData)
{
    QByteArray tmp = entryData;
    while (tmp.size() != entryData.size())
    {
        tmp.append('\0');
    }
    return qFromLittleEndian<qint32>(tmp.data());
}

static quint32 dataToUnSigned(QByteArray entryData)
{
    QByteArray tmp = entryData;
    while (tmp.size() != 4)
    {
        tmp.append('\0');
    }
    qDebug() << entryData << " - " << tmp;
    return qFromLittleEndian<quint32>(tmp.data());
}


void WRamEntryDisplay::setWRamMapEntry(WRAMMap::MapEntry *entry)
{
    qDebug() << "Showing " << entry->name << QString::number(entry->address, 16) << entry->size;
    unsigned int eSize = entry->size;
    ui->adddressLabel->setText("$" + QString::number(entry->address, 16).toUpper());
    QByteArray entryData = wramData->mid(entry->address, eSize);
    const unsigned char* data = (unsigned char*) entryData.constData();
    QString rawText;
    quint32 unsigned_value = 0;
    qint32  signed_value = 0;
    quint32 offset = 0xFFFFFF;
    if (eSize < 4)
    {
        for (unsigned int i = 0; i < eSize; i++)
        {
            rawText.append(QString::number(data[i], 16).toUpper() + " ");
        }
    }
    switch (entry->type) {
    case DataType::Byte:
    case DataType::Word:
    case DataType::Long:
    {
        unsigned_value = dataToUnSigned(entryData);
        signed_value = dataToSigned(entryData);
        break;
    }
    case DataType::WordPtr:
    case DataType::LongPtr:
    {
        offset = dataToUnSigned(entryData);
        if (entry->type == DataType::LongPtr && offset < 0x800000 && offset >= 7E0000)
        {
            offset -= 0x7E0000;
        }
        if (entry->type == DataType::LongPtr && (offset >= 0x800000 || offset < 7E0000))
        {
            rawText = rawText + "ROM address : " + QString::number(offset, 16);
        } else {
            qDebug() << "Offset " << QString::number(offset, 16);
            unsigned_value = dataToUnSigned(wramData->mid(offset, 3));
            signed_value = dataToSigned(wramData->mid(offset, 3));
        }
    }
    }
    ui->rawBytesLabel->setText(rawText);
    ui->signedLabel->setText(QString::number(signed_value));
    ui->unsignedLabel->setText(QString::number(unsigned_value));
}
