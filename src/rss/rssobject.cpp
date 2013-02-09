#include "rssobject.h"

RssObject::RssObject(QObject *parent) :
    QObject(parent)
{
}
void RssObject::getUrl(QString url)
{
       emit sendHTML(url);
}

void RssObject::getContent(QString content)
{
       emit sendContent(content);
}
