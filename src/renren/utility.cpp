#include "utility.h"

Utility::Utility()
{
}

QString Utility::getSignature(QMap<QString, QString> param)
{
    QString sig = "";

    QMapIterator<QString, QString> iterator(param);
    while(iterator.hasNext()) {
        iterator.next();
        sig += iterator.key() + "=" + iterator.value();
    }

    sig += Config::Secret;

    QString md5 = "";
    QByteArray bb;
    bb = QCryptographicHash::hash ( sig.toAscii(), QCryptographicHash::Md5 );
    md5.append(bb.toHex());

    return md5;
}

QString Utility::getCallID()
{
    return QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch());
}
