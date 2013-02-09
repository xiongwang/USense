#include "oauth2.h"

OAuth2::OAuth2()
{
}

OAuth2::OAuth2(RenRenAccount account)
{
    this->account = account;
}

void OAuth2::setAccount(RenRenAccount account)
{
    this->account = account;
}

RenRenAccount OAuth2::getAccount()
{
    return this->account;
}

void OAuth2::getAccessToken()
{
    QMap<QString,QString> param;
    HttpGet httpGet;
    QString post_form_id;
    QString m;

    param.clear();
    param.insert("redirect_uri", "http://graph.renren.com/oauth/grant?client_id=" + Config::APIKey + "&scope=" + Config::Scope + "&redirect_uri=http%3A%2F%2Fgraph.renren.com%2Foauth%2Flogin_success.html&response_type=token&display=iframe&secure=true&pp=1");
    param.insert("origin", "80204");
    param.insert("username", this->account.getUsername());
    param.insert("password", this->account.getPassword());

    QString htmlPost = httpGet.post(QUrl("https://graph.renren.com/login"), param);

    //qDebug("auth:"+htmlPost.toLocal8Bit());

    QRegExp rx1("name=\"post_form_id\" value=\"(.+)<input type=\"hidden\" name=\"secure\" value=\"true\"");

    int pos = rx1.indexIn(htmlPost);
    if (pos > -1)
    {
         post_form_id = rx1.cap(1);
         //qDebug("post_form_id found:" + post_form_id.toLocal8Bit());
    }
    if(pos == -1)
        qDebug("post_form_id not found!");
    post_form_id.replace("\" />\n","");

    QRegExp rx2("name=\"m\" value=\"(.+)\" />");

    pos = rx2.indexIn(htmlPost);
    if (pos > -1)
    {
         m = rx2.cap(1);
         //qDebug("m found:" + m.toLocal8Bit());
    }
    if(pos == -1)
        qDebug("m not found!");

    param.clear();
    param.insert("origin", "80104");
    param.insert("redirect_uri", "http://graph.renren.com/oauth/login_success.html");
    param.insert("client_id", Config::APIKey);
    param.insert("response_type", "token");
    param.insert("scope", Config::Scope);
    param.insert("state", "");
    param.insert("display", "iframe");
    param.insert("post_form_id", post_form_id);
    param.insert("secure", "true");
    param.insert("m", m);

    htmlPost = httpGet.post(QUrl("https://graph.renren.com/oauth/grant"), param);
    htmlPost = httpGet.getUrl();

    QRegExp access("http://graph.renren.com/oauth/login_success.html#access_token=(.+)&expires_in=");

    pos = access.indexIn(htmlPost);

    if (pos > -1)
    {
         htmlPost = access.cap(1);
         //qDebug("access_token found:" + htmlPost.toLocal8Bit());
         this->account.setAccessToken(htmlPost);
    }
    if(pos == -1)
    {
        qDebug("access_token not found!");
        return;
    }
}

void OAuth2::getSessionKey()
{
    HttpGet httpGet;
    QString url = "https://graph.renren.com/renren_api/session_key?oauth_token="+this->account.getAccessToken();
    QString sessionkey;
    QString sessionsecret;
    QString expire;
    QString uid;
    QString sjson = httpGet.get(url);

//    qDebug("Get Session Json:"+ sjson.toLocal8Bit());
    QRegExp rx("session_key\":\"(.+)oauth_token");

    int pos = rx.indexIn(sjson);
    if (pos > -1)
    {
         sessionkey = rx.cap(1);
         sessionkey.replace("\"},\"","");
         qDebug("Session found:" + sessionkey.toLocal8Bit());
    }
    if(pos == -1)
        qDebug("Session not found!");

    rx.	setPattern("expires_in\":(.+)session_key");
    pos = rx.indexIn(sjson);
    if (pos > -1)
    {
        expire = rx.cap(1);
        expire.replace(",\"","");
        qDebug("expire found:" + expire.toLocal8Bit());
    }
    if(pos == -1)
        qDebug("expire not found!");

    rx.setPattern("session_secret\":\"(.+)expires_in");
    pos = rx.indexIn(sjson);
    if (pos > -1)
    {
         sessionsecret = rx.cap(1);
         sessionsecret.replace("\",\"","");
         qDebug("sessionsecret found:" + sessionsecret.toLocal8Bit());
    }
    if(pos == -1)
        qDebug("sessionsecret not found!");

    rx.setPattern("id\":(.+)");
    pos = rx.indexIn(sjson);
    if (pos > -1)
    {
         uid = rx.cap(1);
         uid.replace("}}","");
         qDebug("uid found:" + uid.toLocal8Bit());
    }
    if(pos == -1)
        qDebug("uid not found!");
    double expire_unix = QDateTime::currentMSecsSinceEpoch()/1000 + expire.toLong();
    this->account.setSessionKey(sessionkey);
    this->account.setExpires(QString::number(expire_unix));
    this->account.setSessionSecret(sessionsecret);
    this->account.setUid(uid);
    //this->account.setAccessToken(Config::APPID+"|"+sessionkey);
}

void OAuth2::getAuthorizationCode()
{
    QMap<QString,QString> param;
    HttpGet httpGet;
    QString post_form_id;
    QString m;
    QString access_token;

    param.clear();
    param.insert("redirect_uri", "http://graph.renren.com/oauth/grant?client_id=" + Config::APIKey + "&scope=" + Config::Scope + "&redirect_uri=http%3A%2F%2Fgraph.renren.com%2Foauth%2Flogin_success.html&response_type=code&display=iframe&secure=true&pp=1");
    param.insert("origin", "80204");
    param.insert("username", this->account.getUsername());
    param.insert("password", this->account.getPassword());

    QString htmlPost = httpGet.post(QUrl("https://graph.renren.com/login"), param);

    QRegExp rx1("name=\"post_form_id\" value=\"(.+)<input type=\"hidden\" name=\"secure\" value=\"true\"");

    int pos = rx1.indexIn(htmlPost);
    if (pos > -1)
    {
         post_form_id = rx1.cap(1);
         qDebug("post_form_id found:" + post_form_id.toLocal8Bit());
    }
    if(pos == -1)
        qDebug("post_form_id not found!");
    post_form_id.replace("\" />\n","");

    QRegExp rx2("name=\"m\" value=\"(.+)\" />");

    pos = rx2.indexIn(htmlPost);
    if (pos > -1)
    {
         m = rx2.cap(1);
         qDebug("m found:" + m.toLocal8Bit());
    }
    if(pos == -1)
        qDebug("m not found!");

    param.clear();
    param.insert("origin", "80104");
    param.insert("redirect_uri", "http://graph.renren.com/oauth/login_success.html");
    param.insert("client_id", Config::APIKey);
    param.insert("response_type", "code");
    param.insert("scope", Config::Scope);
    param.insert("state", "");
    param.insert("display", "iframe");
    param.insert("post_form_id", post_form_id);
    param.insert("secure", "true");
    param.insert("m", m);

    htmlPost = httpGet.post(QUrl("https://graph.renren.com/oauth/grant"), param);
    qDebug(htmlPost.toLocal8Bit());
    htmlPost = httpGet.getUrl();

    QRegExp access("code=(.+)");

    pos = access.indexIn(htmlPost);

    if (pos > -1)
    {
         htmlPost = access.cap(1);
         qDebug("authorization_code found:" + htmlPost.toLocal8Bit());
         this->account.setAuthorizationCode(htmlPost);
    }
    if(pos == -1)
    {
        qDebug("authorization_code not found!");
        return;
    }

    param.clear();
    param.insert("grant_type", "authorization_code");
    param.insert("code", htmlPost);
    param.insert("client_id", Config::APIKey);
    param.insert("client_secret", Config::Secret);
    param.insert("redirect_uri", "http://graph.renren.com/oauth/login_success.html");

    htmlPost = httpGet.post(QUrl("https://graph.renren.com/oauth/token"), param);
    qDebug("json:" + htmlPost.toLocal8Bit());

    QRegExp rx3("access_token\": \"(.+)\"expires_in");

    pos = rx3.indexIn(htmlPost);
    if (pos > -1)
    {
         access_token = rx3.cap(1);
         qDebug("access_token found:" + access_token.toLocal8Bit());
    }
    if(pos == -1)
        qDebug("access_token not found!");

    access_token.replace("\",\n  ","");
    this->account.setAccessToken(access_token);
    qDebug(access_token.toLocal8Bit());
}
