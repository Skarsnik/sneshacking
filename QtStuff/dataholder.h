#ifndef DATAHOLDER_H
#define DATAHOLDER_H

#include <QObject>
#include <mytype.h>

class DataHolder
{
    Q_OBJECT
public:
    static      QByteArray  getData(snesAddress address, quint32 size);
    static      QByteArray  getWRAMData(quint16 address, quint16 size);
    static      QByteArray  getROMData(quint16 address, quint16 size);
    static      QByteArray  getROMData(snesAddress address, quint16 size);

    static      void        setWRAMData(const QByteArray& data);
    static      void        setROMData(quint8 bank, const QByteArray& data);
    static      DataHolder* instance();
signals:
    void    wramChanged();

private:
    DataHolder();
    static DataHolder*  myDH;
    QByteArray          WRAM;
    QVector<QByteArray> ROM;
};

#endif // DATAHOLDER_H
