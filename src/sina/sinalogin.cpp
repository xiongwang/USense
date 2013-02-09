#include "sinalogin.h"

#include "sinaweibo.h"
#include "sinaservices.h"
#include "oauth.h"

SinaLogin::SinaLogin(Account *pAccount){

    this->account = pAccount;
    sinaServices = SinaServices::getInstance();
    conf = Configuration::getInstance();

    /*
    if(!conf->getRemeberedPassword().isEmpty()&&!conf->getRemeberedUsername().isEmpty())
    {
        ui->lineEdit_Username->setText(conf->getRemeberedUsername());
        ui->lineEdit_Password->setText(conf->getRemeberedPassword());
        ui->checkBox_Autologin->setChecked(true);
    }
    */

}

SinaLogin::~SinaLogin(){

}

void SinaLogin::checkAuthStart()
{
    startLogin();
    //oauth
    QString accessToken, accessTokenSecret;
    if(!getAccessToken(accessToken, accessTokenSecret))
    {
        loginFailed();
        return;
    }
    SinaWeibo sina(this->account);
    if(sina.verifyCredentials()) //success
    {
        /*
        if(ui->checkBox_Autologin->isChecked())
        {
            conf->setRemeberedUP(ui->lineEdit_Username->text(),
                                 ui->lineEdit_Password->text());
        }
        */
        /*
        SinaWeibo *sina = new SinaWeibo(account);
        sina->initHomePageStatus();
        sina->initEmotions();
        delete sina;
        */
        emit loginSuccessed();
    }
    else
    {
        loginFailed();
        return ;
    }
}

bool SinaLogin::getAccessToken(QString &accessToken, QString &accessTokenSecret)
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
            qDebug("获取授权失败！！！");

        }
    } else {
        sinaServices->setServices(SinaServices::MyAccessToken, accessToken);
        sinaServices->setServices(SinaServices::MyAccessTokenSecret, accessTokenSecret);
    }
    return result;
}

void SinaLogin::loginFailed(){
    QMessageBox::warning(NULL,"登录失败","登录失败，请检查用户名密码以及网络",QMessageBox::Ok);
}

void SinaLogin::startLogin()
{
    if(sinaServices->getServices("Source").isEmpty()||sinaServices->getServices("Secret").isEmpty())
    {
        QMessageBox::warning(NULL, "没有AppKey(AppSecret)",
                             "没有检测到AppKey(AppSecret)\n新浪禁止泄漏AppKey,您从源码中编译的程序将不能正常运行."
                             "你可以自己申请AppKey(AppSecret)或者下载编译好的程序.",QMessageBox::Ok,QMessageBox::Ok);
        return ;
    }

}

