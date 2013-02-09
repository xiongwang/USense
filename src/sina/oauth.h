

#ifndef OAUTH_H
#define OAUTH_H

#include <QByteArray>
#include <QMap>
#include <QNetworkRequest>

/*! \class OAuth
  * \brief OAuth认证相关
  */
class OAuth
{
    
public:
    enum HttpMethod {
        GET,
        POST
    };
    
public:
    OAuth();
    QByteArray getHmacSha1Result(QString data, QString key);
    int getRequestToken(QString &oauthToken, QString &oauthTokenSecret);
    QByteArray createSignature(HttpMethod httpMethod, const QString &requestUrl, 
                                                        const QString key, QMap<QString, QString> &params);
    QString authorize(const QString &token, const QString &username, const QString &password);
    int getAccessToken(const QString &oauthToken, const QString &oauthTokenSecret, 
                                                        const QString &verifier,
                                                        QString &accessToken, QString &accessTokenSecret);
    bool startOAuthAuthorzationAuto(const QString &username, const QString &password, 
                                                            QString &accessToken, QString &accessTokenSecret);
    QString createTimeStamp();
    QString createNonce();
    void setOAuthInfo(QNetworkRequest &request, QMap<QString, QString> &params);

    
public:
    static const QString consumerSecretString;//app secret
    static const QString oauthCallbackString;//oob
    static const QString oauthConsumerKeyString;//app key
    static const QString oauthNonceString;
    static const QString oauthSignatureString;
    static const QString oauthSignatureMethodString;//hmac-sha1
    static const QString oauthTimestampString;
    static const QString oauthVersionString;//1.0
    static const QString oauthTokenString;
    static const QString oauthVerifierString;
};

#endif // OAUTH_H
