#include "register.h"

Register::Register()
{
}

QJsonObject     Register::toJSON() const
{
    auto jObj = Dictionnary::toJSON();
    jObj.insert("canonicalname", canonicalName);
    jObj.insert("flags", flags);
    return jObj;
};

Register Register::fromJSON(const QJsonObject &jsonObject)
{
    Register reg(Dictionnary::fromJSON(jsonObject));
    reg.canonicalName = jsonObject["canonicalname"].toString();
    reg.flags = jsonObject["flags"].toString();
    return reg;
}
