#ifndef HTTPGET_H
#define HTTPGET_H

#include <QObject>
#include <QUrl>
#include <QMap>
#include <QNetworkAccessManager>
#include <QPixmap>


class QIODevice;
class QNetworkReply;
class QNetworkCookieJar;

class HttpGet : public QObject
{
    Q_OBJECT
public:
    HttpGet(QObject* parent = 0);
    QString get(QUrl getUrl);
    QString get(const char* getUrl);
    QString post(QUrl postUrl, const QMap<QString, QString>& params);
    QString getRawHeader(QString);
    QPixmap getPixMapFromUrl(QUrl);
    void initPixMap(QUrl);


    QNetworkCookieJar* getCookieJar() const;
    void setCookieJar(QNetworkCookieJar* cookieJar);

    void setAutoRedirect(bool ifAutoRedirect);
    void setAutoKeepReferer(bool ifAutoKeepReferer);
    //void setAutoKeepCookie(bool ifAutoKeepCookie); //we already auto keep cookie in object 'http'

    void setCodec(const QString& thecodec);
    QString getUrl();

    //nt error();//may cause crash so disable it
    //QString errorStr(); //may cause crash so disable it
private:
    QNetworkAccessManager manager;
    QNetworkReply* netReply;
    QString content;
    QUrl url;
    QNetworkAccessManager http;
    volatile bool isFinished;
    QString codecStr;
    bool autoRedirect;
    bool autoKeepReferer;
    QString refererUrl;
    //bool autoKeepCookie;
    int redirectTimes; //in case we redirect too many times
    QNetworkCookieJar* cookieJar;
    QByteArray imageByteArray;
private slots:
        void httpFinished(QNetworkReply* reply);
        void getPixMapFromUrlFinished(QNetworkReply*);
};

void mySleep(int millisecond);

#endif // HTTPGET_H
