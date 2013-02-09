#ifndef RENRENSTATUSWIDGET_H
#define RENRENSTATUSWIDGET_H

#include <QWidget>
#include "renrenaccount.h"
#include "dealrenrenevent.h"
#include "emotionapi.h"

namespace Ui {
    class RenrenStatusWidget;
}

class RenrenStatusWidget : public QWidget
{
    Q_OBJECT

public:
    RenrenStatusWidget(RenRenAccount *account,QWidget* parent = 0);
    RenrenStatusWidget(RenRenAccount *account,int widgetType,QWidget* parent = 0);
    RenrenStatusWidget(RenRenAccount *account,QString arg_1,QString arg_2,int widgetType,QWidget* parent = 0);
    ~RenrenStatusWidget();

    void init();

    void setForward_id(QString);
    void setForward_owner(QString);
    void setStatus_id(QString);
    void setOwner_id(QString);
    void setReply_id(QString);

    void setAlbum_id(QString);
    void setPhoto_id(QString);
    void setShare_id(QString);
    void setToUser_id(QString);
    void setLog_id(QString);

    void setUgc_id(QString);
    void setShare_Url(QString);
    void setShare_Type(QString);

    void setWidgetType(int);


    void clearContent();
    void setContent(QString);
signals:
    void sendStatus(QString,QString,QString,QString,QString,int);

public slots:
    void sendButtonClicked();
    void atButtonClicked();
    void emotionButtonClicked();
    void cancelButtonClicked();
    void setHtmlByRow(QModelIndex);
    void addEmotionSlot(QString);

    void addJavaScriptObject();

private:
    void connectSlot();
    void setHtml(int row);

private:

    QString forward_id;
    QString forward_owner;
    QString status_id;
    QString owner_id;
    QString reply_id;

    QString album_id;
    QString photo_id;
    QString share_id;
    QString toUser_id;
    QString log_id;

    QString ugc_id;
    QString share_Url;
    QString share_Type;

    RenRenAccount *account;
    Dealrenrenevent *dealRenrenEvent;
    int widgetType;
    Ui::RenrenStatusWidget *ui;
};

#endif // RENRENSTATUSWIDGET_H
