#ifndef SNESSTRUC_H
#define SNESSTRUC_H

#include <QString>
#include <QVector>

enum  FieldType  {
    None,
    Byte,
    Word,
    LongPtr,
    WordPtr
};

struct SNESField
{
    enum FieldType  type;
    QString         name;
    quint8          size;
    QString         comment;
};


struct SNESStruct
{
    QString name;
    QVector<SNESField> fields;
};

#endif // SNESSTRUC_H
