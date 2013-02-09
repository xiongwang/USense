#ifndef STATUSAPI_H
#define STATUSAPI_H

#include <vector>
#include "httpget.h"
#include <QtXml>
#include "utility.h"
#include "method.h"
#include "renrenaccount.h"

using namespace std;

class StatusInfo
{
public:
    QString comment_count;
    QString message;
    QString forward_message;
    QString root_message;
    QString uid;
    QString time;
    QString srcURL;
    QString statusId;
    QString srcName;
    class Place
    {
    public:
        QString location;
        QString name;
        long lbsId;
        double longitude;
        double latitude;
        QString url;
    };
    Place place;
};

class PlaceInfo
{
public:
    enum Privacy {Private = 0, FriendsOnly = 1, Public = 2};
public:
    long placeId;
    double placeLongitude;
    double placeLatitude;
    double gpsLongitude;
    double gpsLatitude;
    QString placeName;
    QString placeLocation;
    Privacy privacy;
};

class CommentInfo
{
public:
    QString uid;
    QString name;
    QString tinyurl;
    QString comment_id;
    QString time;
    QString text;
};

class StatusApi:QObject
{
public:
    RenRenAccount account;
    StatusApi();
    QString GetFirstStatusFromId(QString uid);
    vector<StatusInfo> GetAllStatusFromId(QString uid);
    vector<CommentInfo> GetCommentsFromStatusId(QString status_id,QString owner_id);
    StatusInfo GetSingleStatus(QString status_id,QString owner_id);
    void StatusUpdate(QString status,QString forward_id = NULL,QString forward_owner = NULL);
    void StatusAddComment(QString status_id,QString owner_id ,QString content,QString rid);
};

#endif // STATUSAPI_H
