#ifndef BLOGAPI_H
#define BLOGAPI_H

#include <QObject>
#include "config.h"
#include "renrenaccount.h"
#include "utility.h"
#include "method.h"
#include "httpget.h"
#include <vector>
#include <QtXml>

using namespace std;
class BlogComment
{
public:
    QString id;
    QString uid;
    QString name;
    QString headurl;
    QString time;
    QString content;
    QString is_whisper;
};
class BlogInfo
{
public:
    QString id;
    QString uid;
    QString name;
    QString headurl;
    QString title;
    QString time;
    QString content;
    QString view_count;
    QString comment_count;
    QString visalbe;
    BlogComment comment;
};
class BlogApi : public QObject
{
    Q_OBJECT
public:
    explicit BlogApi(QObject *parent = 0);
    RenRenAccount account;
    BlogInfo GetBlogFromId(QString id,QString uid);
    vector<BlogInfo> GetAllBlogsFromUid(QString uid);
    vector<BlogComment> GetComment(QString id,QString uid);
    void AddComment(QString content,QString id,QString uid,QString rid);

signals:

public slots:

};

#endif // BLOGAPI_H
