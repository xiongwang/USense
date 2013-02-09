#include "blogapi.h"

BlogApi::BlogApi(QObject *parent) :
    QObject(parent)
{
}

BlogInfo BlogApi::GetBlogFromId(QString id, QString uid)
{
    QMap<QString,QString> param;
    QString httpresult;
    param.clear();

    param.insert("access_token", account.getAccessToken());
    param.insert("id", id);
    param.insert("method", Method::BLOG_GET);
    param.insert("uid", uid);
    param.insert("v", Config::Version);

    QString sig = Utility::getSignature(param);
    param.insert("sig", sig);

    HttpGet http;
    httpresult = http.post(QUrl(Config::RESTServerURL), param);

    BlogInfo bloginfo;
    QDomDocument doc;
    QString errorStr;
    int errorLine;
    int errorCol;
    doc.setContent(httpresult,true, &errorStr, &errorLine, &errorCol);
    QDomElement root = doc.documentElement();
    bloginfo.id = QString(QObject::tr(root.elementsByTagName("id").at(0).toElement().text().toUtf8().data()));
    bloginfo.uid = QString(QObject::tr(root.elementsByTagName("uid").at(0).toElement().text().toUtf8().data()));
    bloginfo.name = QString(QObject::tr(root.elementsByTagName("name").at(0).toElement().text().toUtf8().data()));
    bloginfo.headurl = QString(QObject::tr(root.elementsByTagName("headurl").at(0).toElement().text().toUtf8().data()));
    bloginfo.title = QString(QObject::tr(root.elementsByTagName("title").at(0).toElement().text().toUtf8().data()));
    bloginfo.time = QString(QObject::tr(root.elementsByTagName("time").at(0).toElement().text().toUtf8().data()));
    bloginfo.content = QString(QObject::tr(root.elementsByTagName("content").at(0).toElement().text().toUtf8().data()));
    bloginfo.view_count = QString(QObject::tr(root.elementsByTagName("view_count").at(0).toElement().text().toUtf8().data()));
    bloginfo.comment_count = QString(QObject::tr(root.elementsByTagName("comment_count").at(0).toElement().text().toUtf8().data()));
    bloginfo.visalbe = QString(QObject::tr(root.elementsByTagName("visalbe").at(0).toElement().text().toUtf8().data()));

    return bloginfo;
}

vector<BlogInfo> BlogApi::GetAllBlogsFromUid(QString uid)
{
    QMap<QString,QString> param;
    QString httpresult;
    param.clear();

    param.insert("access_token", account.getAccessToken());
    param.insert("method", Method::BLOG_GETS);
    param.insert("uid", uid);
    param.insert("v", Config::Version);

    QString sig = Utility::getSignature(param);
    param.insert("sig", sig);

    HttpGet http;
    httpresult = http.post(QUrl(Config::RESTServerURL), param);

    vector<BlogInfo> vbloginfo;
    QDomDocument doc;
    QString errorStr;
    int errorLine;
    int errorCol;
    doc.setContent(httpresult,true, &errorStr, &errorLine, &errorCol);
    QDomElement root = doc.documentElement();
    QDomNodeList nodes = root.elementsByTagName("blog");
    for(uint i = 0 ; i < nodes.length() ; i++)
    {
        QDomElement element = nodes.at(i).toElement();
        BlogInfo bloginfo;
        bloginfo.id = QString(QObject::tr(element.elementsByTagName("id").at(0).toElement().text().toUtf8().data()));
        bloginfo.time = QString(QObject::tr(element.elementsByTagName("time").at(0).toElement().text().toUtf8().data()));
        bloginfo.title = QString(QObject::tr(element.elementsByTagName("title").at(0).toElement().text().toUtf8().data()));
        bloginfo.content = QString(QObject::tr(element.elementsByTagName("content").at(0).toElement().text().toUtf8().data()));
        bloginfo.view_count = QString(QObject::tr(element.elementsByTagName("view_count").at(0).toElement().text().toUtf8().data()));
        bloginfo.comment_count = QString(QObject::tr(element.elementsByTagName("comment_count").at(0).toElement().text().toUtf8().data()));

        vbloginfo.push_back(bloginfo);
    }
    return vbloginfo;
}

void BlogApi::AddComment(QString content,QString id,QString uid,QString rid)
{
    QMap<QString,QString> param;
    QString httpresult;
    param.clear();

    param.insert("access_token", account.getAccessToken());
    param.insert("content", content);
    param.insert("id", id);
    param.insert("method", Method::BLOG_ADDCOMMENT);
    param.insert("rid", rid);
    param.insert("uid", uid);
    param.insert("v", Config::Version);

    QString sig = Utility::getSignature(param);
    param.insert("sig", sig);

    HttpGet http;
    httpresult = http.post(QUrl(Config::RESTServerURL), param);

    qDebug(httpresult.toLocal8Bit());
}

vector<BlogComment> BlogApi::GetComment(QString id,QString uid)
{
    QMap<QString,QString> param;
    QString httpresult;
    param.clear();

    param.insert("access_token", account.getAccessToken());
    param.insert("id", id);
    param.insert("method", Method::BLOG_GETCOMMENTS);
    param.insert("uid", uid);
    param.insert("v", Config::Version);

    QString sig = Utility::getSignature(param);
    param.insert("sig", sig);

    HttpGet http;
    httpresult = http.post(QUrl(Config::RESTServerURL), param);

    vector<BlogComment> vBlogComments;
    QDomDocument doc;
    QString errorStr;
    int errorLine;
    int errorCol;
    doc.setContent(httpresult,true, &errorStr, &errorLine, &errorCol);
    QDomElement root = doc.documentElement();
    QDomNodeList nodes = root.elementsByTagName("comment");
    for(uint i = 0 ; i < nodes.length() ; i++)
    {
        QDomElement element = nodes.at(i).toElement();
        BlogComment blogComment;
        blogComment.uid = QString(QObject::tr(element.elementsByTagName("uid").at(0).toElement().text().toUtf8().data()));
        blogComment.headurl = QString(QObject::tr(element.elementsByTagName("headurl").at(0).toElement().text().toUtf8().data()));
        blogComment.name = QString(QObject::tr(element.elementsByTagName("name").at(0).toElement().text().toUtf8().data()));
        blogComment.time = QString(QObject::tr(element.elementsByTagName("time").at(0).toElement().text().toUtf8().data()));
        blogComment.content = QString(QObject::tr(element.elementsByTagName("content").at(0).toElement().text().toUtf8().data()));

        vBlogComments.push_back(blogComment);
    }
    return vBlogComments;
}
