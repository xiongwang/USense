#include "statusapi.h"

StatusApi::StatusApi()
{
}

QString StatusApi::GetFirstStatusFromId(QString uid)
{
    QMap<QString,QString> param;
    QString firststatus;
    param.clear();

    param.insert("access_token", account.getAccessToken());
    param.insert("format", Config::ResultFormat);
    param.insert("method", Method::STATUS_GETS);
    param.insert("uid", uid);
    param.insert("v", Config::Version);

    QString sig = Utility::getSignature(param);
    param.insert("sig", sig);

    HttpGet http;
    firststatus = http.post(QUrl(Config::RESTServerURL), param);

    QDomDocument doc;
    QString errorStr;
    int errorLine;
    int errorCol;
    if(!doc.setContent(firststatus,true, &errorStr, &errorLine, &errorCol))
        return "";
    QDomElement root = doc.documentElement();
    QDomNodeList nodes = root.elementsByTagName("status");
    QDomElement element = nodes.at(0).toElement();
    StatusInfo status;
    status.message = QString(QObject::tr(element.elementsByTagName("message").at(0).toElement().text().toUtf8().data()));
    status.forward_message = QString(QObject::tr(element.elementsByTagName("forward_message").at(0).toElement().text().toUtf8().data()));
    status.comment_count = QString(QObject::tr(element.elementsByTagName("comment_count").at(0).toElement().text().toUtf8().data()));
    return status.message+status.forward_message;
}

vector<CommentInfo> StatusApi::GetCommentsFromStatusId(QString status_id, QString owner_id)
{
    QMap<QString,QString> param;
    QString comments;
    param.clear();

    param.insert("access_token", account.getAccessToken());
    param.insert("format", Config::ResultFormat);
    param.insert("method", Method::STATUS_GETCOMMENT);
    param.insert("owner_id", owner_id);
    param.insert("status_id", status_id);
    param.insert("v", Config::Version);

    QString sig = Utility::getSignature(param);
    param.insert("sig", sig);

    HttpGet http;
    comments = http.post(QUrl(Config::RESTServerURL), param);

    vector<CommentInfo> vComments;

    QDomDocument doc;
    QString errorStr;
    int errorLine;
    int errorCol;
    if(!doc.setContent(comments,true, &errorStr, &errorLine, &errorCol))
        return vComments;
    QDomElement root = doc.documentElement();
    QDomNodeList nodes = root.elementsByTagName("comment");
    for(uint i = 0 ; i < nodes.length() ; i++)
    {
        QDomElement element = nodes.at(i).toElement();
        CommentInfo commentinfo;
        commentinfo.uid = QString(QObject::tr(element.elementsByTagName("uid").at(0).toElement().text().toUtf8().data()));
        commentinfo.name = QString(QObject::tr(element.elementsByTagName("name").at(0).toElement().text().toUtf8().data()));
        commentinfo.tinyurl = QString(QObject::tr(element.elementsByTagName("tinyurl").at(0).toElement().text().toUtf8().data()));
        commentinfo.comment_id = QString(QObject::tr(element.elementsByTagName("comment_id").at(0).toElement().text().toUtf8().data()));
        commentinfo.time = QString(QObject::tr(element.elementsByTagName("time").at(0).toElement().text().toUtf8().data()));
        commentinfo.text = QString(QObject::tr(element.elementsByTagName("text").at(0).toElement().text().toUtf8().data()));
        vComments.push_back(commentinfo);
    }
    return vComments;
}

StatusInfo StatusApi::GetSingleStatus(QString status_id,QString owner_id)
{
    StatusInfo statusinfo;
    QMap<QString,QString> param;
    QString singlestatus;
    param.clear();

    param.insert("access_token", account.getAccessToken());
    param.insert("format", Config::ResultFormat);
    param.insert("method", Method::STATUS_GET);
    param.insert("owner_id", owner_id);
    param.insert("status_id", status_id);
    param.insert("v", Config::Version);

    QString sig = Utility::getSignature(param);
    param.insert("sig", sig);

    HttpGet http;
    singlestatus = http.post(QUrl(Config::RESTServerURL), param);

    qDebug("singlestatus:");
    qDebug(singlestatus.toLocal8Bit());

    QDomDocument doc;
    QString errorStr;
    int errorLine;
    int errorCol;
    doc.setContent(singlestatus,true, &errorStr, &errorLine, &errorCol);
    QDomElement root = doc.documentElement();
    statusinfo.comment_count = QString(QObject::tr(root.elementsByTagName("comment_count").at(0).toElement().text().toUtf8().data()));
    statusinfo.time = QString(QObject::tr(root.elementsByTagName("time").at(0).toElement().text().toUtf8().data()));
    statusinfo.forward_message = QString(QObject::tr(root.elementsByTagName("forward_message").at(0).toElement().text().toUtf8().data()));
    statusinfo.message = QString(QObject::tr(root.elementsByTagName("message").at(0).toElement().text().toUtf8().data()));
    statusinfo.root_message = QString(QObject::tr(root.elementsByTagName("root_message").at(0).toElement().text().toUtf8().data()));

    return statusinfo;
}

void StatusApi::StatusUpdate(QString status, QString forward_id, QString forward_owner)
{
    QMap<QString,QString> param;
    QString result;
    param.clear();

    param.insert("access_token", account.getAccessToken());
    param.insert("forward_id", forward_id);
    param.insert("forward_owner",forward_owner);
    param.insert("format", Config::ResultFormat);
    param.insert("method", Method::STATUS_SET);
    param.insert("status", status);
    param.insert("v", Config::Version);

    QString sig = Utility::getSignature(param);
    param.insert("sig", sig);

    HttpGet http;
    result = http.post(QUrl(Config::RESTServerURL), param);

    qDebug(result.toLocal8Bit());
}

void StatusApi::StatusAddComment(QString status_id, QString owner_id, QString content, QString rid)
{
    QMap<QString,QString> param;
    QString result;
    param.clear();

    param.insert("access_token", account.getAccessToken());
    param.insert("content",content);
    param.insert("method", Method::STATUS_ADDCOMMENT);
    param.insert("owner_id", owner_id);
    param.insert("rid", rid);
    param.insert("status_id", status_id);
    param.insert("v", Config::Version);

    QString sig = Utility::getSignature(param);
    param.insert("sig", sig);

    HttpGet http;
    result = http.post(QUrl(Config::RESTServerURL), param);
    qDebug(result.toLocal8Bit());
}

vector<StatusInfo> StatusApi::GetAllStatusFromId(QString uid)
{
    QMap<QString,QString> param;
    QString httpresult;
    param.clear();

    param.insert("access_token", account.getAccessToken());
    param.insert("method", Method::STATUS_GETS);
    param.insert("uid", uid);
    param.insert("v", Config::Version);

    QString sig = Utility::getSignature(param);
    param.insert("sig", sig);

    HttpGet http;
    httpresult = http.post(QUrl(Config::RESTServerURL), param);

    vector<StatusInfo> vstatusinfo;
    QDomDocument doc;
    QString errorStr;
    int errorLine;
    int errorCol;
    doc.setContent(httpresult,true, &errorStr, &errorLine, &errorCol);
    QDomElement root = doc.documentElement();
    QDomNodeList nodes = root.elementsByTagName("status");
    for(uint i = 0 ; i < nodes.length() ; i++)
    {
        QDomElement element = nodes.at(i).toElement();
        StatusInfo statusinfo;
        statusinfo.comment_count = QString(QObject::tr(element.elementsByTagName("comment_count").at(0).toElement().text().toUtf8().data()));
        statusinfo.time = QString(QObject::tr(element.elementsByTagName("time").at(0).toElement().text().toUtf8().data()));
        statusinfo.forward_message = QString(QObject::tr(element.elementsByTagName("forward_message").at(0).toElement().text().toUtf8().data()));
        statusinfo.message = QString(QObject::tr(element.elementsByTagName("message").at(0).toElement().text().toUtf8().data()));
        statusinfo.root_message = QString(QObject::tr(element.elementsByTagName("root_message").at(0).toElement().text().toUtf8().data()));
        statusinfo.statusId = QString(QObject::tr(element.elementsByTagName("status_id").at(0).toElement().text().toUtf8().data()));

        vstatusinfo.push_back(statusinfo);
    }
    return vstatusinfo;
}
