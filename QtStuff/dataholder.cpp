#include "dataholder.h"

DataHolder* DataHolder::myDH = nullptr;

QByteArray DataHolder::getData(snesAddress address, quint32 size)
{
    return QByteArray();
}

QByteArray DataHolder::getWRAMData(quint16 address, quint16 size)
{
    return QByteArray();
}

QByteArray DataHolder::getROMData(quint16 address, quint16 size)
{
    return QByteArray();
}

QByteArray DataHolder::getROMData(snesAddress address, quint16 size)
{
    return QByteArray();
}

void DataHolder::setWRAMData(const QByteArray &data)
{

}

void DataHolder::setROMData(quint8 bank, const QByteArray &data)
{

}

DataHolder *DataHolder::instance()
{
    if (myDH == nullptr)
        myDH = new DataHolder();
    return myDH;
}

DataHolder::DataHolder() {}
