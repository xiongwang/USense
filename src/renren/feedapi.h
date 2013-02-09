#ifndef FEEDAPI_H
#define FEEDAPI_H

#include "config.h"
#include "renrenaccount.h"
#include "utility.h"
#include "method.h"
#include "httpget.h"
#include <vector>
#include <QtXml>

using namespace std;

class Feed_media
{
public:
    QString href;
    QString media_type;
    QString src;
    QString media_id;
    QString owner_id;
};

class Comment
{
public:
    QString uid;
    QString name;
    QString headurl;
    QDateTime time;
    QString comment_id;
    QString text;
};

class Comments
{
public:
    int count;
    vector<Comment> comment;
};

class Likes
{
public:
    QString total_count;
    QString friend_count;
    bool user_like;
};

class Place
{
public:
    QString lbs_id;
    QString name;
    QString address;
    QString url;
    QString longitude;
    QString latitude;
};

class FeedInfo
{
public:
    QString post_id;
    QString source_id;
    QString feed_type;
    QString update_time;
    QString actor_id;
    QString name;
    QString actor_type;
    QString headurl;
    QString prefix;
    QString message;
    QString title;
    QString href;
    QString description;
    vector<Feed_media> vfeed_media;
    QString count;
    Likes likes;
    Place place;
};

class FeedApi : public QObject
{
public:
    FeedApi();
    RenRenAccount account;
    vector<FeedInfo> vFeedInfo;
    vector<FeedInfo> vUserFeed;
    void getFeed(int page);
    void getFeed(int page, QString uid);

};

#endif // FEEDAPI_H
