#pragma once

#include <QJsonObject>
#include "register.h"
class RegisterDefinitions
{
public:
    RegisterDefinitions();
    QJsonObject     toJSON() const;
    static  RegisterDefinitions fromJSON(const QJsonObject &obj);

    QVector<Register>   registers;
};

