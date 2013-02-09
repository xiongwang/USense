#ifndef RSSOBJECT_H
#define RSSOBJECT_H

#include <QObject>

class RssObject : public QObject
{
    Q_OBJECT
public:
    explicit RssObject(QObject *parent = 0);

signals:
    void explains();
    void sendHTML(QString str);
    void sendContent(QString str);

public slots:
    void getUrl(QString url);
    void getContent(QString content);

};

#endif // RSSOBJECT_H
