#ifndef USERAPI_H
#define USERAPI_H

#include <vector>
#include "httpget.h"
#include <QtXml>
#include "utility.h"
#include "method.h"
#include "renrenaccount.h"

using namespace std;

class ProfileInfo
{
public:
    long uid;
    QString name;
    QString headurl;
    QString star;
    QString network_name;
    class Base_info
    {
    public:
        QString gender;
        class Birth
        {
        public:
            QString birth_year;
            QString birth_month;
            QString birth_day;
        };
        class Hometown
        {
        public:
            QString province;
            QString city;
        };
        Birth birthday;
        Hometown hometown;
    };

    class Status
    {
    public:
        QString content;
        QString time;
    };

    Base_info base_info;
    Status status;
    long visitors_count;
    long blogs_count;
    long albums_count;
    long friends_count;
    long guestbook_count;
    long status_count;
};

class WorkInfo
{
public:
    QString company;
    QString description;
    QString startDate;
    QString endDate;
};
class UnivInfo
{
public:
    QString name;
    unsigned long year;
    QString department;
};
class HsInfo
{
public:
    QString name;
    unsigned long year;
};

class UserInfo
{
public:
    class Hometown
    {
    public:
        QString country;
        QString province;
        QString city;
    };

    long uid;
    QString name;
    QString sex;     // male: true; female: false;
    QString star;
    QString zidou;
    QString vip;
    QString birthday;
    QString email_hash;
    QString tinyurl;
    QString headurl;
    QString mainurl;
    QString headURLWithLogo;
    QString tinyURLWithLogo;
    Hometown hometown;
    UnivInfo univInfo;
    vector<WorkInfo> vWorkHistory;
    vector<UnivInfo> vUnivHistory;
    vector<HsInfo> vHsHistory;
};

class UserApi
{
public:
    UserApi();
    RenRenAccount account;
    long getLoggedInUser();
    UserInfo getUserInfo(QString uid);
    bool hasAppPermission();
    bool isAppUser();
    void getProfileInfo(QString uid);
    ProfileInfo profileInfo;
};

#endif // USERAPI_H
