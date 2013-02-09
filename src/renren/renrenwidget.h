#ifndef RENRENWIDGET_H
#define RENRENWIDGET_H

#include <QWidget>
#include <QtNetwork>
#include <qdatetime.h>
#include "renrenaccount.h"
#include "feedapi.h"
#include "getfriendsapi.h"
#include "httpget.h"
#include "utility.h"
#include "statusapi.h"
#include  "emotionapi.h"
#include "dealrenrenevent.h"
#include "userapi.h"
#include "renrenstatuswidget.h"
#include "blogapi.h"
#include "photoapi.h"
#include "shareapi.h"
#include "src/mywebview.h"

namespace Ui {
    class RenrenWidget;
}

class RenrenWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RenrenWidget(QWidget *parent = 0);
    ~RenrenWidget();  
    RenRenAccount *prenrenAccount;
    FeedApi *pFeedApi;
    GetFriendsApi *pGetFriendsApi;
    StatusApi *pStatusApi;
    EmotionApi *pEmotionApi;
    UserApi * pUserApi;
    BlogApi *pBlogApi;
    PhotoApi *pPhotoApi;
    ShareApi *pShareApi;
    MyWebView *leftWebView;
    MyWebView *rightWebView;

    vector<Friend> vSameFriend;
    void getFeed();
    void getFriends();
    void setCookie(QNetworkRequest &request);

    QPixmap getPixmap();

public slots:
    void addJavaScriptObject();
    void forward_from_otherClient(QString);
    void forwardToOtherClientSlot(QString);
signals:
    void forwardToOtherClientSignal(QString);

private:
    HttpGet httpGet;
    Ui::RenrenWidget *ui;
    Dealrenrenevent *dealRenrenEvent;
    RenrenStatusWidget *pStatusWidget;
    void SetFriendPage();

    void changeSelectedPixmap(int,int);
    int currentSelected;

    QPixmap myFriends;
    QPixmap myFriendsLighted;

    QPixmap newThings;
    QPixmap newThingsLighted;

    QPixmap sendStatus;
    QPixmap sendStatusLighted;

    QString statusPage;
    QString friendsPage;

private slots:
    void createStatusWidgetSlot(QString arg_1,QString arg_2,QString arg_3,QString arg_4,int widgetType);

    void StatusSlot(QString content,QString arg_1,QString arg_2,QString arg_3,QString arg_4,int widgetType);

    void setSameFriends();
    void receiveSignalFromMainWidget(int index);
    void ReadFeed(QString post_id);
    void SetNewPage(int page);
    void setProfilePage(QString uid);
    void setStatusPage(QString uid);
    void setAlbumsPage(QString uid);
    void setBlogsPage(QString uid);
    void ReadStatusFeed(QString status_id ,QString owner_id);
    void ReadBlogShareFeed(QString id ,QString uid,QString actor_id);
    void ReadBlogFeed(QString id,QString uid);
    void ReadPhotoUploadFeed(vector<Feed_media> vfeed_media);
    void ReadPhotoShareFeed(vector<Feed_media> vfeed_media);
    void ReadPhotoAlbumShareFeed(QString aid,QString uid);
    void PlayVideo(QString str);
};

#endif // RENRENWIDGET_H
