#ifndef ROM_H
#define ROM_H

#include <QString>
#include <QVector>

const int SNESBankSize = 0x10000;

enum class RomType {
    HiROM,
    LoROM,
    ExHiROM,
    ExLoROM
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
    RomType         type;
    HeaderInfos     headerInfos;
    QVector<Bank>   banks;
    static Rom      openRomFile(const QString& path);
};

#endif // ROM_H
