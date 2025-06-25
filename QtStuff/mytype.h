#pragma once

#include <QDebug>

#include "qglobal.h"

enum snesAddress : quint32 {};
QDebug operator<<(QDebug debug, const snesAddress &a);

