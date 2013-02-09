

#ifndef SINAWEIBO_H
#define SINAWEIBO_H

#include <QObject>
#include <QIcon>
#include <QtNetwork>
#include "account.h"
#include "sinaparserxml.h"
#include "sinaservices.h"
#include "status.h"
#include "configuration.h"
#include "oauth.h"
#include <src/mynetworkcookiejar.h>

/*! \class SinaWeibo
 * \brief 微博信息处理与显示
 *
 *
 */
class SinaWeibo : public QObject
{
    Q_OBJECT
public:
    SinaWeibo(Account* account);
    ~SinaWeibo();
    bool verifyCredentials();
    QIcon getProfileIcon(const QString &urlString);

    enum DMType{
        List = 1,
        Session = 2
    };

public:
    QString getFriendsTimeline( qint64 sinceId=0, qint64 maxId=0, int count=20, \
                            int page=1, int baseApp=0, int feature=0);
    QString getMentions(qint64 sinceId=0, qint64 maxId=0, int count=20, int page=1);
    QString getUserTimeline(QString userId="0", int count=20, int page=1, int baseApp=0, int feature=0 );
    QString getUserTimelineByScreenName(QString screenName, int count=20, int page=1, int baseApp=0, int feature=0);
    QString getFavorites(int page=1);
    QString getCommentTimeline(qint64 sinceId=0, qint64 maxId=0, int count=20, int page=1);
    QString getDirectMessage(qint64 sinceId=0, qint64 maxId=0, int count=20, int page=1);

    QString getSendDirectMessage(qint64 sinceId=0, qint64 maxId=0, int count=20, int page=1);

    QString getEmotions();
    QString getUnread(QString sinceId);
    QString getStatusCountsByJson(QList<Status> &statusList);
    QString statusTextFormat(QString statusText);
    QString timeFormatHtml(QDateTime datetime);
    QString getCommentsList(QString id, int page=1, int count=20);

    QString getPublicStatus(int count=20);
    QString getDailyTrends();

    QString getFollowers(QString);
    QString getFriends(QString);
    QString getSingleStatus(QString);
    QString getSquareHtml();
    QString getLongUrl(const QList<QString>& shortUrlList);
    QString getImgAndVedioHtml(const Status& status);



    void getDMSessionStatus(QList<Status> &statusList,QString senderId);

    void setCommentsAndRetweetNum(QList<Status> &statusList);
    void setVedioPic(QList<Status> &statusList);
    void setVedioPic(QMap<QString,Status> &retwStatusMap);

    bool getSinaAccountByID(Account &account,QString id);
    static bool compareDateTime(const Status &statusOne,const Status &statusTwo);
public:    
    QString homePageStatusToHtml(QList<Status> &statusList, QMap<QString,Status> &retwStatusMap);
    QString atMePageStatusToHtml(QList<Status> &statusList, QMap<QString,Status> &retwStatusMap);
    QString userWeiboPageStatusToHtml(QList<Status> &statusList, QMap<QString,Status> &retwStatusMap);
    QString favoritePageStatusToHtml(QList<Status> &statusList, QMap<QString,Status> &retwStatusMap);
    QString commentPageStatusToHtml(QList<Status> &statusList, QMap<QString,Status> &retwStatusMap);
    QString directMessagePageStatusToHtml(QList<Status> &statusList,DMType dmType);
    QString commentsListToHtml(QList<Status> &statusList);
    QString statusToHtmlForNotifier(Status &status, Status &retwStatus);
    QString statusToHtmlForNotifier(Status &status);
    QString FriendsAndFollowersToHtml(QList<Account>&);
    QString othersCommentsToHtml(QList<Status>&);
    QString dailyTrendsToHtml(QStringList& nameList,QStringList& queryList);


    void initHomePageStatus();
    void initAtMePageStatus();
    void initUserWeiboPageStatus();
    void initFavoritePageStatus();
    void initCommentPageStatus();
    void initDirectMessagePageStatus();

    void initPublicStatus();
    
    void initEmotions();
    int sendStatusWithoutPicture(QString statusContent);
    int sendStatusWithPicture(QString statusContent, QByteArray pictureInByteArray, QString fileName);
    int repost(QString id, QString status);
    int commentStatus(QString id, QString status);
    int addFavorite(QString id);
    int deleteFavorite(QString id);
    int deleteStatus(QString id);
    void getUnreadInfo(int &statusUnread, int &atMeMentionsUnread, 
                          int &commentsUnread, int &directMessageUnread, int &newFollower);
    int resetCount(int type);
    int replyComment(QString cid, QString id, QString statusText);
    int sendDirectMessage(QString uid, QString statusText);
public:
    QString contentFrame;
signals:

public slots:
    
private:
    Account *account;
    SinaServices *servicesInstance;
    QString basicAuthInfo;
    Configuration *conf;
    /*
    QString contentFrame;
    */
    QString everyStatusHtml;
    QString retwStatusHtml;

private:
    void setBasicAuth(QNetworkRequest &request);
    void setOAuthAuthorize( QNetworkRequest &request, OAuth::HttpMethod httpMethod, QString url, QMap<QString,QString> &params);

};

#endif // SINAWEIBO_H
