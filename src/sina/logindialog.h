

#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include "account.h"
#include "sinaservices.h"


namespace Ui {
    class LoginDialog;
}

/*! \class LoginDialog
 * \brief 登录对话框
 *
 *
 */
class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    LoginDialog(Account *account);
    ~LoginDialog();

private:
    Ui::LoginDialog *ui;
    Account *account;
    QMovie *movie;
    SinaServices *sinaServices;

    
public slots:
    void checkAuthStart();
    
public:
    bool getAccessToken(QString &accessToken, QString &accessTokenSecret);
    void loginFailed();
    void startLogin();
};

#endif // LOGINDIALOG_H
