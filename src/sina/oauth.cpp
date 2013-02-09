

#include "oauth.h"
#include "sinaservices.h"
#include "http.h"
#include <QtCore>

/*! \brief 构造函数
  *  初始化oauthVersion, oauthCallback, key, secret, SignatureMethod.
  */
OAuth::OAuth()
{
    
}

const QString OAuth::consumerSecretString = "consumer_secret";//app secret
const QString OAuth::oauthConsumerKeyString = "oauth_consumer_key";//app key
const QString OAuth::oauthSignatureMethodString = "oauth_signature_method";//HMAC-SHA1
const QString OAuth::oauthTimestampString = "oauth_timestamp";
const QString OAuth::oauthNonceString = "oauth_nonce";
const QString OAuth::oauthVersionString = "oauth_version";//1.0
const QString OAuth::oauthCallbackString = "oauth_callback";//oob
const QString OAuth::oauthSignatureString = "oauth_signature";
const QString OAuth::oauthTokenString = "oauth_token";
const QString OAuth::oauthVerifierString = "oauth_verifier";

/*! \brief 获取access token
 *
 */
int OAuth::getAccessToken(const QString &oauthToken, const QString &oauthTokenSecret, 
                          const QString &verifier, QString &accessToken, QString &accessTokenSecret)
{
    QMap<QString, QString> params;
    SinaServices *services = SinaServices::getInstance();
    params.insert(OAuth::oauthConsumerKeyString, services->getServices("Source"));
    params.insert(OAuth::oauthNonceString, this->createNonce());
    params.insert(OAuth::oauthSignatureMethodString, "HMAC-SHA1");
    params.insert(OAuth::oauthTimestampString, this->createTimeStamp());
    params.insert(OAuth::oauthVersionString, "1.0");
    params.insert(OAuth::oauthTokenString, oauthToken);
    params.insert(OAuth::oauthVerifierString, verifier);
    //create signature
    QByteArray tmp = this->createSignature( POST, services->getServices("AccessToken"), 
                                            services->getServices("Secret")+"&"+oauthTokenSecret, params);
    
    params.insert(OAuth::oauthSignatureString, tmp.toPercentEncoding());
    QNetworkRequest request(QUrl(services->getServices("AccessToken")));
    this->setOAuthInfo( request, params);
    Http *http = new Http();
    QString httpResult = http->post(request, NULL);
    int errorCode = http->getNetworkErrorCode();
    if(errorCode == QNetworkReply::NoError)
    {
        qDebug(httpResult.toAscii());
        QStringList resultList = httpResult.split("&");
        for( int i=0; i<resultList.length(); i++)
        {
            QString tmp = resultList.at(i);
            if(tmp.startsWith("oauth_token="))
                accessToken = tmp.right(tmp.size() - 12);
            else if(tmp.startsWith("oauth_token_secret="))
                accessTokenSecret = tmp.right(tmp.size() -  19);
        }
    }
    else
    {
        qDebug("error code is %d", errorCode);
    }
    delete http;
    return errorCode;
}

/*! \brief 获取Request Token
 *  \param [out] oauthToken 返回OAuthToken
 *  \param [out] oauthTokenSecret 返回OAuthTokenSecret
 *  \param [out] Http状态码
 */
int OAuth::getRequestToken(QString &oauthToken, QString &oauthTokenSecret)
{
    QMap<QString, QString> params;
    SinaServices *services = SinaServices::getInstance();
    params.insert(OAuth::oauthCallbackString, "oob");
    params.insert(OAuth::oauthConsumerKeyString, services->getServices("Source"));
    params.insert(OAuth::oauthNonceString, this->createNonce());
    params.insert(OAuth::oauthSignatureMethodString, "HMAC-SHA1");
    params.insert(OAuth::oauthTimestampString, this->createTimeStamp());
    params.insert(OAuth::oauthVersionString, "1.0");
    //create signature
    QByteArray tmp = this->createSignature( GET, services->getServices("RequestToken"), 
                                            services->getServices("Secret")+"&", params);
    
    params.insert(OAuth::oauthSignatureString, tmp.toPercentEncoding());
    
    //start network
    QNetworkRequest request(QUrl(services->getServices("RequestToken")));
    this->setOAuthInfo( request, params);
    Http *http = new Http();
    QString httpResult = http->get(request);
    int httpErrorCode = http->getNetworkErrorCode();
    if(httpErrorCode==QNetworkReply::NoError)
    {
        //do something
        qDebug("Get Request Token Success!");
        QStringList resultList = httpResult.split("&");
        for( int i=0; i<resultList.length(); i++)
        {
            QString tmp = resultList.at(i);
            if(tmp.startsWith("oauth_token="))
                oauthToken = tmp.right(tmp.size() - 12);
            else if(tmp.startsWith("oauth_token_secret="))
                oauthTokenSecret = tmp.right(tmp.size() -  19);
        }
    }
    else
    {
        qDebug("Get Request Token Error: %d", httpErrorCode);
    }
//    qDebug("Request Token: "+httpResult.toAscii());
    delete http;
    return httpErrorCode;
}

/*! \brief 设置OAuth HTTP Authorization
 *
 */
void OAuth::setOAuthInfo(QNetworkRequest &request, QMap<QString, QString> &params)
{
    QStringList paramsList;
    QString tmp = "%1=\"%2\"";
    QMapIterator<QString, QString> it(params);
    while(it.hasNext())
    {
        it.next();
        paramsList.append(tmp.arg(it.key()).arg(it.value()));
    }
    tmp = "OAuth " + paramsList.join(", ");
    /*
    qDebug("header:");
    qDebug(tmp.toAscii());
    */
    request.setRawHeader("Authorization", tmp.toAscii());
}

/*! \brief 生成签名
 * 
 *  \warning 有中文时要进行两次url编码，在该函数中会进行一次url编码，所以params参数中的中文也要进行
 * 一次url编码
 */
QByteArray OAuth::createSignature(HttpMethod httpMethod, const QString &requestUrl, const QString key, 
                                QMap<QString, QString> &params)
{
    //create baseString
    QString baseString;
    switch(httpMethod)
    {
    case GET:
        baseString.append("GET");
        break;
    case POST:
        baseString.append("POST");
        break;
    default:
        break;
    }
    baseString.append("&");
    baseString.append(requestUrl.toAscii().toPercentEncoding());
    baseString.append("&");
    QString tmp;
    QMapIterator< QString, QString> it(params);
    while(it.hasNext())
    {
        it.next();
        tmp.append(it.key()+"="+it.value()+"&");
    }
    tmp.remove(tmp.length()-1, 1);//remove last &
    baseString.append(tmp.toAscii().toPercentEncoding());
    return getHmacSha1Result(baseString, key);
}

/*! \brief 生成时间戳
 *
 */
QString OAuth::createTimeStamp()
{
    return QString::number(QDateTime::currentDateTime().toTime_t());
}

/*! \brief 生成Nonce
 *
 */
QString OAuth::createNonce()
{
    //create nonce
    QString nonce;
    qsrand(QDateTime::currentDateTime().toTime_t());
    const char chars[63] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    for( int i=0; i<16; i++)
    {
        nonce.append( chars[ qrand()%62]);
    }
    //qDebug("nonce is "+nonce.toAscii());
    return nonce;
}

/*! \brief hmac-sha1算法实现
 *  \warning 返回值未进行url编码
 */
QByteArray OAuth::getHmacSha1Result(QString data, QString key)
{
    QByteArray ipadArray( 64, char(0x36));
    QByteArray opadArray( 64, char(0x5C));
    QByteArray temp;
    if( key.length() > 64 )
    {
        temp = QCryptographicHash::hash(key.toAscii(), 
                                        QCryptographicHash::Sha1);
    }
    else
    {
        temp = key.toAscii();
    }
    temp.leftJustified( 64, (char) 0);
    for( int i=0; i<temp.length(); i++)
    {
        ipadArray[i] = ipadArray[i] ^ temp.at(i);
        opadArray[i] = opadArray[i] ^ temp.at(i);
    }
    temp = QCryptographicHash::hash( ipadArray.append( data.toAscii() ), 
                                     QCryptographicHash::Sha1);
    temp = QCryptographicHash::hash( opadArray.append( temp ), 
                                     QCryptographicHash::Sha1);
    return temp.toBase64();
}

/*! \brief 用户授权
 * \param [in] token request_token
 * \param [in] username username
 * \param [in] password password
 * \return verifier number
 */
QString OAuth::authorize(const QString &token, const QString &username, const QString &password)
{
    SinaServices *services = SinaServices::getInstance();
    QString urlString = services->getServices("Authorize")+
                        "?oauth%5Ftoken="+token.toAscii().toPercentEncoding("&=",".-_~")+
                        "&oauth%5Fcallback=xml&userId="+username.toAscii().toPercentEncoding("&=",".-_~")+
                        "&passwd="+password.toAscii().toPercentEncoding("&=",".-_~");
    QUrl url=QUrl::fromEncoded(urlString.toAscii(), QUrl::StrictMode);
    if(url.isValid())
    {
        QNetworkRequest request(url);
        Http *http = new Http();
        QString content = http->get(request);
        int errorCode = http->getNetworkErrorCode();
        if(errorCode == QNetworkReply::NoError)
        {
            //qDebug(content.toAscii());
            QRegExp regExp("<oauth_verifier>(.+)</oauth_verifier>");
            if(regExp.indexIn(content)!=-1)
            {
                return regExp.cap(1);
            }
            else
            {
                qDebug("not found!");
                return QString::null;
            }
        }
        else
        {
            qDebug("error code is %d", errorCode);
            return QString::null;
        }
    }
    else
    {
        return QString::null;
    }
}

/*! \brief 根据用户名和密码获取AccessToken and AccessTokenSecret
 * \return 是否成功
 */
bool OAuth::startOAuthAuthorzationAuto(const QString &username, const QString &password, 
                                       QString &accessToken, QString &accessTokenSecret)
{
    QString requestToken, requestTokenSecret;
    bool result;// function result
    if(QNetworkReply::NoError == getRequestToken( requestToken, requestTokenSecret))
    {
        QString verifierNumber = authorize( requestToken, username, password );
        if(!verifierNumber.isEmpty())
        {
            int err = getAccessToken( requestToken, requestTokenSecret, verifierNumber, accessToken, accessTokenSecret );
            if(err == QNetworkReply::NoError)
            {
                result = true;
            }
            else
            {
                result = false;
            }
        }
        else
        {
            result = false;
        }
    }
    else
    {
        result = false;
    }
    return result;
}
