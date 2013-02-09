#ifndef RENRENACCOUNT_H
#define RENRENACCOUNT_H

#include <qstring.h>

class RenRenAccount
{
public:
    RenRenAccount();
    void setAccessToken(QString accessToken);
    void setUsername(QString userName);
    void setPassword(QString password);
    void setSessionKey(QString sessionKey);
    void setAuthorizationCode(QString authorizationCode);
    void setUid(QString uid);
    void setSessionSecret(QString sessionsecret);
    void setExpires(QString expires);
    QString getAccessToken();
    QString getUsername();
    QString getPassword();
    QString getSessionKey();
    QString getAuthorizationCode();
    QString getUid();
    QString getSessionSecret();
    QString getExpires();

private:
    QString Uid;
    QString SessionSecret;
    QString Expires;
    QString AccessToken;
    QString Username;
    QString Password;
    QString SessionKey;
    QString  AuthorizationCode;
};

#endif // RENRENACCOUNT_H
