#include "emotionapi.h"

EmotionApi::EmotionApi(QObject *parent) :
    QObject(parent)
{
    QMap<QString,QString> param;
    QString emotions;
    param.clear();

    param.insert("api_key", Config::APIKey);
    param.insert("format", Config::ResultFormat);
    param.insert("method","status.getEmoticons");
    param.insert("type","all");
    param.insert("v", Config::Version);

    QString sig = Utility::getSignature(param);
    param.insert("sig", sig);

    HttpGet http;
    emotions = http.post(QUrl(Config::RESTServerURL), param);
    QDomDocument doc;
    QString errorStr;
    int errorLine;
    int errorCol;
    if(!doc.setContent(emotions,true, &errorStr, &errorLine, &errorCol))
        return;
    QDomElement root = doc.documentElement();
    QDomNodeList nodes= root.elementsByTagName("emoticon");
    param.clear();
    this->mEmotions.clear();
    QString strEmotion;
    QString strIcon;
    for(uint i=0; i<nodes.length();i++)
    {
        QDomElement element = nodes.at(i).toElement();
        strEmotion = QString(QObject::tr(element.elementsByTagName("emotion").at(0).toElement().text().toUtf8().data()));
        strIcon = QString(QObject::tr(element.elementsByTagName("icon").at(0).toElement().text().toUtf8().data()));
        this->mEmotions.insert(strEmotion,
                     "<img src=\""+strIcon+"\"/>");
    }
}

QString EmotionApi::TextHandler(QString strText)
{
    if(!this->mEmotions.isEmpty()){
        QMapIterator<QString, QString> i(mEmotions);
        while (i.hasNext()) {
            i.next();
            strText.replace(i.key(),i.value());
        }
    }
    return strText;
}

QString EmotionApi::RemoveEmotion(QString strText)
{
    if(!this->mEmotions.isEmpty()){
        QMapIterator<QString, QString> i(mEmotions);
        while (i.hasNext()) {
            i.next();
            strText.replace(i.key(),"");
        }
    }
    return strText;
}
