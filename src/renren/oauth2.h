#ifndef OAUTH2_H
#define OAUTH2_H

#include <qmap.h>
#include <QRegExp>
#include <QDateTime>
#include "httpget.h"
#include "renrenaccount.h"
#include "config.h"

class OAuth2
{
public:
    OAuth2();
    OAuth2(RenRenAccount account);

    void setAccount(RenRenAccount account);
    RenRenAccount getAccount();
    void getAccessToken();
    void getSessionKey();
    void getAuthorizationCode();

private:
    RenRenAccount account;
};

#endif // OAUTH2_H
