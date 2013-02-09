#include <QTextCodec>
#include <QApplication>
#include <QByteArray>
#include "httpget.h"
#include <QBuffer>
#include <QDebug>
#include <QTextStream>
#include <QNetworkReply>
#include <QList>
#include <QNetworkCookieJar>

#if (WIN32)
#include "windows.h"
#else
//linux here
#include <unistd.h>
#endif


HttpGet::HttpGet(QObject* parent)
    :QObject(parent)
{
    connect(&http, SIGNAL(finished(QNetworkReply*)), this, SLOT(httpFinished(QNetworkReply*)));
    netReply = NULL;
    redirectTimes = 0;
    autoRedirect = true;
    autoKeepReferer = true;
    //autoKeepCookie = true;

    isFinished = false;
    codecStr = ""; //default is UTF-8
    cookieJar = NULL;
    refererUrl = "";
}

void HttpGet::setAutoRedirect(bool ifAutoRedirect){
    autoRedirect = ifAutoRedirect;
}





void HttpGet::httpFinished(QNetworkReply * reply){
    QVariant returnCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    cookieJar = getCookieJar();
    refererUrl = url.toString();
    QString codecStrThisTime = codecStr;

    if(!redirectionTarget.isNull() && autoRedirect && !redirectionTarget.isNull() && redirectTimes < 10){
        QUrl newUrl = url.resolved(redirectionTarget.toUrl());
        url = newUrl;
        redirectTimes++;
        get(newUrl);
    }
    else if(!returnCode.isNull() && (returnCode.toInt() >= 400)){
        content = "";
        isFinished = true;
    }
    else{
        QByteArray ba = reply->readAll();
        QString tempContent = ba;
        bool getSetting = false;
        int headb = tempContent.indexOf("<head>");
        int heade = tempContent.indexOf("</head>");
        QString codecInPage;

        //get code setting in page
        if(headb != -1 && heade > headb){
                QString header = tempContent.mid(headb, heade - headb);
                int metab = 0;
                while(!getSetting && (metab = header.indexOf("<meta ", metab)) != -1){
                        int metae = header.indexOf(">", metab);
                        if(metae != -1){
                                QString metaline = header.mid(metab, metae - metab);

                                if(metaline.indexOf("charset=") != -1){
                                        int charsetb = metaline.indexOf("charset=") + QString("charset=").length();
                                        int charsete = metaline.indexOf("\"", charsetb);
                                        if(charsete != -1){
                                                codecInPage = metaline.mid(charsetb, charsete - charsetb);
                                                if(codecStrThisTime.isEmpty()){
                                                        codecStrThisTime = codecInPage;
                                                }
                                                else{
                                                        if(codecStrThisTime != codecInPage){
                                                                qDebug() << "warning: in " << url.toString() << ", we find charset is " << codecInPage
                                                                                << ", but the codec is set to " << codecStrThisTime;
                                                        }
                                                }
                                                getSetting = true;
                                        }
                                        else{
                                                qDebug() << "strange error when tried to get charset setting in web page " << url.toString();
                                        }
                                }
                        }
                        metab++;
                }
        } //finish getting code setting in page


                if(codecStrThisTime.isEmpty()){
                        codecStrThisTime = "UTF-8";
                }
        //qDebug() << codecStrThisTime;
            QTextCodec *codec = QTextCodec::codecForName(codecStrThisTime.toAscii());
            if(!codec){
                if(codecInPage.isEmpty() && !QTextCodec::codecForName(codecInPage.toAscii())){
                        codecStrThisTime = "UTF-8";
                }
                else{
                codecStrThisTime = codecInPage;
                }

                qDebug() << "warning: codec invalid.  Set to " << codecStrThisTime;

                codec = QTextCodec::codecForName(codecStrThisTime.toAscii());
            }
            if(codec){
                content = codec->toUnicode(ba);
            }
        else{
                qDebug() << "warning: codec invalid: " << codecStrThisTime; //should not be here..
                content = ba;
        }

        isFinished = true;
    }
}

void HttpGet::setCodec(const QString& thecodec)
{
        codecStr = thecodec;
}

void HttpGet::setCookieJar(QNetworkCookieJar* cookieJar)
{
    QObject* parent = cookieJar->parent();
    http.setCookieJar(cookieJar);
    cookieJar->setParent(parent);
}

void HttpGet::setAutoKeepReferer(bool ifAutoKeepReferer){
        autoKeepReferer = ifAutoKeepReferer;
}

//void HttpGet::setAutoKeepCookie(bool ifAutoKeepCookie){
//	autoKeepCookie = ifAutoKeepCookie;
//}

QNetworkCookieJar* HttpGet::getCookieJar() const
{
    return http.cookieJar();
}



QString HttpGet::post(QUrl postUrl, const QMap<QString, QString>& params){
//    if(autoKeepCookie && cookieJar != NULL){
//        setCookieJar(cookieJar);
//    }

    content = "";
    isFinished = false;
    //url = getUrl; //I don't set url to postUrl here...
    QString postContent;

    if(!params.isEmpty()){
        QMapIterator<QString, QString> i(params);
        while (i.hasNext()) {

            i.next();
            postContent += (QUrl::toPercentEncoding(i.key()) + "=" + QUrl::toPercentEncoding(i.value()) + "&");
        }

        if(postContent.endsWith("&")){
            postContent = postContent.left(postContent.length()-1);
        }
    }

    QByteArray ba;
    ba.append(postContent);

    QNetworkRequest req(postUrl);

    if(autoKeepReferer && !refererUrl.isEmpty()){
        req.setRawHeader("Referer", refererUrl.toStdString().c_str());
    }
    req.setRawHeader("User-Agent", "Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US) AppleWebKit/534.3 (KHTML, like Gecko) Chrome/6.0.472.63 Safari/534.3 ChromePlus/1.4.2.0");
    http.post(req, ba);


    while(!isFinished){

        mySleep(30);
        QApplication::processEvents();
    }
    return content;
}

QString HttpGet::get(QUrl getUrl)
{
//	if(autoKeepCookie && cookieJar != NULL){
//		setCookieJar(cookieJar);
//	}
    content = "";
    isFinished = false;
    url = getUrl;
    QNetworkRequest req(getUrl);
    if(autoKeepReferer && !refererUrl.isEmpty()){
        req.setRawHeader("Referer", refererUrl.toStdString().c_str());
    }
    req.setRawHeader("User-Agent", "Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US) AppleWebKit/534.3 (KHTML, like Gecko) Chrome/6.0.472.63 Safari/534.3 ChromePlus/1.4.2.0");

    http.get(req);
    while(!isFinished)
    {
        mySleep(30);
        QApplication::processEvents();
    }
    return content;
}

QString HttpGet::get(const char* getUrl){
        QString u(getUrl);
        return get(u);
}

QString HttpGet::getUrl(){
    return this->url.toString();
}
QPixmap HttpGet::getPixMapFromUrl(QUrl)
{
    QPixmap pixmap;
    pixmap.loadFromData(this->imageByteArray);
    if (pixmap.isNull())
        qDebug("pixmap Null!!!");
    else
        return pixmap;
}

void HttpGet::initPixMap(QUrl url)
{
    QNetworkRequest request;
    request.setUrl(url);
    manager.get(request);
    connect(&manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(getPixMapFromUrlFinished(QNetworkReply*)));
}

void HttpGet::getPixMapFromUrlFinished(QNetworkReply *pReply)
{
    this->imageByteArray = pReply->readAll();

}

//sleep how many millisecond
void mySleep(int millisecond){
#if (WIN32)
        Sleep(millisecond);
#else
//linux
        usleep(millisecond*1000);
#endif
}
