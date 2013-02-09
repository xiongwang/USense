
#ifndef SINASERVICES_H
#define SINASERVICES_H

#include <QObject>
#include <QMap>
#include "emotion.h"

/*! \class SinaServices
 * \brief 新浪API相关信息
 *
 *
 */
class SinaServices : public QObject
{
    Q_OBJECT
public:
    explicit SinaServices(QObject *parent = 0);
    static SinaServices* getInstance();
    QString getServices(QString key);
    void setServices(const QString key, const QString value);
    
signals:

public slots:
    
private:
    static SinaServices* instance;
    QMap<QString, QString> services;
    int lastCategoryAt;
    bool isExistInEmotionCategoryList(QString categoryName);
    
public:
    QMap<QString, QString> emoticonMap;
    QList<Emotion> emotionList;
    QStringList emotionCategoryList;
    void addToEmotionCategoryList(QString categoryName);
    
    static const QString MyAccessToken;
    static const QString MyAccessTokenSecret;
};

#endif // SINASERVICES_H
