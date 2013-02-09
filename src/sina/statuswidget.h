#ifndef STATUSWIDGET_H
#define STATUSWIDGET_H

#include <QWidget>
#include <QDialog>
#include <QtCore>
#include <QCompleter>
#include "status.h"
#include "dealwebkitevent.h"
#include "emotion.h"
#include "account.h"

namespace Ui {
    class StatusWidget;
}

class StatusWidget : public QWidget
{
    Q_OBJECT

public:
    StatusWidget(Account *account,QWidget* parent = 0);//发表新微博
    StatusWidget(Account *account,QString &retwId, QString &appendText,QWidget* parent = 0);//转发微博
    StatusWidget(Account *account, QString &commentStatusId,QWidget* parent = 0);//评论微博
    StatusWidget(Account *account, QString &cid, QString &id, int dialogType,QWidget* parent = 0);//回复评论
    StatusWidget(Account *account, QString uid, int dialogType,QWidget* parent = 0);//发送私信
    ~StatusWidget();
    void setTitle(QString title);
public slots:
    void sendButtonClicked();
    void cancelButtonClicked();
    void trendsButtonClicked();
    void addPictureButtonClicked();
    void wordsCount();
    void addEmotionClicked();
    void addJavaScriptObject();
    void setHtmlByRow(QModelIndex modelIndex);
    void insertPhaseText(QString phase);
    void deletePicture();
    void clearContent();

    void sendStatus(QString statusText);
    void sendStatus(QString statusText, QByteArray pictureDataInByteArray, QString fileName);
    void repost(QString id, QString text);
    void commentStatus(QString id, QString text);
    void replyComment(QString cid, QString id, QString text);
    void sendDirectMessage(QString uid, QString text);

    void setText(QString text);
    void slotInsertString(QString);
    void freeSendButton();
    void mentionButtonClicked();


signals:

private:
    Account *account;
    Ui::StatusWidget *ui;
    DealWebkitEvent *dealWebkitEvent;
    QString emotionListToHtml(QList<Emotion> &list);
    QList<Emotion> getEmotionListByCategoryName(QString categoryName);
    int dialogType;/* 0---SendStatus  1-----RetwStatus  2-----Comment */
    QString pictureFileName;
    QByteArray pictureInByteArray;
    int wordsLimit;
    QString id;
    QString cid;

    void setHtml(int row);
    void connectSlot();
    void init();

protected:
    void closeEvent(QCloseEvent *e);

};

#endif // STATUSWIDGET_H
