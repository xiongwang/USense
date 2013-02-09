#ifndef DEALRENRENEVENT_H
#define DEALRENRENEVENT_H

#include <QObject>
#include "renrenaccount.h"

class Dealrenrenevent : public QObject
{
    Q_OBJECT
public:
    explicit Dealrenrenevent(RenRenAccount *account, QObject *parent = 0);

    bool isDomReady();

signals:
    void getSameFriendsSignal();
    void getCommentsPageSignal(QString owner_id, QString status_id);
    void getShareInfoSignal(QString post_id);
    void getProfileInfoSignal(QString uid);
    void getStatusInfoSignal(QString uid);
    void getAlbumsInfoSignal(QString uid);
    void getBlogsInfoSignal(QString uid);
    void readStatusSignal(QString id, QString uid);
    void readBlogSignal(QString id, QString uid);
    void getNewPageSignal(int page);
    void addEmotionSignal(QString);
    void createStatusWidgetSignal(QString arg_1,QString arg_2,QString arg_3,QString arg_4,int widgetType);
    void forwardToOtherClientSignal(QString);
    void playVideoSignal(QString);

public slots:
    void getSameFriends();
    void getCommentsPage(QString owner_id,QString status_id);
    void getShareInfo(QString post_id);
    void getNewPageInfo(int page);
    void getProfileInfo(QString uid);
    void getStatusInfo(QString uid);
    void getAlbumsInfo(QString uid);
    void getBlogsInfo(QString uid);
    void readStatus(QString id, QString uid);
    void readBlog(QString id, QString uid);
    void setDomReady(bool b);
    void addEmotion(QString);
    void createStatusWidget(QString arg_1,QString arg_2,QString arg_3,QString arg_4,int widgetType);
    void forwardToOtherClient(QString);
    void playVideo(QString);

private:
    RenRenAccount *account;
    bool domReady;

};

#endif // DEALRENRENEVENT_H
