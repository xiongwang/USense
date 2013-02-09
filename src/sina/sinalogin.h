#ifndef SINALOGIN_H
#define SINALOGIN_H

#include "account.h"
#include "sinaservices.h"
#include "configuration.h"

#include <QMessageBox>

class SinaLogin : public QObject{
    Q_OBJECT
public:
    SinaLogin (Account* pAccount);
    ~SinaLogin();
public:
    bool getAccessToken(QString &accessToken, QString &accessTokenSecret);
    void startLogin();
    void loginFailed();
public slots:
    void checkAuthStart();
signals:
    void loginSuccessed();
private:
    Account *account;
    SinaServices *sinaServices;
    Configuration *conf;
};

#endif // SINALOGIN_H
