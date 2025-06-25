#pragma once


#include "dictionnary.h"

/*enum RegisterFlag {

};

Q_DECLARE_FLAGS(RegisterFlags, RegisterFlag);*/

struct Register : public Dictionnary
{
    QString     canonicalName;
    QString     flags;
    Register();
    Register(const Dictionnary& dic) : Dictionnary(dic) {}
    virtual QJsonObject     toJSON() const override;
    static Register fromJSON(const QJsonObject &jsonObject);
};
