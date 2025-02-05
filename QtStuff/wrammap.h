#ifndef WRAMMAP_H
#define WRAMMAP_H

#include <QObject>
#include <QJsonObject>
#include "snestype.h"

using namespace SNESType;

class WRAMMap
{
public:
    WRAMMap();
    QJsonObject     toJSON() const;
    static  WRAMMap fromJSON(const QJsonObject &obj);

    struct MapEntry : public Thing
    {
        DataType    type;
        DataType    subType;
        unsigned    int size;
        quint32     address;
        SNESStruct  sstruct;
        Array       array;
        unsigned    int calculatedSize() const;
        QString         toString();
    };
    QVector<MapEntry>   entries;
};

#endif // WRAMMAP_H
