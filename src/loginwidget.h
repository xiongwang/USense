#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QWidget>
#include <qmap.h>

#include "sina/account.h"
#include "sina/sinaservices.h"
#include "sina/sinalogin.h"
#include "renren/renrenaccount.h"
#include "renren/oauth2.h"
#include "renren/feedapi.h"
#include "clientwidget.h"
#include "appwidget.h"

namespace Ui {
    class LoginWidget;
}

class LoginWidget : public QWidget
{
    Q_OBJECT

public:
    enum Client {
        None   = 0,
        Login  = 1,
        RenRen = 2,
        Sina   = 3,
        Rss    = 4,
        App    = 5
    };

    explicit LoginWidget(QWidget *parent = 0);
    ~LoginWidget();

private:
    enum Position {
        Left = 0,
        Up = 1
    };

public slots:
    void sinaLoginSuccessed();
    void renrenLoginSuccessed();
    void startLogin(QString,QString);
    void rssRun();
    void changeCurrentClient(Client client);
    Client getCurrentClient()const{return this->currentClient;}
signals:
    void sendToMainWidget(Client client);
    void CheckRenrenLogin();
    void startLoginSignal();
public:
    Account* getSinaAccount();
private:
    Ui::LoginWidget *ui;
    SinaLogin *pSinaLogin;
    Account *pAccount;
    RenRenAccount *prenrenAccount;
    OAuth2 *pOAuth;
    FeedApi *pFeedApi;
    ClientWidget *pClientWidget;
    AppWidget *pAppWidget;
    Client currentClient;



 public:
    RenRenAccount* getRenRenAccount();
};

#endif // LOGINWIDGET_H
