#include "mytype.h"
#include "mglobal.h"

QDebug operator<<(QDebug debug, const snesAddress &a)
{
    debug.nospace() << sHex(a, 3);
    return debug;
}
