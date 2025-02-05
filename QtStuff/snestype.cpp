#include "snestype.h"
#include <QJsonValue>
#include <QJsonArray>
#include <QMetaEnum>

namespace SNESType {


SNESField SNESField::fromJSON(const QJsonObject& obj)
{
    SNESField toret;
    toret.name = obj.value("name").toString();
    toret.size = (quint8) obj.value("size").toInt();
    toret.comment = obj.value("comment").toString();
    const QMetaObject &mo = SNESType::staticMetaObject;
    int i = mo.indexOfEnumerator("FieldType");
    QMetaEnum fieldMetaEnum = mo.enumerator(i);
    toret.type = static_cast<SNESType::FieldType>(fieldMetaEnum.keyToValue(obj.value("type").toString().toLocal8Bit()));
    return toret;
}

QJsonObject SNESField::toJSON() const
{
    QJsonObject toret;
    toret.insert("name", name);
    toret.insert("comment", comment);
    toret.insert("size", size);
    const QMetaObject &mo = SNESType::staticMetaObject;
    int i = mo.indexOfEnumerator("FieldType");
    QMetaEnum fieldMetaEnum = mo.enumerator(i);
    toret.insert("type", qPrintable(fieldMetaEnum.valueToKey(static_cast<int>(type))));
    return toret;
}

SNESStruct SNESStruct::fromJSON(const QJsonObject& obj)
{
    SNESStruct toret;
    toret.name = obj.value("name").toString();
    toret.size = obj.value("size").toInt();
    toret.comment = obj.value("comment").toString();
    QJsonArray fields = obj.value("fields").toArray();   
    for (QJsonValue fieldObj : fields)
    {
        toret.fields.append(SNESField::fromJSON(fieldObj.toObject()));
    }
    return toret;
}

QJsonObject SNESStruct::toJSON() const
{
    QJsonObject toret;
    toret.insert("name", name);
    toret.insert("size", (int) size);
    toret.insert("comment", comment);
    QJsonArray fieldArray;

    for (SNESField field : fields)
    {
        fieldArray.append(field.toJSON());
    }
    toret.insert("fields", fieldArray);
    return toret;
}


Array   Array::fromJSON(const QJsonObject& obj)
{
    Array toret;
    toret.size = obj.value("size").toInt();
    toret.name = obj.value("name").toString();
    toret.comment = obj.value("comment").toString();
    const QMetaObject &mo = SNESType::staticMetaObject;
    int i = mo.indexOfEnumerator("DataType");
    QMetaEnum dataTypeMetaEnum = mo.enumerator(i);
    toret.type = static_cast<SNESType::DataType>(dataTypeMetaEnum.keyToValue(obj.value("type").toString().toLocal8Bit()));
    return toret;
}

QJsonObject Array::toJSON() const
{
    QJsonObject toret;
    toret.insert("name", name);
    toret.insert("size", (int) size);
    toret.insert("comment", comment);
    toret.insert("type", DataTypeToString(type));
    return toret;
}


QString DataTypeToString(const DataType type)
{
    const QMetaObject &mo = SNESType::staticMetaObject;
    int i = mo.indexOfEnumerator("DataType");
    QMetaEnum metaEnum = mo.enumerator(i);
    return qPrintable(metaEnum.valueToKey(static_cast<int>(type)));
}

DataType DataTypeFromString(const QString str)
{
    const QMetaObject &mo = SNESType::staticMetaObject;
    int i = mo.indexOfEnumerator("DataType");
    QMetaEnum dataTypeMetaEnum = mo.enumerator(i);
    return static_cast<SNESType::DataType>(dataTypeMetaEnum.keyToValue(str.toLocal8Bit()));
}

QString FieldTypeToString(const FieldType type)
{
    const QMetaObject &mo = SNESType::staticMetaObject;
    int i = mo.indexOfEnumerator("FieldType");
    QMetaEnum metaEnum = mo.enumerator(i);
    return qPrintable(metaEnum.valueToKey(static_cast<int>(type)));
}

FieldType FieldTypeFromString(const QString str)
{
    const QMetaObject &mo = SNESType::staticMetaObject;
    int i = mo.indexOfEnumerator("DataType");
    QMetaEnum metaEnum = mo.enumerator(i);
    return static_cast<SNESType::FieldType>(metaEnum.keyToValue(str.toLocal8Bit()));
}

}
