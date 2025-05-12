#include "wrammap.h"

#include <QJsonArray>

WRAMMap::WRAMMap()
{

}

QJsonObject WRAMMap::toJSON() const
{
    QJsonObject toret;
    QJsonArray jsArray;
    for(const Dictionnary& entry : entries)
    {
        jsArray.append(entry.toJSON());
    }
    toret.insert("wrammap", jsArray);
    return toret;
}

WRAMMap WRAMMap::fromJSON(const QJsonObject& obj)
{
    WRAMMap toret;

    const QJsonArray array = obj.value("wrammap").toArray();
    for (const auto& entry : array)
    {
        toret.entries.append(Dictionnary::fromJSON(entry.toObject()));
    }
    return toret;
}
