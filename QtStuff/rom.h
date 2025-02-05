#ifndef ROM_H
#define ROM_H

#include "mytype.h"
#include <QFile>
#include <QString>
#include <QVector>

const int SNESBankSize = 0x10000;

enum class RomMapping {
    LoROM = 1,
    HiROM = 2,
    ExLoROM = 3,
    ExHiROM = 4
};

struct Bank {
    QString     name;
    QString     description;
    QByteArray  data;
};

struct HeaderInfos
{
    quint16 nativeVectorCop;
    quint16 nativeVectorBrk;
    quint16 nativeVectorAbort;
    quint16 nativeVectorNMI;
    quint16 nativeVectorReset;
    quint16 nativeVectorIRQ;

    quint16 emulationVectorCop;
    quint16 emulationVectorAbort;
    quint16 emulationVectorNMI;
    quint16 emulationVectorReset;
    quint16 emulationVectorIRQ;

};

struct Rom
{
    QString         name;
    bool            hasHeader;
    QByteArray      rawTitle;
    RomMapping      mapping;
    bool            fastRom;
    HeaderInfos     headerInfos;
    QVector<Bank>   banks;
    QFile*          qFile;
    QByteArray      datas(snesAddress, quint32 length);
    snesAddress     pcToSnes(quint32 address);
    quint32         snesToPc(snesAddress address);
    static Rom      openRomFile(const QString& path);
};

#endif // ROM_H
