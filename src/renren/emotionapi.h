#ifndef EMOTIONAPI_H
#define EMOTIONAPI_H

#include <QObject>
#include "utility.h"
#include "method.h"
#include "httpget.h"
#include <vector>
#include <QtXml>
#include "config.h"

class EmotionApi : public QObject
{
    Q_OBJECT
public:
    explicit EmotionApi(QObject *parent = 0);
    QMap<QString,QString> mEmotions;

    QString TextHandler(QString strText);
    QString RemoveEmotion(QString strText);


signals:

public slots:

};

#endif // EMOTIONAPI_H
