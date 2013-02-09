#ifndef UTILITY_H
#define UTILITY_H

#include <qmap.h>
#include "config.h"
#include <vector>
#include <QTextCodec>
#include <QCryptographicHash>
#include <QTime>

class Utility
{
public:
    Utility();

    QMap<QString,QString> Param;

    static QString getSignature(QMap<QString, QString> param);
    static QString getCallID();
};

#endif // UTILITY_H
