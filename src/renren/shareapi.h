#ifndef SHAREAPI_H
#define SHAREAPI_H

#include <QObject>
#include "config.h"
#include "renrenaccount.h"
#include "utility.h"
#include "method.h"
#include "httpget.h"
#include <vector>
#include <QtXml>
#include <QDateTime>

class ShareApi : public QObject
{
    Q_OBJECT
public:
    explicit ShareApi(QObject *parent = 0);
    RenRenAccount account;
    void Share(QString comment,QString type,QString ugc_id,QString user_id,QString url);
    void AddComment(QString content,QString share_id,QString user_id,QString to_user_id);
signals:

public slots:

};

#endif // SHAREAPI_H
