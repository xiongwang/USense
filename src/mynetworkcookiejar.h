#ifndef MYNETWORKCOOKIEJAR_H
#define MYNETWORKCOOKIEJAR_H

#include <QNetworkCookieJar>
#include "renren/httpget.h"
#include <QDateTime>

class MyNetworkCookieJar : public QNetworkCookieJar
{
    Q_OBJECT
public:
    explicit MyNetworkCookieJar(QObject *parent = 0);
    void setCookies(const QList<QNetworkCookie> & cookieList);
    QString getImageUrl(const QString &sourceUrl);

signals:

public slots:
private:
    QString getResult(const QString &sourceUrl);
    void initLists();

    QList<QNetworkCookie> cookieList;
    QList<QByteArray> nameList,valueList;
    QList<QString> dateTimeList;

};

#endif // MYNETWORKCOOKIEJAR_H
