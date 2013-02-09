

#ifndef SINAPARSERXML_H
#define SINAPARSERXML_H

#include <QObject>
#include <QtXml>
#include "account.h"
#include "status.h"
#include "emotion.h"

/*! \class SinaParserXml
 * \brief xml解析
 *
 *
 */
class SinaParserXml : public QObject
{
    Q_OBJECT
public:
    enum DMType{
        Send  = 1,
        Receive = 2
    };

    SinaParserXml();
    bool parserSinaAccountInformation(Account *account, QString &xmlContent);
    bool parserStatusXml(QString &content,QList<Status> &statusList, QMap<QString,Status> &retwStatusMap);
    bool parserCommentTimeline(QString &content, QList<Status> &commentList,
                               QMap<QString, Status> &retwCommentMap);
    void parserDirectMessage(QString &xmlContent, QList<Status> &directMessageList,DMType dmType);
    void parserDM(QDomElement &messageElement, Status &status);
    bool parserStatus( QDomElement &statusElement , Status &status,Status &retwStatus);
    bool parserCommentStatus( QDomElement &statusElement , Status &status, Status &retwStatus);
    bool parserUserInfo(Status &status, QDomNodeList nodes);
    void parserUnreadXml(QString &xmlContent, int &statusUnread, int &atMeMentionsUnread, 
                         int &commentUnread, int &directMessageUnread, int &newFollower);
    void parserEmotions(QString &xmlContent, QList<Emotion> &emotionList);
    Status parserRetweetedStatus( QDomNodeList nodes);
    void parserOthersComments(QString &xmlContent,QList<Status>& commentList);

    bool parserFriendsAndFollowersXml(QList<Account> &accountList,QString &xmlContent);
    void parserTrends(QString xmlContent,QStringList& nameList,QStringList& queryList);
    void fillUrlMap(QString xmlContent,QMap<QString,QString>& urlMap);


signals:

public slots:

private:
    void fillAccount(Account *account,QString tagName, QString text);
    void fillStatus(Status &status, QString tagName, QString text);
    void fillUserInfo(Status &status, QString tagName, QString text);
    void fillEmotion(Emotion &emotion, QString tagName, QString text);
    void fillMessage(Status &status, QString tagName, QString text);
    void fillSender(Status &status, QString tagName, QString text);
};

#endif // SINAPARSERXML_H
