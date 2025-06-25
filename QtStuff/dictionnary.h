#pragma once

#include "mytype.h"
#include "snestype.h"

using namespace SNESType;

struct  Dictionnary : public NamedThing
{
    DataType    type = DataType::Unknow;
    DataType    subType = DataType::Unknow;
    unsigned    int size = 0;
    snesAddress address = snesAddress(0);
    SNESStruct  sstruct;
    Array       array;
    unsigned    int calculatedSize() const;
    virtual QString         toString();
    virtual QJsonObject     toJSON() const;
    static Dictionnary fromJSON(const QJsonObject &jsonObject);
};
