#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QList>
#include <QMap>
#include "status.h"

/*! \class Account
 * \brief 用户帐号
 *
 * 新浪微博用户的一些信息
 */
class Account
{
public:
    Account();
    
    void setUsername(QString username){this->username = username;}
    QString getUsername(){ return this->username; }
    void setPassword(QString password){ this->password = password; }
    QString getPassword(){ return this->password; }
    void setId(QString id){ this->id = id; }
    QString getId(){ return this->id; }
    void setScreenName(QString screenName){ this->screenName = screenName; }
    QString getScreenName(){ return this->screenName; }
    void setName(QString name){ this->name = name;}
    QString getName(){ return this->name; }
    void setProfileImageUrl(QString profileImageUrl){ this->profileImageUrl = profileImageUrl;}
    QString getProfileImageUrl()const{ return this->profileImageUrl; }
    void setLocation(QString location){this->location = location;}
    QString getLocation(){return this->location;}
    void setDescription(QString description){this->description = description;}
    QString getDescription(){return this->description;}
    void setGender(QString gender){this->gender = gender;}
    QString getGender(){return this->gender;}

    void setVerified( bool verified ){ this->verified = verified;}
    bool isVerified(){ return this->verified; }

    void setFriends( int friends ){ this->friends = friends; }
    int getFriends(){ return this->friends; }
    void setFollower( int follower ){ this->follower = follower; }
    int getFollower(){ return this->follower; }
    void setWeiboCount( int weiboCount ){ this->weiboCount = weiboCount; }
    int getWeiboCount(){ return this->weiboCount; }
    void setFavourites(int favourites){this->favourites = favourites;}
    int getFavourites(){return this->favourites;}


    
    void StatusManagerAdd(QList<Status> &addToStatusList, QMap<QString, Status> &addToRetwMap,
                          Status &status, Status &retwStatus, int limit, bool isStart);
    void StatusManagerRemove(QList<Status> &statusListRemoveOne, QMap<QString, Status> &RetwMap);
    void RemoveAll(QList<Status> &statusList, QMap<QString, Status> &retwMap);
    void RemoveAll(QList<Status> &statusList);

    static bool updateStatus(QList<Status> &statusList, QMap<QString, Status> &retwMap,
                                QList<Status> &sourceList, QMap<QString, Status> &sourceMap);
    
signals:

public slots:
    
private:
    QString username;
    QString password;
    QString id;//用户id
    QString screenName;//昵称
    QString name;//同微博昵称
    QString profileImageUrl;//用户头像
    QString location;
    QString description;
    QString gender;
    bool verified;//是否认证
    int friends;//关注数
    int follower;//粉丝数
    int weiboCount;//发表微博数
    int favourites;

public:
    QList<Status> homePageStatus;
    QList<Status> atMePageStatus;
    QList<Status> userWeiboPageStatus;
    QList<Status> favoritesPageStatus;
    QList<Status> commentPageComments;//comment
    QList<Status> directMessagePageStatus;
    QList<Status> directMessagePageStatusInSession;
    QList<Status> publicStatus;
    QMap<QString, Status> homePageRetweeted;
    QMap<QString, Status> atMePageRetweeted;
    QMap<QString, Status> userWeiboPageRetweeted;
    QMap<QString, Status> favoritesPageRetweeted;
    QMap<QString, Status> commentPageRetweeted;//status, no retweeted status
    QMap<QString, Status> publicRetweeted;
};

#endif // ACCOUNT_H
