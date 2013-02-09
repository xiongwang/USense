

#include "sinaparserxml.h"
#include <QtXml>

SinaParserXml::SinaParserXml()
{
//    this->xml = content;
//    this->account = account;
}



bool SinaParserXml::parserSinaAccountInformation(Account *account, QString &xmlContent)
{
    QDomDocument doc;
    QString errorStr;/* Error message */
    int errorLine;
    int errorCol;
    if(!doc.setContent(xmlContent,true,&errorStr,&errorLine,&errorCol))
    {
        //Deal error
        return false;
    }
    QDomElement root = doc.documentElement(); /* root element */
    QDomNodeList nodes = root.childNodes();
    Status latestStatus;
    for(uint i=0; i<nodes.length(); i++)
    {
        QDomNode node = nodes.at(i);
        if(node.isElement())
        {
            QDomElement element = node.toElement();
            if (element.tagName() == "status")
            {
                latestStatus =  parserRetweetedStatus(element.childNodes());
                account->RemoveAll(account->homePageStatus);
                account->homePageStatus.append(latestStatus);
            }
            else
                fillAccount(account,element.tagName(), element.text());
        }
    }
    return true;
}

void SinaParserXml::fillAccount(Account *account, QString tagName, QString text)
{
    if(tagName=="id")
        account->setId(text);
    else if(tagName=="screen_name")
        account->setScreenName(text);
    else if(tagName=="name")
        account->setName(text);
    else if (tagName == "location")
        account->setLocation(text);
    else if (tagName == "description")
        account->setDescription(text);
    else if (tagName == "gender")
    {
        if (text == "m")
            account->setGender("Male");
        else if (text == "f")
            account->setGender("Female");
        else if (text == "n")
            account->setGender("Unknown");
    }
    else if(tagName=="profile_image_url")
        account->setProfileImageUrl(text);
    else if(tagName=="friends_count")
        account->setFriends(text.toInt());
    else if(tagName=="followers_count")
        account->setFollower(text.toInt());
    else if(tagName=="statuses_count")
        account->setWeiboCount(text.toInt());
    else if(tagName == "favourites_count")
        account->setFavourites(text.toInt());
    else if(tagName=="verified")
    {
        if(text=="true")
            account->setVerified(true);
        else
            account->setVerified(false);
    }
}

bool SinaParserXml::parserStatusXml(QString &content, QList<Status> &statusList, 
                                    QMap<QString, Status> &retwStatusMap)
{
    QDomDocument doc;
    QString errorStr;
    int errorLine;
    int errorCol;
    if(!doc.setContent(content,true,&errorStr, &errorLine, &errorCol))
    {
        return false;
    }
    QDomElement root = doc.documentElement();
    QDomNodeList nodes = root.elementsByTagName("status");
    for(uint i=0; i<nodes.length(); i++)
    {
        Status status;
        Status retwStatus;
        QDomNode node = nodes.at(i);
        if(node.isElement())
        {
            QDomElement element = node.toElement();
            parserStatus(element, status, retwStatus);
            status.setRetweetedId(retwStatus.getId());
            statusList.append(status);
            retwStatusMap.insert(retwStatus.getId(), retwStatus);
        }
    }
    return true;
}

void SinaParserXml::parserUnreadXml(QString &xmlContent, int &statusUnread, int &atMeMentionsUnread, 
                                    int &commentUnread, int &directMessageUnread, int &newFollower)
{
//    statusUnread = 0; atMeMentionsUnread = 0;
//    commentUnread = 0; directMessageUnread = 0; newFollower = 0;
    QDomDocument doc; QString errorStr; int errorLine; int errorCol;
    if(!doc.setContent(xmlContent,true,&errorStr, &errorLine, &errorCol))
        return;
    //QDomElement root = doc.documentElement();
    QDomNodeList nodes = doc.documentElement().childNodes();
    for( uint i=0; i<nodes.length(); i++)
    {
        QDomNode node = nodes.at(i);
        QDomElement element = node.toElement();
        if(element.tagName()=="new_status")
            statusUnread += element.text().toInt();
        else if(element.tagName()=="followers")
            newFollower += element.text().toInt();
        else if(element.tagName()=="dm")
            directMessageUnread += element.text().toInt();
        else if(element.tagName()=="mentions")
            atMeMentionsUnread += element.text().toInt();
        else if(element.tagName() == "comments")
            commentUnread += element.text().toInt();
    }
}

bool SinaParserXml::parserCommentTimeline(QString &content, QList<Status> &commentList, 
                                          QMap<QString, Status> &retwCommentMap)
{
    QDomDocument doc;
    QString errorStr;
    int errorLine;
    int errorCol;
    if(!doc.setContent(content,true,&errorStr, &errorLine, &errorCol))
    {
        return false;
    }
    QDomElement root = doc.documentElement();
    QDomNodeList nodes = root.elementsByTagName("comment");
    for(uint i=0; i<nodes.length(); i++)
    {
        Status status;
        Status retwStatus;
        QDomNode node = nodes.at(i);
        if(node.isElement())
        {
            QDomElement element = node.toElement();
            parserCommentStatus(element, status, retwStatus);
            status.setRetweetedId(retwStatus.getId());
            commentList.append(status);
            retwCommentMap.insert(status.getRetweetedId(), retwStatus);
        }
    }
    return true;
}

void SinaParserXml::parserDirectMessage(QString &xmlContent, QList<Status> &directMessageList,DMType dmType)
{

    if (dmType == Receive)
    {
        QList<QString> senderList;
        QDomDocument doc; QString errorStr; int errorLine; int errorCol;
        if(!doc.setContent(xmlContent,true, &errorStr, &errorLine, &errorCol))
            return;
        QDomElement root = doc.documentElement();
        QDomNodeList nodes = root.elementsByTagName("direct_message");
        for( uint i=0; i<nodes.length(); i++)
        {
            Status status;
            QDomNode node = nodes.at(i);
            QDomElement element = node.toElement();
            parserDM(element, status);

            if ( -1 == senderList.indexOf(status.getUserId()))
            {
                directMessageList.append(status);
            }

            senderList.append(status.getUserId());
        }

        for (int i = 0 ; i != directMessageList.size(); ++i)
        {
            Status status = directMessageList.at(i);
            QString newUserName = status.getUserName()
                    .append(" ["
                            + QString::number(senderList.count(status.getUserId())).toLocal8Bit()
                            + "]");

            status.setUserName(newUserName);

            directMessageList.replace(i,status);
        }

    }
    else if (dmType == Send) {

        QDomDocument doc; QString errorStr; int errorLine; int errorCol;
        if(!doc.setContent(xmlContent,true, &errorStr, &errorLine, &errorCol))
            return;
        QDomElement root = doc.documentElement();
        QDomNodeList nodes = root.elementsByTagName("direct_message");
        for( uint i=0; i<nodes.length(); i++)
        {
            Status status;
            QDomNode node = nodes.at(i);
            QDomElement element = node.toElement();
            parserDM(element, status);
            directMessageList.append(status);
        }

    }
    else
        return ;



}

void SinaParserXml::parserDM(QDomElement &messageElement, Status &status)
{
    QDomNodeList messageNodes = messageElement.childNodes();
    for( uint j=0; j<messageNodes.length(); j++)
    {
        QDomNode node = messageNodes.at(j);
        QDomElement element = node.toElement();
        if(element.tagName()=="sender")
        {
            QDomNodeList senderNodes = element.childNodes();
            for( uint i=0; i<senderNodes.length(); i++)
            {
                QDomElement senderElement = senderNodes.at(i).toElement();
                fillSender(status, senderElement.tagName(), senderElement.text());
            }

        }
        else
            fillMessage(status, element.tagName(), element.text());
    }
}


void SinaParserXml::fillSender(Status &status, QString tagName, QString text)
{
    if(tagName=="id")
        status.setUserId(text);
    else if(tagName=="screen_name")
        status.setUserName(text);
    else if(tagName=="profile_image_url")
        status.setUserProfileImageUrl(text);
}

void SinaParserXml::fillMessage(Status &status, QString tagName, QString text)
{
    if(tagName=="id")
        status.setId(text);
    else if(tagName=="created_at")
    {
        QLocale lo(QLocale::C);
        QDateTime dateTime = lo.toDateTime(text, "ddd MMM dd HH:mm:ss +0800 yyyy");
        status.setCreatedAt(dateTime);
    }
    else if(tagName=="text")
        status.setText(text);
    else if (tagName == "sender_id")
        status.setSenderId(text);
    else if (tagName == "recipient_id")
        status.setRecipientId(text);
}

bool SinaParserXml::parserCommentStatus(QDomElement &statusElement, Status &status, Status &retwStatus)
{
    QDomNodeList statusNodes = statusElement.childNodes();
    for(uint j=0; j<statusNodes.length(); j++)
    {
        QDomNode node = statusNodes.at(j);
        if(node.isElement())
        {
            QDomElement element = node.toElement();
            if(element.tagName()=="user")
                parserUserInfo(status, element.childNodes());
            else if(element.tagName()=="reply_comment")
            {
                retwStatus = parserRetweetedStatus(element.childNodes());
            }
            else if (element.tagName() == "status")
            {
                if (retwStatus.getText().length() == 0)
                {
                    retwStatus = parserRetweetedStatus(element.childNodes());
                }
            }
            else
                fillStatus(status, element.tagName(), element.text());
        }
    }
    return true;
}

/* parser a status */
bool SinaParserXml::parserStatus(QDomElement &statusElement,Status &status,Status &retwStatus)
{
    QDomNodeList statusNodes = statusElement.childNodes();
    for(uint j=0; j<statusNodes.length(); j++)
    {
        QDomNode node = statusNodes.at(j);

        if(node.isElement())
        {
            QDomElement element = node.toElement();

            if(element.tagName()=="user")
                parserUserInfo(status, element.childNodes());
            else if(element.tagName()=="retweeted_status")
            {
                retwStatus = parserRetweetedStatus(element.childNodes());
                status.setRetweetedId(retwStatus.getId());
            }
            else
                fillStatus(status,element.tagName(),element.text());
        }
    }

    return true;
}

void SinaParserXml::fillUserInfo(Status &status, QString tagName, QString text)
{
    if(tagName=="screen_name")
        status.setUserName(text);
    else if(tagName=="id")
        status.setUserId(text);
    else if(tagName=="profile_image_url")
        status.setUserProfileImageUrl(text);

}

Status SinaParserXml::parserRetweetedStatus(QDomNodeList nodes)
{
    Status retweetedStatus;
    for( uint k=0; k<nodes.length(); k++)
    {
        QDomElement element = nodes.at(k).toElement();
        if(element.tagName()=="user")
            parserUserInfo(retweetedStatus, element.childNodes());
        else
            fillStatus(retweetedStatus, element.tagName(), element.text());
    }
    return retweetedStatus;
}

void SinaParserXml::fillStatus(Status &status, QString tagName, QString text)
{
    if(tagName=="created_at")
    {
        QLocale lo(QLocale::C);
        QDateTime dateTime = lo.toDateTime(text, "ddd MMM dd HH:mm:ss +0800 yyyy");
        status.setCreatedAt(dateTime);
    }
    else if(tagName=="id")
        status.setId(text);
    else if(tagName=="text")
        status.setText(text);
    else if(tagName=="favorited")
    {
        if(text=="false")
            status.setFavourited(false);
        else
            status.setFavourited(true);
    }
    else if(tagName=="thumbnail_pic")
        status.setThumbnailPic(text);
    else if(tagName=="bmiddle_pic")
        status.setBmiddlePic(text);
    else if(tagName=="original_pic")
        status.setOriginalPic(text);


}

bool SinaParserXml::parserUserInfo(Status &status, QDomNodeList nodes)
{
    for( uint i=0; i<nodes.length(); i++)
    {
        QDomElement element = nodes.at(i).toElement();
        fillUserInfo(status, element.tagName(), element.text());
    }
    return true;
}

void SinaParserXml::parserEmotions(QString &xmlContent, QList<Emotion> &emotionList)
{
    QDomDocument doc;
    QString errorStr;
    int errorLine;
    int errorCol;
    if(!doc.setContent(xmlContent,true,&errorStr, &errorLine, &errorCol))
    {
        return;
    }
    QDomElement root = doc.documentElement();
    QDomNodeList nodes = root.elementsByTagName("emotion");
    for(uint i=0; i<nodes.length(); i++)
    {
        Emotion emotion;
        QDomNodeList nodesList = nodes.at(i).toElement().childNodes();
        for(uint j=0; j<nodesList.length(); j++)
            fillEmotion(emotion, nodesList.at(j).toElement().tagName(), nodesList.at(j).toElement().text());
        emotionList.append(emotion);
    }
}

void SinaParserXml::fillEmotion(Emotion &emotion, QString tagName, QString text)
{
    if(tagName=="phrase")
        emotion.setPhrase(text);
    else if(tagName=="url")
        emotion.setUrl(text);
    else if(tagName=="category")
        emotion.setCategory(text);
    else if(tagName=="is_hot")
    {
        if(text=="true")
            emotion.setHot(true);
        else
            emotion.setHot(false);
    }
}

bool SinaParserXml::parserFriendsAndFollowersXml(QList<Account> &accountList, QString &xmlContent)
{
    /*
    QFile file(QCoreApplication::applicationDirPath() + "/test.html");
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        qDebug(file.errorString().toLocal8Bit());
    }

    file.write(xmlContent.toLocal8Bit());
    file.close();
    */
    accountList.clear();
    QDomDocument doc;
    QString errorStr;
    int errorLine;
    int errorCol;
    if(!doc.setContent(xmlContent,true,&errorStr, &errorLine, &errorCol))
    {
        return false;
    }



    QDomElement root = doc.documentElement();


    QDomNodeList nodeList = root.childNodes();

    for(int i = 0; i != nodeList.length(); ++i)
    {
        QDomNode node = nodeList.at(i);
        if (node.isElement())
        {
            QDomElement element = node.toElement();
            QDomNodeList nodes = element.childNodes();
            Account account;
            for (int j = 0; j != nodes.length(); ++j)
            {
                Status status;
                Status retweetStatus;
                QDomNode tmpNode = nodes.at(j);
                QDomElement tmpElement;
                if (tmpNode.isElement())
                {
                     tmpElement = tmpNode.toElement();
                    if (tmpElement.tagName() != "status")
                    {
                        fillAccount(&account,tmpElement.tagName(),tmpElement.text());
                    }
                    else
                    {
                            parserStatus(tmpElement,status,retweetStatus);
                            account.homePageStatus.append(status);
                            account.homePageRetweeted.insert(retweetStatus.getId(),retweetStatus);
                    }
                }
            }
            if (account.getWeiboCount() != 0)
                accountList.append(account);
        }
    }

    return true;
}

void SinaParserXml::parserOthersComments(QString &xmlContent,QList<Status>& commentList)
{
    QDomDocument doc;
    QString errorStr;
    int errorLine;
    int errorCol;
    if(!doc.setContent(xmlContent,true,&errorStr, &errorLine, &errorCol))
    {
        return;
    }
    QDomElement root = doc.documentElement();
    QDomNodeList nodes = root.elementsByTagName("comment");
    for(uint i=0; i<nodes.length(); i++)
    {
        Status status;
        Status retwStatus;
        QDomNode node = nodes.at(i);
        if(node.isElement())
        {
            QDomElement element = node.toElement();
            parserCommentStatus(element, status, retwStatus);
            commentList.append(status);
        }
    }
}

void SinaParserXml::parserTrends(QString xmlContent,QStringList& nameList,QStringList& queryList)
{
    QDomDocument doc;
    QString errorStr;
    int errorLine;
    int errorCol;
    if(!doc.setContent(xmlContent,true,&errorStr, &errorLine, &errorCol))
    {
        return;
    }
    QDomElement root = doc.documentElement();
    QDomNodeList nameNodeList = root.elementsByTagName("name");
    QDomNodeList queryNodeList = root.elementsByTagName("query");

    Q_ASSERT(nameNodeList.size() == queryNodeList.size());

    for(uint i = 0 ; i != nameNodeList.size(); ++i)
    {
        QDomNode nameNode = nameNodeList.at(i);
        QDomNode queryNode = queryNodeList.at(i);

        nameList.append(nameNode.toElement().text());
        queryList.append(queryNode.toElement().text());
    }
}

void SinaParserXml::fillUrlMap(QString xmlContent, QMap<QString, QString> &urlMap)
{
    QDomDocument doc;
    QString errorStr;
    int errorLine;
    int errorCol;
    if(!doc.setContent(xmlContent,true,&errorStr, &errorLine, &errorCol))
    {
        return;
    }
    QDomElement root = doc.documentElement();
    QDomNodeList shortUrlNodeList = root.elementsByTagName("url_short");
    QDomNodeList longUrlNodeList = root.elementsByTagName("url_long");

    Q_ASSERT(shortUrlNodeList.size() == longUrlNodeList.size());

    for (uint i = 0; i != shortUrlNodeList.size(); ++i)
    {
        QDomNode shortUrlNode = shortUrlNodeList.at(i);
        QDomNode longUrlNode  = longUrlNodeList.at(i);

        urlMap.insert(shortUrlNode.toElement().text(),longUrlNode.toElement().text());
    }
}
