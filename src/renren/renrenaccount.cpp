#include "renrenaccount.h"

RenRenAccount::RenRenAccount()
{
}

QString RenRenAccount::getAccessToken()
{
    return this->AccessToken;
}

QString RenRenAccount::getPassword()
{
    return this->Password;
}

QString RenRenAccount::getUsername()
{
    return this->Username;
}

QString RenRenAccount::getSessionKey()
{
    return this->SessionKey;
}

QString RenRenAccount::getAuthorizationCode()
{
    return this->AuthorizationCode;
}

void RenRenAccount::setAccessToken(QString accessToken)
{
    this->AccessToken = accessToken;
}

void RenRenAccount::setPassword(QString password)
{
    this->Password = password;
}

void RenRenAccount::setUsername(QString username)
{
    this->Username = username;
}

void RenRenAccount::setSessionKey(QString sessionKey)
{
    this->SessionKey = sessionKey;
}

void RenRenAccount::setAuthorizationCode(QString authorizationCode)
{
    this->AuthorizationCode = authorizationCode;
}

void RenRenAccount::setSessionSecret(QString sessionsecret)
{
    this->SessionSecret = sessionsecret;
}

QString RenRenAccount::getSessionSecret()
{
    return this->SessionSecret;
}
void RenRenAccount::setExpires(QString expires)
{
    this->Expires = expires;
}

QString RenRenAccount::getExpires()
{
    return this->Expires;
}

void RenRenAccount::setUid(QString uid)
{
    this->Uid = uid;
}

QString RenRenAccount::getUid()
{
    return this->Uid;
}
