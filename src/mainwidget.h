#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include "sina/sinawidget.h"
#include "rss/rssreader.h"
#include "renren/renrenwidget.h"
#include "loginwidget.h"

#define Client LoginWidget::Client
#define None   LoginWidget::None
#define RenRen LoginWidget::RenRen
#define Sina   LoginWidget::Sina
#define Login  LoginWidget::Login
#define Rss    LoginWidget::Rss
#define App    LoginWidget::App


namespace Ui {
    class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();
private:
    enum Position {
        Left = 0,
        Up = 1
    };
protected:
    void mousePressEvent(QMouseEvent *);
    QByteArray toQByteArray(int);
    void loadClient(Client client);
    void removeCurrentClient();
    int currentSelected;
    void initSinaWidget(Account*);
    void initRenrenWidget(RenRenAccount);
    void initLoginWidget();
    void showLoginWidget();
    void showSinaWidget();
    void showRenrenWidget();
    void setBackGroundPic(QString);

private:
    Ui::MainWidget *ui;
    QPalette palette;
    Client currentClient;
    SinaWidget *pSinaWidget;
    LoginWidget *pLoginWidget;
    RssReader *pRssReader;
    RenrenWidget *pRenrenWidget;
    QList<Client> clientList;
    QPixmap renrenPhoto;
    QPixmap sinaPhoto;
    QPixmap sinaPixmap;
    QPixmap sinaLightedPixmap;
    QPixmap renrenPixmap;
    QPixmap renrenLightedPixmap;
    QPixmap appPixmap;
    QPixmap appLightedPixmap;
    QPixmap bgBeforeLogin;
    QPixmap bgAfterLogin;
    QMovie movie;


    const static int widget_widthInX  = 80;
    const static int widget_heightInX = 80;
    const static int label_widthInX   = 60;
    const static int label_heightInX  = 60;

    const static int widget_widthInY  = 60;
    const static int widget_heightInY = 60;
    const static int label_widthInY   = 50;
    const static int label_heightInY  = 50;

    const static int distanceInX   = 10;
    const static int distanceInY   = 5;

signals:
    void clicked(Position pos,int index);
    void sendSignalToSina(int index);
    void sendSignalToRenren(int index);
    void forward_to_otheClientSignal(QString str);

public slots:
    void loadStartedSlot();
    void loadFinishedSlot(bool isFinished);
    void movieStart();
    void movieFinished(bool);
    void showErrorMessage(QString);
    void hideErrorMessage();
    void hideLostFocusWidget(QWidget*,QWidget*);

private slots:
    void response(Position pos ,int index);
    void loginSuccessed(Client client);
    void forwardToOtherClientSlot(QString);

protected:
    void closeEvent(QCloseEvent *);
};

#endif // MAINWIDGET_H
