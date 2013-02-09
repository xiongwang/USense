#include "configuration.h"
#include <QFileInfo>
#include <QDir>
#include <QSettings>

Configuration::Configuration()
{
    QString confFileName;
    QDir softwareHome;
    QDir mk;
#if defined(Q_WS_X11)
    softwareHome = QDir::home();
    mk.setPath(softwareHome.absolutePath()+"/.USense");
    if(!mk.exists())  
    {
        softwareHome.mkdir(".USense");
    }
    this->profilePath = softwareHome.absolutePath()+"/.USense";
    confFileName = mk.absolutePath()+"/USense.conf";
#endif
#if defined(Q_WS_WIN)
    softwareHome = QDir::home();
    mk.setPath(softwareHome.absolutePath()+"/profiles");
    if(!mk.exists())
        softwareHome.mkdir("profiles");
    confFileName = mk.absolutePath()+"/USense.ini";
    profilePath = softwareHome.absolutePath()+"/profiles";
#endif
#if defined(Q_OS_LINUX)
    softwareHome = QDir::home();
    mk.setPath(softwareHome.absolutePath() + "/.USense");
    if (!mk.exists())
    {
        softwareHome.mkdir(".USense");
    }
    this->profilePath = softwareHome.absolutePath()+"/.USense";
    confFileName = mk.absolutePath() + "/USense.conf";
    qDebug("confFileName = " + confFileName.toLocal8Bit());
#endif
    QFile confFile(confFileName);
    /*
    confFile.open(QFile::ReadWrite);
    confFile.close();
    */
    this->homeCS = "HomePageStatusesCount";
    this->atMeCS =  "AtMePageStatusesCount";
    this->userWeiboCS = "UserWeiboPageStatusesCount";
    this->favoriteCS = "FavoritePageStatusesCount";
    this->commentCS = "CommentPageStatusesCount";
    this->directMessageCS = "DirectMessagePageCount";
    this->updateStatusS = "UpdateTime";
    this->isNotifyCS = "Notify";
    this->settings = new QSettings(confFileName, QSettings::IniFormat);
}

Configuration* Configuration::instance = NULL;
const QString Configuration::accessToken = "AccessToken";
const QString Configuration::accessTokenSecret = "AccessTokenSecret";

QString Configuration::getRemeberedUsername()
{
    QString up(QByteArray::fromBase64(settings->value("Remebered/UP").toByteArray()));
    QStringList upList = up.split(QRegExp(":"));
    if(upList.size()==2)
        return upList.at(0);
    return "";
}

QString Configuration::getRemeberedPassword()
{
    QString up(QByteArray::fromBase64(settings->value("Remebered/UP").toByteArray()));
    QStringList upList = up.split(QRegExp(":"));
    if(upList.size()==2)
        return upList.at(1);
    return "";
}

void Configuration::saveAccountConf(QString id, int homeC, int atMeC, int userC, 
                                    int commentC, int dMC, int updateTime)
{
    settings->beginGroup(id);
    settings->setValue(this->homeCS, homeC);
    settings->setValue(this->atMeCS, atMeC);
    settings->setValue(this->userWeiboCS, userC);
    settings->setValue(this->commentCS, commentC);
    settings->setValue(this->updateStatusS, updateTime);
    settings->setValue(this->directMessageCS, dMC);
    settings->endGroup();
    settings->sync();
}

int Configuration::getAccountConf(QString id, QString key)
{
    int t = settings->value(id+"/"+key, 0).toInt();
    if(t==0&&key!=this->updateStatusS)
    {
        settings->setValue(id+"/"+key, 20);
        settings->sync();
        return 20;
    }
    if(t==0&&key==this->updateStatusS)
    {
        settings->setValue(id+"/"+key, 150);//150s
        settings->sync();
        return 150;
    }
    return t;
}

bool Configuration::loadSettings() //load from config file
{
    
    return true;
}

bool Configuration::saveSettings()  //save to config file
{
    settings->sync();
    return true;
}

Configuration* Configuration::getInstance()
{
    if(!instance) 
        instance = new Configuration();
    return instance;
}

void Configuration::setRemebemberedUP(QString username, QString password)
{
//    this->settings->setValue("Remebered/Username", username);
//    this->settings->setValue("Remebered/Password", password);
    QString tmp = username + ":" + password;
    this->settings->setValue("Remebered/UP", tmp.toAscii().toBase64());
    this->settings->sync();
}

QString Configuration::getProfilesDir()
{
    return this->profilePath;
}

/*! \brief 根据Email地址获取保存的AccessToken AccessTokenSecret
 *  \param emailAddress Email地址
 *  \return 是否成功
 */
bool Configuration::getTokenByEmailAddress(const QString &emailAddress, QString &accessToken, QString &accessTokenSecret)
{
    accessToken = settings->value(emailAddress+"/"+Configuration::accessToken, "").toString();
    accessTokenSecret = settings->value(emailAddress+"/"+Configuration::accessTokenSecret, "").toString();
    if(!accessToken.isEmpty()&&!accessTokenSecret.isEmpty())
    {
        return true;
    }
    return false;
}

/*! \brief 保存AccessToken AccessTokenSecret
 *
 */
void Configuration::saveTokenByEmailAddress(const QString &emailAddress, const QString &accessToken, const QString &accessTokenSecret)
{
    settings->setValue(emailAddress+"/"+Configuration::accessToken, accessToken);
    settings->setValue(emailAddress+"/"+Configuration::accessTokenSecret, accessTokenSecret);
    this->saveSettings();
}
