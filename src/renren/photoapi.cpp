#include "photoapi.h"

PhotoApi::PhotoApi(QObject *parent) :
    QObject(parent)
{
}

vector<PhotoInfo> PhotoApi::GetPhotosFromId(QString id,QString uid)
{
    QMap<QString,QString> param;
    QString httpresult;
    param.clear();

    param.insert("access_token", account.getAccessToken());
    param.insert("method", Method::PHOTOS_GET);
    param.insert("pids",id);
    param.insert("uid",uid);
    param.insert("v", Config::Version);

    QString sig = Utility::getSignature(param);
    param.insert("sig", sig);

    HttpGet http;
    httpresult = http.post(QUrl(Config::RESTServerURL), param);

    vector<PhotoInfo> vPhotoInfo;
    QDomDocument doc;
    QString errorStr;
    int errorLine;
    int errorCol;
    doc.setContent(httpresult,true, &errorStr, &errorLine, &errorCol);
    QDomElement root = doc.documentElement();
    QDomNodeList nodes = root.elementsByTagName("photo");
    for(uint i = 0 ; i < nodes.length() ; i++)
    {
        QDomElement element = nodes.at(i).toElement();
        PhotoInfo photoinfo;
        photoinfo.pid = QString(QObject::tr(element.elementsByTagName("pid").at(0).toElement().text().toUtf8().data()));
        photoinfo.aid = QString(QObject::tr(element.elementsByTagName("aid").at(0).toElement().text().toUtf8().data()));
        photoinfo.uid = QString(QObject::tr(element.elementsByTagName("uid").at(0).toElement().text().toUtf8().data()));
        photoinfo.url_tiny = QString(QObject::tr(element.elementsByTagName("url_tiny").at(0).toElement().text().toUtf8().data()));
        photoinfo.url_head = QString(QObject::tr(element.elementsByTagName("url_head").at(0).toElement().text().toUtf8().data()));
        photoinfo.url_large = QString(QObject::tr(element.elementsByTagName("url_large").at(0).toElement().text().toUtf8().data()));
        photoinfo.caption = QString(QObject::tr(element.elementsByTagName("caption").at(0).toElement().text().toUtf8().data()));
        photoinfo.time = QString(QObject::tr(element.elementsByTagName("time").at(0).toElement().text().toUtf8().data()));
        photoinfo.view_count = QString(QObject::tr(element.elementsByTagName("view_count").at(0).toElement().text().toUtf8().data()));
        photoinfo.comment_count = QString(QObject::tr(element.elementsByTagName("comment_count").at(0).toElement().text().toUtf8().data()));

        vPhotoInfo.push_back(photoinfo);
    }
    return vPhotoInfo;
}

vector<PhotoInfo> PhotoApi::GetPhotoAlbumFromId(QString aid, QString uid)
{
    QMap<QString,QString> param;
    QString httpresult;
    param.clear();

    param.insert("access_token", account.getAccessToken());
    param.insert("aid",aid);
    param.insert("method", Method::PHOTOS_GET);
    param.insert("uid",uid);
    param.insert("v", Config::Version);

    QString sig = Utility::getSignature(param);
    param.insert("sig", sig);

    HttpGet http;
    httpresult = http.post(QUrl(Config::RESTServerURL), param);

    qDebug(httpresult.toLocal8Bit());

    vector<PhotoInfo> vPhotoInfo;
    QDomDocument doc;
    QString errorStr;
    int errorLine;
    int errorCol;
    doc.setContent(httpresult,true, &errorStr, &errorLine, &errorCol);
    QDomElement root = doc.documentElement();
    QDomNodeList nodes = root.elementsByTagName("photo");
    for(uint i = 0 ; i < nodes.length() ; i++)
    {
        QDomElement element = nodes.at(i).toElement();
        PhotoInfo photoinfo;
        photoinfo.pid = QString(QObject::tr(element.elementsByTagName("pid").at(0).toElement().text().toUtf8().data()));
        photoinfo.aid = QString(QObject::tr(element.elementsByTagName("aid").at(0).toElement().text().toUtf8().data()));
        photoinfo.uid = QString(QObject::tr(element.elementsByTagName("uid").at(0).toElement().text().toUtf8().data()));
        photoinfo.url_tiny = QString(QObject::tr(element.elementsByTagName("url_tiny").at(0).toElement().text().toUtf8().data()));
        photoinfo.url_head = QString(QObject::tr(element.elementsByTagName("url_head").at(0).toElement().text().toUtf8().data()));
        photoinfo.url_large = QString(QObject::tr(element.elementsByTagName("url_large").at(0).toElement().text().toUtf8().data()));
        photoinfo.caption = QString(QObject::tr(element.elementsByTagName("caption").at(0).toElement().text().toUtf8().data()));
        photoinfo.time = QString(QObject::tr(element.elementsByTagName("time").at(0).toElement().text().toUtf8().data()));
        photoinfo.view_count = QString(QObject::tr(element.elementsByTagName("view_count").at(0).toElement().text().toUtf8().data()));
        photoinfo.comment_count = QString(QObject::tr(element.elementsByTagName("comment_count").at(0).toElement().text().toUtf8().data()));

        vPhotoInfo.push_back(photoinfo);
    }
    return vPhotoInfo;
}

void PhotoApi::AddComment(QString content,QString aid,QString pid,QString uid,QString rid)
{
    QMap<QString,QString> param;
    QString httpresult;
    param.clear();

    param.insert("access_token", account.getAccessToken());
    param.insert("aid",aid);
    param.insert("content",content);
    param.insert("method", Method::PHOTOS_ADDCOMMENT);
    param.insert("pid",pid);
    param.insert("rid",rid);
    param.insert("uid",uid);
    param.insert("v", Config::Version);

    QString sig = Utility::getSignature(param);
    param.insert("sig", sig);

    HttpGet http;
    httpresult = http.post(QUrl(Config::RESTServerURL), param);
}
