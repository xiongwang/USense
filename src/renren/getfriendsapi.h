#ifndef GETFRIENDSAPI_H
#define GETFRIENDSAPI_H

#include <QObject>
#include "config.h"
#include "renrenaccount.h"
#include "userapi.h"
#include "utility.h"
#include "method.h"
#include "httpget.h"
#include <vector>
#include <QtXml>

using namespace std;

class Friend
{
public:
    QString id;
    QString name;
    QString sex;
    QString headurl;
    QString tinyurl;
};
class GetFriendsApi : public QObject
{
    Q_OBJECT
public:
    explicit GetFriendsApi(QObject *parent = 0);
    RenRenAccount account;
    vector<Friend> vFriends;
    UserApi *pUserApi;

    void getFriendsFromAccount();
    vector<Friend> getSameFriends(QString uid);

signals:

public slots:

};

#endif // GETFRIENDSAPI_H
