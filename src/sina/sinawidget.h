#ifndef SINAWIDGET_H
#define SINAWIDGET_H

#include "account.h"
#include "sinaweibo.h"
#include "dealwebkitevent.h"
#include "configuration.h"
#include "src/mywebview.h"
#include "statuswidget.h"
#include "showpicture.h"


#include <QSystemTrayIcon>
namespace Ui {
    class SinaWidget;
}

class SinaWidget : public QWidget
{
    Q_OBJECT

    enum WebViewPos{
        Left = 0,
        Right = 1
    };

public:
    explicit SinaWidget(Account* ,QWidget *parent = 0);
    ~SinaWidget();
    QString getImageUrl();
    QPixmap getPixmapFromUrl(QUrl);
    void initPixmap(QUrl url);
public slots:
    void prepare();
    void preHtml();
    void newWeiboButtonClicked();
    void homePageButtonClicked();
    void myWeiboPageButtonClicked();
    void atMePageButtonClicked();
    void commentPageButtonClicked();
    void favouritePageButtonClicked();
    void messagePageButtonClicked();
    void configureButtonClicked();
    void publicStatusesButtonClicked();
    void squareButtonClicked();

    void getNewPage(QString pageNumStr, int feature);
    void getOthersTimelineByScreenName(QString screenName, int page=1);
    void getMoreHomePageStatus(int page=1, int feature=0);
    void getMoreUserPageStatus(int page=1, int feature=0);
    void getMoreAtMePageStatus(int page=1);
    void getMoreCommentsPageStatus(int page=1);
    void getMoreFavoritePageStatus(int page=1);
    void getMoreDirectMessagePageStatus(int page=1);

    void refreshStatus();
    void updateIcon(QPixmap *pixmap, int unreadCount);
    void refreshSystemTrayIconTooltip();
    void showPictueInWidget(QString bigUrl, QString midUrl);
    void systemTrayIconAction(QSystemTrayIcon::ActivationReason reason);

    void addJavaScriptObject();
    void dealWebkitEventDisconnectThenConnect();

    void getSessionStatus(QString senderId);

    void forwardToOtherClientSlot(QString);
    void forward_from_otherClient(QString);

    void showUserInfo(QString);
    void showFriends(QString);
    void showFollowers(QString);
    void showComments(QString userId,QString statusId);
    void showpublicStatuses();
    void showDailyTrends();
    void playVedio(QString);

signals:

    void forwardToOtherClientSignal(QString);

public:
    MyWebView *leftWebView;
    MyWebView *rightWebView;

private:
    Ui::SinaWidget *ui;
    StatusWidget *pStatusWidget;
    Account *account;
    Account *otherAccount;
    QTimer *timer;
    DealWebkitEvent *dealWebkitEvent;
    QString basicHtml;
    QString homePageHtml;
    QString atMePageHtml;
    QString myWeiboPageHtml;
    QString favoritePageHtml;
    QString commentPageHtml;
    QString directMessagePageHtml;
    QString publicStatusesHtml;
    QString squareHtml;
    QString dailyTrendsHtml;

    bool publicStatusesHtmlReady;
    bool publicStatusesReady;

    bool squareHtmlReady;
    bool dailyTrendsHtmlReady;

    bool homePageHtmlReady;
    bool homePageStatusReady;
    bool atMePageHtmlReady;
    bool atMePageStatusReady;
    bool myWeiboPageHtmlReady;
    bool myWeiboPageStatusReady;
    bool favoritePageHtmlReady;
    bool favoritePageStatusReady;
    bool commentPageHtmlReady;
    bool commentPageStatusReady;
    bool directMessagePageHtmlReady;
    bool directMessagePageStatusReady;
    int statusUnread;
    int commentsUnread;
    int directMessageUnread;
    int newFollower;
    int atMeMentionsUnread;
    Configuration *conf;
    QSystemTrayIcon *systemTrayIcon;//系统托盘
    QMenu *systemTrayMenu;//系统托盘右键菜单
    QAction *quitAction;
    QAction *restoreAction;
    /* more button */
    QMenu *moreMenu;
    QAction *about;//关于
    QAction *feedBack;//反馈
    ShowPicture *showPicture;
    int currentSelected;
private:
    void createSystemTrayIcon();
    void setWebviewHtml(QString html,WebViewPos pos);

    bool getUserInfo(QString userId);
    void changeSelectedPixmap(int,int);
private slots:
    void receiveSignalFromMainWidget(int index);

private:
    QPixmap myFriendsPage;
    QPixmap myFriendsPageLighted;

    QPixmap atMePage;
    QPixmap atMePageLighted;

    QPixmap myWeiboPage;
    QPixmap myWeiboPageLighted;

    QPixmap newStatus;
    QPixmap newStatusLighted;

    QPixmap commentsPage;
    QPixmap commentsPageLighted;

    QPixmap myFavouritePage;
    QPixmap myFavouritePageLighted;

    QPixmap directMessage;
    QPixmap directMessageLighted;

    QPixmap publicPage;
    QPixmap publicPageLighted;

};

#endif // SINAWIDGET_H
