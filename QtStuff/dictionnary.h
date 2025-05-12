#pragma once

#include "mytype.h"
#include "snestype.h"

using namespace SNESType;

struct  Dictionnary : public NamedThing
{
    DataType    type;
    DataType    subType = DataType::Unknow;
    unsigned    int size;
    snesAddress address;
    SNESStruct  sstruct;
    Array       array;
    unsigned    int calculatedSize() const;
    QString         toString();
    QJsonObject     toJSON() const;
    static Dictionnary fromJSON(const QJsonObject &jsonObject);
};
