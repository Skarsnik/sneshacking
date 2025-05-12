#ifndef WRAMMAP_H
#define WRAMMAP_H

#include <QObject>
#include <QJsonObject>
#include "dictionnary.h"

using namespace SNESType;

class WRAMMap
{
public:
    WRAMMap();
    QJsonObject     toJSON() const;
    static  WRAMMap fromJSON(const QJsonObject &obj);

    QVector<Dictionnary>   entries;
};

#endif // WRAMMAP_H
