#include "feedapi.h"

FeedApi::FeedApi()
{
}

void FeedApi::getFeed(int page)
{
    QMap<QString,QString> param;
    QString feed;
    param.clear();

    param.insert("access_token", account.getAccessToken());
    param.insert("count", feed.setNum(Config::page_count));
    param.insert("format", Config::ResultFormat);
    param.insert("method", Method::FEED_GET);
    param.insert("page", feed.setNum(page));
    param.insert("type","10,20,21,30,32,33,50");
    param.insert("v", Config::Version);

    QString sig = Utility::getSignature(param);
    param.insert("sig", sig);

    HttpGet http;
    feed = http.post(QUrl(Config::RESTServerURL), param);

    //qDebug("feed"+feed.toLocal8Bit());

    QDomDocument doc;
    QString errorStr;
    int errorLine;
    int errorCol;
    if(!doc.setContent(feed,true, &errorStr, &errorLine, &errorCol))
        return;
    QDomElement root = doc.documentElement();
    QDomNodeList nodes = root.elementsByTagName("feed_post");
    for(uint i = 0 ; i < nodes.length() ; i++)
    {
        QDomElement element = nodes.at(i).toElement();
        FeedInfo feedinfo;
        feedinfo.post_id = QString(QObject::tr(element.elementsByTagName("post_id").at(0).toElement().text().toUtf8().data()));
        feedinfo.source_id = QString(QObject::tr(element.elementsByTagName("source_id").at(0).toElement().text().toUtf8().data()));
        feedinfo.feed_type = QString(QObject::tr(element.elementsByTagName("feed_type").at(0).toElement().text().toUtf8().data()));
        feedinfo.update_time = QString(QObject::tr(element.elementsByTagName("update_time").at(0).toElement().text().toUtf8().data()));
        feedinfo.actor_id = QString(QObject::tr(element.elementsByTagName("actor_id").at(0).toElement().text().toUtf8().data()));
        feedinfo.name = QString(QObject::tr(element.elementsByTagName("name").at(0).toElement().text().toUtf8().data()));
        feedinfo.headurl = QString(QObject::tr(element.elementsByTagName("headurl").at(0).toElement().text().toUtf8().data()));
        feedinfo.prefix = QString(QObject::tr(element.elementsByTagName("prefix").at(0).toElement().text().toUtf8().data()));
        feedinfo.message = QString(QObject::tr(element.elementsByTagName("message").at(0).toElement().text().toUtf8().data()));
        feedinfo.title = QString(QObject::tr(element.elementsByTagName("title").at(0).toElement().text().toUtf8().data()));
        feedinfo.description = QString(QObject::tr(element.elementsByTagName("description").at(0).toElement().text().toUtf8().data()));
        QDomElement attachmentele = element.elementsByTagName("attachment").at(0).toElement();
        QDomNodeList medianodes =attachmentele.elementsByTagName("feed_media");

        for(uint j=0 ; j<medianodes.length() ; j++)
        {
            QDomElement mediaelement = medianodes.at(j).toElement();
            Feed_media singlemedia;
            singlemedia.href = QString(QObject::tr(mediaelement.elementsByTagName("href").at(0).toElement().text().toUtf8().data()));
            singlemedia.media_type = QString(QObject::tr(mediaelement.elementsByTagName("media_type").at(0).toElement().text().toUtf8().data()));
            singlemedia.src = QString(QObject::tr(mediaelement.elementsByTagName("src").at(0).toElement().text().toUtf8().data()));
            singlemedia.media_id = QString(QObject::tr(mediaelement.elementsByTagName("media_id").at(0).toElement().text().toUtf8().data()));
            singlemedia.owner_id = QString(QObject::tr(mediaelement.elementsByTagName("owner_id").at(0).toElement().text().toUtf8().data()));
            feedinfo.vfeed_media.push_back(singlemedia);
        }
        feedinfo.count = QString(QObject::tr(element.elementsByTagName("count").at(0).toElement().text().toUtf8().data()));

        this->vFeedInfo.push_back(feedinfo);
    }
}

void FeedApi::getFeed(int page,QString uid)
{
    vUserFeed.clear();
    QMap<QString,QString> param;
    QString feed;
    param.clear();

    param.insert("access_token", account.getAccessToken());
    param.insert("count", feed.setNum(5));
    param.insert("format", Config::ResultFormat);
    param.insert("method", Method::FEED_GET);
    param.insert("page", feed.setNum(page));
    param.insert("type","10,20,21,30,32,33,50");
    param.insert("uid", uid);
    param.insert("v", Config::Version);

    QString sig = Utility::getSignature(param);
    param.insert("sig", sig);

    HttpGet http;
    feed = http.post(QUrl(Config::RESTServerURL), param);

    //qDebug("feed"+feed.toLocal8Bit());

    QDomDocument doc;
    QString errorStr;
    int errorLine;
    int errorCol;
    if(!doc.setContent(feed,true, &errorStr, &errorLine, &errorCol))
        return;
    QDomElement root = doc.documentElement();
    QDomNodeList nodes = root.elementsByTagName("feed_post");
    for(uint i = 0 ; i < nodes.length() ; i++)
    {
        QDomElement element = nodes.at(i).toElement();
        FeedInfo feedinfo;
        feedinfo.post_id = QString(QObject::tr(element.elementsByTagName("post_id").at(0).toElement().text().toUtf8().data()));
        feedinfo.source_id = QString(QObject::tr(element.elementsByTagName("source_id").at(0).toElement().text().toUtf8().data()));
        feedinfo.feed_type = QString(QObject::tr(element.elementsByTagName("feed_type").at(0).toElement().text().toUtf8().data()));
        feedinfo.update_time = QString(QObject::tr(element.elementsByTagName("update_time").at(0).toElement().text().toUtf8().data()));
        feedinfo.actor_id = QString(QObject::tr(element.elementsByTagName("actor_id").at(0).toElement().text().toUtf8().data()));
        feedinfo.name = QString(QObject::tr(element.elementsByTagName("name").at(0).toElement().text().toUtf8().data()));
        feedinfo.headurl = QString(QObject::tr(element.elementsByTagName("headurl").at(0).toElement().text().toUtf8().data()));
        feedinfo.prefix = QString(QObject::tr(element.elementsByTagName("prefix").at(0).toElement().text().toUtf8().data()));
        feedinfo.message = QString(QObject::tr(element.elementsByTagName("message").at(0).toElement().text().toUtf8().data()));
        feedinfo.title = QString(QObject::tr(element.elementsByTagName("title").at(0).toElement().text().toUtf8().data()));
        feedinfo.description = QString(QObject::tr(element.elementsByTagName("description").at(0).toElement().text().toUtf8().data()));
        QDomElement attachmentele = element.elementsByTagName("attachment").at(0).toElement();
        QDomNodeList medianodes =attachmentele.elementsByTagName("feed_media");

        for(uint j=0 ; j<medianodes.length() ; j++)
        {
            QDomElement mediaelement = medianodes.at(j).toElement();
            Feed_media singlemedia;
            singlemedia.href = QString(QObject::tr(mediaelement.elementsByTagName("href").at(0).toElement().text().toUtf8().data()));
            singlemedia.media_type = QString(QObject::tr(mediaelement.elementsByTagName("media_type").at(0).toElement().text().toUtf8().data()));
            singlemedia.src = QString(QObject::tr(mediaelement.elementsByTagName("src").at(0).toElement().text().toUtf8().data()));
            singlemedia.media_id = QString(QObject::tr(mediaelement.elementsByTagName("media_id").at(0).toElement().text().toUtf8().data()));
            singlemedia.owner_id = QString(QObject::tr(mediaelement.elementsByTagName("owner_id").at(0).toElement().text().toUtf8().data()));
            feedinfo.vfeed_media.push_back(singlemedia);
        }
        feedinfo.count = QString(QObject::tr(element.elementsByTagName("count").at(0).toElement().text().toUtf8().data()));

        this->vUserFeed.push_back(feedinfo);
    }
}
