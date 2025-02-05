#include "wrammap.h"

#include <QJsonArray>

WRAMMap::WRAMMap()
{

}

QJsonObject WRAMMap::toJSON() const
{
    QJsonObject toret;
    QJsonArray jsArray;
    for(const MapEntry& entry : entries)
    {
        QJsonObject obj;
        obj.insert("name", entry.name);
        obj.insert("comment", entry.comment);
        obj.insert("type", DataTypeToString(entry.type));
        if (entry.type == DataType::Array)
            obj.insert("array", entry.array.toJSON());
        if (entry.type == DataType::Struct)
            obj.insert("struct", entry.sstruct.toJSON());
        obj.insert("address", QString("$%1").arg(entry.address, 0, 16));
        jsArray.append(obj);
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
        QJsonObject obj = entry.toObject();
        MapEntry newEntry;
        newEntry.type = DataTypeFromString(obj.value("type").toString());
        newEntry.name = obj.value("name").toString();
        bool ok;
        newEntry.address = obj.value("address").toString().remove(0, 1).toUInt(&ok, 16);
        newEntry.comment = obj.value("comment").toString();
        if (newEntry.type == DataType::Array)
        {
            newEntry.array = Array::fromJSON(obj.value("array").toObject());
        }
        if (newEntry.type == DataType::Struct)
        {
            newEntry.sstruct = SNESStruct::fromJSON(obj.value("array").toObject());
        }
        newEntry.size = newEntry.calculatedSize();
        toret.entries.append(newEntry);
    }
    return toret;
}

unsigned int WRAMMap::MapEntry::calculatedSize() const
{
    switch(type)
    {
    case DataType::Byte:
        return 1;
    case DataType::Word:
    case DataType::WordPtr:
        return 2;
    case DataType::Long:
    case DataType::LongPtr:
        return 3;
    case DataType::Array:
    default:
        return size;
    }

}
