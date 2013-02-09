#include "getfriendsapi.h"

GetFriendsApi::GetFriendsApi(QObject *parent) :
    QObject(parent)
{
}

vector<Friend> GetFriendsApi::getSameFriends(QString uid)
{
    this->pUserApi = new UserApi();
    vector<Friend> vSameFriends;
    QMap<QString,QString> param;
    QString strFriends;
    param.clear();

    param.insert("access_token", account.getAccessToken());
    param.insert("fields", "uid,name,tinyurl");
    param.insert("format", Config::ResultFormat);
    param.insert("method", Method::FRIENDS_GETSAMEFRIENDS);
    param.insert("uid1", account.getUid());
    param.insert("uid2", uid);
    param.insert("v", Config::Version);

    QString sig = Utility::getSignature(param);
    param.insert("sig", sig);

    HttpGet http;
    strFriends = http.post(QUrl(Config::RESTServerURL), param);

    //qDebug(strFriends.toLocal8Bit());
    QDomDocument doc;
    QString errorStr;
    int errorLine;
    int errorCol;
    if(!doc.setContent(strFriends,true, &errorStr, &errorLine, &errorCol))
        return vSameFriends;
    QDomElement root = doc.documentElement();
    QDomNodeList nodes = root.elementsByTagName("friend");
    vector<Friend>::iterator it;
    for(uint i = 0 ; i < nodes.length(); i++)
    {
        QDomElement element = nodes.at(i).toElement();
        Friend singlefriend;
        singlefriend.id = QString(QObject::tr(element.elementsByTagName("uid").at(0).toElement().text().toUtf8().data()));
        singlefriend.name = QString(QObject::tr(element.elementsByTagName("name").at(0).toElement().text().toUtf8().data()));
        //singlefriend.tinyurl = QString(QObject::tr(element.elementsByTagName("tinyurl").at(0).toElement().text().toUtf8().data()));
        //singlefriend.tinyurl = pUserApi->getUserInfo(singlefriend.id).tinyurl;

        for(it = this->vFriends.begin(); it != this->vFriends.end(); it++)
        {
            if(it->id == singlefriend.id)
            {
                singlefriend.tinyurl = it->tinyurl;
                break;
            }
        }
        vSameFriends.push_back(singlefriend);
    }

    return vSameFriends;
}

void GetFriendsApi::getFriendsFromAccount()
{
    this->vFriends.clear();
    QMap<QString,QString> param;
    QString strFriends;
    param.clear();

    param.insert("access_token", account.getAccessToken());
    param.insert("format", Config::ResultFormat);
    param.insert("method", Method::FRIENDS_GETFRIENDS);
    param.insert("v", Config::Version);

    QString sig = Utility::getSignature(param);
    param.insert("sig", sig);

    HttpGet http;
    strFriends = http.post(QUrl(Config::RESTServerURL), param);

    QDomDocument doc;
    QString errorStr;
    int errorLine;
    int errorCol;
    if(!doc.setContent(strFriends,true, &errorStr, &errorLine, &errorCol))
        return;
    QDomElement root = doc.documentElement();
    QDomNodeList nodes = root.elementsByTagName("friend");
    for(uint i = 0 ; i < nodes.length() ; i++)
    {
        QDomElement element = nodes.at(i).toElement();
        Friend singlefriend;
        singlefriend.id = QString(QObject::tr(element.elementsByTagName("id").at(0).toElement().text().toUtf8().data()));
        singlefriend.name = QString(QObject::tr(element.elementsByTagName("name").at(0).toElement().text().toUtf8().data()));
        singlefriend.headurl = QString(QObject::tr(element.elementsByTagName("headurl").at(0).toElement().text().toUtf8().data()));
        singlefriend.tinyurl = QString(QObject::tr(element.elementsByTagName("tinyurl").at(0).toElement().text().toUtf8().data()));

        this->vFriends.push_back(singlefriend);
    }
}


