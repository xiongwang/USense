#ifndef CONFIGURATION_H
#define CONFIGURATION_H
#include <QSettings>

/*! \class Configuration
 * \brief 应用程序配置信息
 *
 *
 */
class Configuration
{
public:
    Configuration();
    bool saveSettings();
    bool loadSettings();
    void saveAccountConf(QString id, int homeC, int atMeC, int userC, int commentC, int dMC, int updateTime);
    int getAccountConf(QString id, QString key);
    void setAccountCont(QString id, QString key, QVariant value);
    bool getTokenByEmailAddress( const QString &emailAddress, QString &accessToken, QString &accessTokenSecret );
    void saveTokenByEmailAddress( const QString &emailAddress, const QString &accessToken, const QString &accessTokenSecret );
    QString getRemeberedUsername();
    QString getRemeberedPassword();
    void setRemebemberedUP(QString username, QString password);
    QString getProfilesDir();
   static Configuration* getInstance();
   
public:
    static Configuration* instance;
    static const QString accessToken;
    static const QString accessTokenSecret;
    QSettings *settings;
    QString homeCS;
    QString atMeCS;
    QString userWeiboCS;
    QString favoriteCS;
    QString commentCS;
    QString directMessageCS;
    QString isNotifyCS;
    QString id;
    QString profilePath;
    QString updateStatusS;
};

#endif // CONFIGURATION_H
