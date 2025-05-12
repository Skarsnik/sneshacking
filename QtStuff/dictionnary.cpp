#include "dictionnary.h"

unsigned int Dictionnary::calculatedSize() const
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

QJsonObject Dictionnary::toJSON() const
{
    QJsonObject obj;
    obj.insert("name", name);
    obj.insert("comment", comment);
    obj.insert("type", DataTypeToString(type));
    obj.insert("subtype", DataTypeToString(type));
    if (type == DataType::Array)
        obj.insert("array", array.toJSON());
    if (type == DataType::Struct)
        obj.insert("struct", sstruct.toJSON());
    obj.insert("address", QString("$%1").arg(address, 0, 16));
    return obj;
}

Dictionnary Dictionnary::fromJSON(const QJsonObject& jsonObject)
{
    Dictionnary newEntry;
    newEntry.type = DataTypeFromString(jsonObject.value("type").toString());
    newEntry.subType = DataTypeFromString(jsonObject.value("subtype").toString());
    newEntry.name = jsonObject.value("name").toString();
    bool ok;
    newEntry.address = snesAddress(jsonObject.value("address").toString().remove(0, 1).toUInt(&ok, 16));
    newEntry.comment = jsonObject.value("comment").toString();
    if (newEntry.type == DataType::Array)
    {
        newEntry.array = Array::fromJSON(jsonObject.value("array").toObject());
    }
    if (newEntry.type == DataType::Struct)
    {
        newEntry.sstruct = SNESStruct::fromJSON(jsonObject.value("array").toObject());
    }
    newEntry.size = newEntry.calculatedSize();
    return newEntry;
}
