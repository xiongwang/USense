#include "userapi.h"

UserApi::UserApi()
{
}

UserInfo UserApi::getUserInfo(QString uid)
{
    UserInfo userinfo;

    QMap<QString,QString> param;
    QString info;
    param.clear();

    param.insert("api_key", Config::APIKey);
    param.insert("fields", Config::UserInfo);
    param.insert("format", Config::ResultFormat);
    param.insert("method", Method::USERS_GETINFO);
    param.insert("session_key", account.getSessionKey());
    param.insert("uids", uid);
    param.insert("v", Config::Version);

    QString sig = Utility::getSignature(param);
    param.insert("sig", sig);

    HttpGet http;
    info = http.post(QUrl(Config::RESTServerURL), param);

    QDomDocument doc;
    QString errorStr;
    int errorLine;
    int errorCol;
    doc.setContent(info,true, &errorStr, &errorLine, &errorCol);
    QDomElement root = doc.documentElement();
    QDomNodeList nodes = root.elementsByTagName("user");
    QDomElement element = nodes.at(0).toElement();

    userinfo.name = QString(QObject::tr(element.elementsByTagName("name").at(0).toElement().text().toUtf8().data()));
    userinfo.tinyurl = QString(QObject::tr(element.elementsByTagName("tinyurl").at(0).toElement().text().toUtf8().data()));
    userinfo.headurl = QString(QObject::tr(element.elementsByTagName("headurl").at(0).toElement().text().toUtf8().data()));
    userinfo.birthday = QString(QObject::tr(element.elementsByTagName("birthday").at(0).toElement().text().toUtf8().data()));
    userinfo.sex = QString(QObject::tr(element.elementsByTagName("sex").at(0).toElement().text().toUtf8().data()));
    userinfo.mainurl = QString(QObject::tr(element.elementsByTagName("mainurl").at(0).toElement().text().toUtf8().data()));
    userinfo.hometown.city = QString(QObject::tr(element.elementsByTagName("hometown_location").at(0).toElement().elementsByTagName("city").at(0).toElement().text().toUtf8().data()));
    userinfo.hometown.country = QString(QObject::tr(element.elementsByTagName("hometown_location").at(0).toElement().elementsByTagName("country").at(0).toElement().text().toUtf8().data()));
    userinfo.hometown.province = QString(QObject::tr(element.elementsByTagName("hometown_location").at(0).toElement().elementsByTagName("province").at(0).toElement().text().toUtf8().data()));
    userinfo.univInfo.name = QString(QObject::tr(element.elementsByTagName("university_info").at(0).toElement().elementsByTagName("name").at(0).toElement().text().toUtf8().data()));
    userinfo.univInfo.year = QString(QObject::tr(element.elementsByTagName("university_info").at(0).toElement().elementsByTagName("year").at(0).toElement().text().toUtf8().data())).toLong();
    userinfo.univInfo.department = QString(QObject::tr(element.elementsByTagName("university_info").at(0).toElement().elementsByTagName("department").at(0).toElement().text().toUtf8().data()));

    //qDebug("mainurl "+userinfo.mainurl.toLocal8Bit());
    return userinfo;
}

void UserApi::getProfileInfo(QString uid)
{
    QMap<QString,QString> param;
    QString result;
    param.clear();

    param.insert("access_token", account.getAccessToken());
    param.insert("fields","blogs_count,albums_count,status_count");
    param.insert("method", Method::USERS_GETPROFILEINFO);
    param.insert("uid", uid);
    param.insert("v", Config::Version);

    QString sig = Utility::getSignature(param);
    param.insert("sig", sig);

    HttpGet http;
    result = http.post(QUrl(Config::RESTServerURL), param);

    //qDebug("profileInfo"+result.toLocal8Bit());

    QDomDocument doc;
    QString errorStr;
    int errorLine;
    int errorCol;
    if(!doc.setContent(result,true, &errorStr, &errorLine, &errorCol))
        return;
    QDomElement root = doc.documentElement();

    profileInfo.uid = QString(QObject::tr(root.elementsByTagName("uid").at(0).toElement().text().toUtf8().data())).toLong();
    profileInfo.albums_count = QString(QObject::tr(root.elementsByTagName("albums_count").at(0).toElement().text().toUtf8().data())).toLong();
    profileInfo.status_count = QString(QObject::tr(root.elementsByTagName("status_count").at(0).toElement().text().toUtf8().data())).toLong();
    profileInfo.blogs_count = QString(QObject::tr(root.elementsByTagName("blogs_count").at(0).toElement().text().toUtf8().data())).toLong();
    /*
    profileInfo.base_info.birthday.birth_year = QString(QObject::tr(root.elementsByTagName("base_info").at(0).toElement().elementsByTagName("birth").at(0).toElement().elementsByTagName("birth_year").at(0).toElement().text().toUtf8().data()));
    profileInfo.base_info.birthday.birth_month = QString(QObject::tr(root.elementsByTagName("base_info").at(0).toElement().elementsByTagName("birth").at(0).toElement().elementsByTagName("birth_year").at(0).toElement().text().toUtf8().data()));
    profileInfo.base_info.birthday.birth_day = QString(QObject::tr(root.elementsByTagName("base_info").at(0).toElement().elementsByTagName("birth").at(0).toElement().elementsByTagName("birth_year").at(0).toElement().text().toUtf8().data()));
    profileInfo.base_info.gender = QString(QObject::tr(root.elementsByTagName("base_info").at(0).toElement().elementsByTagName("gender").at(0).toElement().text().toUtf8().data()));
    profileInfo.base_info.hometown.city = QString(QObject::tr(root.elementsByTagName("base_info").at(0).toElement().elementsByTagName("birth").at(0).toElement().elementsByTagName("birth_year").at(0).toElement().text().toUtf8().data()));
    profileInfo.base_info.hometown.province = QString(QObject::tr(root.elementsByTagName("base_info").at(0).toElement().elementsByTagName("birth").at(0).toElement().elementsByTagName("birth_year").at(0).toElement().text().toUtf8().data()));
    profileInfo.name = QString(QObject::tr(root.elementsByTagName("name").at(0).toElement().text().toUtf8().data()));
    profileInfo.headurl = QString(QObject::tr(root.elementsByTagName("headurl").at(0).toElement().text().toUtf8().data()));
    profileInfo.star = QString(QObject::tr(root.elementsByTagName("star").at(0).toElement().text().toUtf8().data()));
    profileInfo.network_name = QString(QObject::tr(root.elementsByTagName("network_name").at(0).toElement().text().toUtf8().data()));
    */
    //qDebug("albums "+sig.setNum(profileInfo.albums_count).toLocal8Bit());
}
