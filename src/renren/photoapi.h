#ifndef PHOTOAPI_H
#define PHOTOAPI_H

#include <QObject>
#include "config.h"
#include "renrenaccount.h"
#include "utility.h"
#include "method.h"
#include "httpget.h"
#include <vector>
#include <QtXml>

using namespace std;
class PhotoInfo
{
public:
    QString pid;
    QString aid;
    QString uid;
    QString url_tiny;
    QString url_head;
    QString url_large;
    QString caption;
    QString time;
    QString view_count;
    QString comment_count;
};
class PhotoApi : public QObject
{
    Q_OBJECT
public:
    RenRenAccount account;
    explicit PhotoApi(QObject *parent = 0);
    vector<PhotoInfo> GetPhotosFromId(QString id,QString uid);
    vector<PhotoInfo> GetPhotoAlbumFromId(QString aid,QString uid);
    void AddComment(QString content,QString aid,QString pid,QString uid,QString rid);
signals:

public slots:

};

#endif // PHOTOAPI_H
