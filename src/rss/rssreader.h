#ifndef RSSREADER_H
#define RSSREADER_H

#include <QWidget>
#include <QtNetwork>
#include "rssobject.h"
#include <QtXml>
#include <QtWebKit>
#include "ui_rssreader.h"
#include <QMouseEvent>

namespace Ui {
    class RssReader;
}

class RssReader : public QWidget
{
    Q_OBJECT

public:
    explicit RssReader(QWidget *parent = 0);
    ~RssReader();

protected:
    void mousePressEvent ( QMouseEvent * event );

private:
    Ui::RssReader *ui;
    QNetworkAccessManager *pGetRss;
    QNetworkAccessManager *pGetPassage;
    RssObject *pRssObject;
    int RssJudge(QString str);
    void ShowcnBetaPassage(QString *str);
    void ShowSinaPassage(QString *str);
    void ShowQiushiPassage(QString *str);

private slots:
    void RssFinished(QNetworkReply*);
    void PassageFinished(QNetworkReply*);
    void addJavaScriptObject();
    void readUrl(QString);
    void ChoseRss(QTreeWidgetItem *item);
};

#endif // RSSREADER_H
