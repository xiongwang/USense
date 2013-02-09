#ifndef DEALWEBKITEVENT_H
#define DEALWEBKITEVENT_H

#include <QObject>
#include "account.h"

/*! \class DealWebkitEvent
 * \brief 处理与webkit的交互
 *
 *
 */
class DealWebkitEvent : public QObject
{
    Q_OBJECT
public:
    explicit DealWebkitEvent(Account *account, QObject *parent = 0);

signals:
    void hideDivById(QString id);
    void insertPhaseText(QString phase);
    void getUserTimelineByScreenNameSignal(QString screenName, int page);
    void getMoreHomePageStatusSignal(int page, int feature);
    void getMoreUserPageStatusSignal(int page, int feature);
    void getMoreAtMePageStatusSignal(int page);
    void getMoreCommentsPageStatusSignal(int page);
    void getMoreFavoritePageStatusSignal(int page);
    void getMoreDirectMessagePageStatusSignal(int page);
    void showBigPicture(QString bigUrl,QString midUrl);
    void parserStatusesCounts(QString jsonContent);
    void setCommentsListHtml(QString html, QString id);
    void getNewPageInfoSignal(QString page, int feature);
    void getSessionStatusSignal(QString);

    void forwardToOtherClientSignal(QString);

    void showUserInfoSignal(QString);
    void showFriendsSignal(QString);
    void showFollowersSignal(QString);
    void showCommentsSignal(QString userId,QString statusId);
    void showPublicStatusSignal();
    void showDailyTrendsSignal();
    void playVedioSignal(QString);



public slots:
    void retwStatusAction(QString idText, QString appendText);
    void openUrl(QString url);
    void commentStatus(QString id);
    void replyComment(QString cid, QString id);
    void addEmotionPhase(QString phase);
    void addFavorite(QString id);
    void sendDirectMessage(QString id);
    void deleteFavorite(QString id);
    void deleteMyStatus(QString id);
    void getUserTimelineByScreenName(QString screenName, int page);
    void getMoreHomePageStatus(int page, int feature);
    void getMoreUserPageStatus(int page, int feature);
    void getMoreAtMePageStatus(int page);
    void getMoreCommentsPageStatus(int page);
    void getMoreFavoritePageStatus(int page);
    void getMoreDirectMessagePageStatus(int page);
    void getNewPageInfo(QString page, int feature);
    void bigPicture(QString bigUrl,QString midUrl);
    void openTrends(QString trendsName);
    void setDomReady(bool b);
    void sendStatusesCounts(QString jsonContent);
    void getCommentsList(QString id, int page);

    void forwardToOtherClient(QString);

    void getSessionStatus(QString);

    void showUserInfoSlot(QString);
    void showFriendsSlot(QString);
    void showFollowersSlot(QString);
    void showCommentsSlot(QString userId,QString statusId);
    void showPublicStatusSlot();
    void showDailyTrendsSlot();

    void sendStatusWithAtSlot(QString username);
    void playVedioSlot(QString vedioUrl);
    
public:
    bool isDomReady();
private:
    Account *account;
    QWidget* parentWidget;
    bool domReady;
};

#endif // DEALWEBKITEVENT_H
