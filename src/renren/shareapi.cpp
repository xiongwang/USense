#include "shareapi.h"

ShareApi::ShareApi(QObject *parent) :
    QObject(parent)
{
}

void ShareApi::Share(QString comment, QString type,  QString ugc_id, QString user_id, QString url)
{
    QMap<QString,QString> param;
    QString httpresult;
    param.clear();

    param.insert("access_token", account.getAccessToken());
    param.insert("comment", comment);
    param.insert("method", "share.share");
    param.insert("type", type);
    param.insert("ugc_id", ugc_id);
    param.insert("url", url);
    param.insert("user_id", user_id);
    param.insert("v", Config::Version);

    QString sig = Utility::getSignature(param);
    param.insert("sig", sig);

    HttpGet http;
    httpresult = http.post(QUrl(Config::RESTServerURL), param);

    qDebug(httpresult.toLocal8Bit());
}

void ShareApi::AddComment(QString content,QString share_id,QString user_id,QString to_user_id)
{
    float call_id =QDateTime::currentMSecsSinceEpoch();
    QMap<QString,QString> param;
    QString httpresult;
    param.clear();

    param.insert("api_key", Config::APIKey);
    param.insert("call_id", QString::number(call_id));
    param.insert("content", content);
    param.insert("method", "share.addComment");
    param.insert("session_key", this->account.getSessionKey());
    param.insert("share_id", share_id);
    param.insert("to_user_id", to_user_id);
    param.insert("user_id", user_id);
    param.insert("v", Config::Version);

    QString sig = Utility::getSignature(param);
    param.insert("sig", sig);

    HttpGet http;
    httpresult = http.post(QUrl(Config::RESTServerURL), param);

    qDebug(httpresult.toLocal8Bit());
}
