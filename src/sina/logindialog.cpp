

#include "logindialog.h"
#include "sinaweibo.h"
#include "sinaservices.h"
#include "ui_logindialog.h"

#include "oauth.h"
#include <QMovie>
#include <QMessageBox>

LoginDialog::LoginDialog(Account *account) :
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    ui->label_loadingText->hide();
    ui->label_loadingText->setAlignment(Qt::AlignCenter);
    this->account = account;
    QObject::connect(ui->pushButton_Login, SIGNAL(clicked()), this, SLOT(checkAuthStart()));

    sinaServices = SinaServices::getInstance();
    if(!conf->getRemeberedPassword().isEmpty()&&!conf->getRemeberedUsername().isEmpty())
    {
        ui->lineEdit_Username->setText(conf->getRemeberedUsername());
        ui->lineEdit_Password->setText(conf->getRemeberedPassword());
        ui->checkBox_RemeberPwd->setChecked(true);
    }
    this->movie = new QMovie(this);
    this->movie->setFileName(":/toolButton/images/loading.gif");
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::checkAuthStart()
{    
    startLogin();

    account->setUsername(ui->lineEdit_Username->text());
    account->setPassword(ui->lineEdit_Password->text());
    
    //oauth
    QString accessToken, accessTokenSecret;
    if(!getAccessToken(accessToken, accessTokenSecret))
    {
        loginFailed();
        return;
    }
    
    SinaWeibo sina(this->account);;
    if(sina.verifyCredentials()) //success
    {
        if(ui->checkBox_RemeberPwd->isChecked())
        {
            conf->setRemeberedUP(ui->lineEdit_Username->text(), 
                                 ui->lineEdit_Password->text());
        }
        SinaWeibo *sina = new SinaWeibo(account);
        sina->initHomePageStatus();
        sina->initEmotions();
        delete sina;
        this->accept();
    }
    else
    {
        loginFailed();
    }
}

/*! \brief 从配置文件中获取AccessToken和AccessTokenSecret,如果配置文件中没有则获取用户授权 
 *  \return 是否成功获取AccessToken
 */
bool LoginDialog::getAccessToken(QString &accessToken, QString &accessTokenSecret)
{
    OAuth oauth;
    bool result;
    result = conf->getTokenByEmailAddress(account->getUsername(), accessToken, accessTokenSecret);
    if(!result)
    {
        result = oauth.startOAuthAuthorzationAuto(account->getUsername(), account->getPassword(),
                                                   accessToken, accessTokenSecret);
        if(result)
        {
            sinaServices->setServices(SinaServices::MyAccessToken, accessToken);
            sinaServices->setServices(SinaServices::MyAccessTokenSecret, accessTokenSecret);
            conf->saveTokenByEmailAddress(account->getUsername(), accessToken, accessTokenSecret);
        } else {
            QMessageBox::warning(this, "获取授权失败", "获取授权失败", QMessageBox::Ok, QMessageBox::Ok);
        }
    } else {
        sinaServices->setServices(SinaServices::MyAccessToken, accessToken);
        sinaServices->setServices(SinaServices::MyAccessTokenSecret, accessTokenSecret);
    }
    return result;
}

/*! \brief 登录失败的处理
 *
 */
void LoginDialog::loginFailed()
{
    ui->label_loading->setPixmap(QPixmap(":/logo/images/qlisa.png"));
    ui->label_loadingText->setText("登录失败");
    ui->pushButton_Login->setEnabled(true);
    ui->pushButton_Login->setText("登录");
}

/*! \brief 开始登录过程时界面处理
 *
 */
void LoginDialog::startLogin()
{
    if(sinaServices->getServices("Source").isEmpty()||sinaServices->getServices("Secret").isEmpty())
    {
        QMessageBox::warning(this, "没有AppKey(AppSecret)",
                             "没有检测到AppKey(AppSecret)\n新浪禁止泄漏AppKey,您从源码中编译的程序将不能正常运行."
                             "你可以自己申请AppKey(AppSecret)或者下载编译好的程序.",QMessageBox::Ok,QMessageBox::Ok);
        return ;
    }
    ui->pushButton_Login->setEnabled(false);
    ui->pushButton_Login->setText("登录中...");
    this->movie->start();
    ui->label_loading->clear();
    ui->label_loading->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    ui->label_loading->setMovie(this->movie);
    ui->label_loadingText->setVisible(true);
}
