#ifndef MGLOBAL_H
#define MGLOBAL_H

#include <QString>

template<typename T>
QString sHex(T value, quint8 sizeType = sizeof(T))
{
    return QString("%1").arg(value, sizeType * 2, 16, QChar('0')).toUpper();
}

template<typename T>
QString sBit(T value, quint8 sizeType = sizeof(T))
{
    return QString("%1").arg(value, sizeType * 8, 2, QChar('0')).toUpper();
}

#endif // MGLOBAL_H
