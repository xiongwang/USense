#ifndef CONFIG_H
#define CONFIG_H

#include <qstring.h>

class Config
{
public:
    static const QString APPID;
    static const QString APIKey;
    static const QString Secret;
    static const QString Version;
    static const QString UserInfo;
    static const QString ResultFormat;
    static const QString RESTServerURL;
    static const QString Scope;
    static const int page_count;
};

#endif // CONFIG_H
