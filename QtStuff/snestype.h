#pragma once

#include <QJsonObject>
#include <QString>
#include <QVector>
#include <QObject>

namespace SNESType
{
    Q_NAMESPACE

enum class BaseType {
    Byte,
    Word,
    Long
};

Q_ENUM_NS(BaseType)

enum class DataType {
    Byte,
    Word,
    Long,
    Unknow,
    Data,
    BitField,
    LongPtr,
    WordPtr,
    Array,
    Struct,
    LinkedList,
    RawGFX,
    CompressedGFX,
    SPCData
};

Q_ENUM_NS(DataType)

enum class FieldType  {
    None,
    Byte,
    Word,
    LongPtr,
    WordPtr
};

Q_ENUM_NS(FieldType)

enum class AddressLocation {
    WRAM,
    SRAM,
    ROM,
    REGISTER
};

Q_ENUM_NS(AddressLocation)

struct NamedThing
{
    QString name;
    QString comment;
};

struct SNESField : public NamedThing
{
    enum FieldType      type;
    quint8              size;
    static SNESField    fromJSON(const QJsonObject& obj);
    QJsonObject         toJSON() const;
};

struct SNESStruct : public NamedThing
{
    QVector<SNESField>  fields;
    unsigned int        size;
    static SNESStruct   fromJSON(const QJsonObject& obj);
    QJsonObject         toJSON() const;
    ~SNESStruct() {
        fields.clear();
    }
};

struct Array : public NamedThing
{
    DataType        type;
    unsigned int    numberOfElements;
    unsigned int    size;
    QJsonObject     toJSON() const;
    static Array    fromJSON(const QJsonObject& obj);
};

struct LinkedList : public SNESStruct
{
    quint8          posNext;
};

QString FieldTypeToString(const FieldType type);
FieldType FieldTypeFromString(const QString str);
QString DataTypeToString(const DataType type);
DataType DataTypeFromString(const QString str);

}

Q_DECLARE_METATYPE(SNESType::DataType)
