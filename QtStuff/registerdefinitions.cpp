#include <QJsonArray>
#include "registerdefinitions.h"


RegisterDefinitions::RegisterDefinitions() {}

QJsonObject RegisterDefinitions::toJSON() const
{
    QJsonObject toret;
    QJsonArray jsArray;
    for (auto const &entry : registers)
    {
        jsArray.append(entry.toJSON());
    }
    toret.insert("registers", jsArray);
    return toret;
}

RegisterDefinitions RegisterDefinitions::fromJSON(const QJsonObject &obj)
{
    RegisterDefinitions toret;

    const QJsonArray array = obj.value("registers").toArray();
    for (const auto& entry : array)
    {
        toret.registers.append(Register::fromJSON(entry.toObject()));
    }
    return toret;
}
