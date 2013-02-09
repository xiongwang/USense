#include "rssreader.h"

RssReader::RssReader(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RssReader)
{
    ui->setupUi(this);
    QTreeWidgetItem *root1 = new QTreeWidgetItem(this->ui->treeWidget_Rsslist,QStringList(QString(QObject::tr("科技"))));
    QTreeWidgetItem *root2 = new QTreeWidgetItem(this->ui->treeWidget_Rsslist,QStringList(QString(QObject::tr("社会"))));
    QTreeWidgetItem *root3 = new QTreeWidgetItem(this->ui->treeWidget_Rsslist,QStringList(QString(QObject::tr("生活"))));
    QTreeWidgetItem *leaf1 = new QTreeWidgetItem(root1,QStringList(QString("cnBeta")));
    QTreeWidgetItem *leaf2 = new QTreeWidgetItem(root1,QStringList(QString("新浪焦点新闻")));
    QTreeWidgetItem *leaf3 = new QTreeWidgetItem(root3,QStringList(QString("糗事百科")));
    ui->webView_Rss->setHtml("<body style=\"background-color:rgb(222,222,222);font-family:微软雅黑\"></body>");
    //ui->webView_Passage->setHtml("<body style=\"background-color:rgb(222,222,222);font-family:微软雅黑\"></body>");
    pRssObject = new RssObject(this);
    pGetRss = new QNetworkAccessManager(this);
    pGetPassage = new QNetworkAccessManager(this);
    connect(pGetRss,SIGNAL(finished(QNetworkReply*)),this,SLOT(RssFinished(QNetworkReply*)));
    connect(pGetPassage,SIGNAL(finished(QNetworkReply*)),this,SLOT(PassageFinished(QNetworkReply*)));
    connect(ui->webView_Rss->page()->mainFrame(),SIGNAL(javaScriptWindowObjectCleared()),this,SLOT(addJavaScriptObject()));
    connect(pRssObject,SIGNAL(sendHTML(QString)),this,SLOT(readUrl(QString)));
    connect(ui->treeWidget_Rsslist,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(ChoseRss(QTreeWidgetItem*)));
    ui->webView_Passage->hide();
}

RssReader::~RssReader()
{
    delete ui;
}
void RssReader::RssFinished(QNetworkReply *reply)  //当回复结束后
{
    QString all = QObject::tr(reply->readAll());
    QRegExp rx("<rss version=\"2.0\">");
    QString res="";
    res += "<script type=\"text/javascript\" src=qrc:/other/res/rss/signals.js></script>";
    res +=  "<body style=\"background-color:rgb(222,222,222);font-family:微软雅黑\">";
    int pos = rx.indexIn(all);
    if(pos > -1)
    {
        QDomDocument doc;
        QString errorStr;
        int errorLine;
        int errorCol;
        if(!doc.setContent(all,true, &errorStr, &errorLine, &errorCol))
            return;
        QDomElement root = doc.documentElement();
        QDomNodeList nodes = root.elementsByTagName("item");
        for( uint i=0; i<nodes.length(); i++)
        {
            QDomElement element = nodes.at(i).toElement();
            QString title = QString(QObject::tr(element.elementsByTagName("title").at(0).toElement().text().toUtf8().data()));
            QString description = QString(QObject::tr(element.elementsByTagName("description").at(0).toElement().text().toUtf8().data()));
            QString pubDate = QString(QObject::tr(element.elementsByTagName("pubDate").at(0).toElement().text().toUtf8().data()));
            QString guid= QString(QObject::tr(element.elementsByTagName("guid").at(0).toElement().text().toUtf8().data()));
            guid.replace("http://go.rss.sina.com.cn/redirect.php?url=","");
            res=res+"<div style=\"height:100px;overflow:hidden;border:solid;border-top:0px;border-left:0px;border-right:0px;\" onclick=\"get('"+guid+"')\">"
                    "<div style=\"font-size:16px;\">"+title+"</div>"
                    "<div style=\"font-size:13px;\">"+pubDate+"</div>"
                    "<div style=\"height:80px;font-size:13px;\">"+description+"</div>"
                    "<div id=\"guid\" style=\"font-size:10px;display:none;\">"+guid+"</div>"
                    "</div>";
        }
        res +="</body>";
        ui->webView_Rss->setHtml(res);
    }
    else
    {
        all.replace(QRegExp("href"),"");
        QString title;
        QString pubDate;
        QString description;
        QString guid="http://feed.feedsky.com/qiushi";
        rx.setPattern("<h4 class=\"itemtitle\">().+</h4>");
        pos = rx.indexIn(all);
            if (pos > -1)
            {
                 title = rx.cap(1);
                 qDebug("found");
            }
            if(pos ==-1)
                qDebug("not found!");
        rx.setPattern("<h5 class=\"itemposttime\".+>(.+)</h5>");
        pos = rx.indexIn(all);
            if (pos > -1)
            {
                 pubDate = rx.cap(1);
                 qDebug("found");
            }
            if(pos ==-1)
                qDebug("not found!");
         rx.setPattern("<description>(.+)</description>");
         pos = rx.indexIn(all);
            if (pos > -1)
            {
                description = rx.cap(1);
                qDebug("found");
            }
            if(pos ==-1)
                qDebug("not found!");
         res=res+"<div style=\"bordhideer-bottom:1px;border:solid;border-top:0px;border-left:0px;border-right:0px;\" onclick=\"get('"+guid+"')\">"
                 "<div style=\"font-size:16px;\">"+title+"</div>"
                 "<div style=\"font-size:13px;\">"+pubDate+"</div>"
                 "<div style=\"font-size:13px;\">"+description+"</div>"
                 "<div id=\"guid\" style=\"font-size:10px;display:none;\">"+guid+"</div>"
                 "</div>";
         res +="</body>";
         ui->webView_Rss->setHtml(res);
    }
}
//将内部对象暴露给js
void RssReader::addJavaScriptObject()
{
    ui->webView_Rss->page()->mainFrame()->addToJavaScriptWindowObject("RssObject",this->pRssObject);
}
//访问文章地址
void RssReader::readUrl(QString url)
{
    pGetPassage->get(QNetworkRequest(QUrl(url)));
    ui->webView_Passage->show();
}
//获取文章内容
void RssReader::PassageFinished(QNetworkReply *reply)
{
    QTextCodec *codec = QTextCodec::codecForName("gb2312");
    QTextCodec::setCodecForCStrings(codec);
    QString all = reply->readAll();
    all.replace(QRegExp("href"),"");
    int kind = this->RssJudge(all);
    switch(kind)
    {
        case 0:
            this->ShowcnBetaPassage(&all);break;
        case 1:
            this->ShowSinaPassage(&all);break;
        case 2:
            this->ShowQiushiPassage(&all);break;
    }

}
//根据点击的item访问不同Rss源
void RssReader::ChoseRss(QTreeWidgetItem *item)
{
    QString str = item->text(0);
    if(str == "cnBeta")
    {
        pGetRss->get(QNetworkRequest(QUrl("http://www.cnbeta.com/backend.php")));
    }
    if(str == "新浪焦点新闻")
    {
        pGetRss->get(QNetworkRequest(QUrl("http://rss.sina.com.cn/news/allnews/tech.xml")));
    }
    if(str == "糗事百科")
    {
        pGetRss->get(QNetworkRequest(QUrl("http://feed.feedsky.com/qiushi")));
    }
}
//左侧button
void RssReader::mousePressEvent(QMouseEvent *event)
{
    QPoint point = ui->label->rect().topLeft();
    if(event->x() > point.x()+10 && event->x() < point.x()+70 && event->y() > point.y()+10 && event->y() < point.y()+70)
    {
        qDebug("catch");
    }
}
//判断Rss来自哪个网站
int RssReader::RssJudge(QString str)
{
    QRegExp rx;
    QStringList rssList;
    rssList << "cnbeta.com" << "sina.com" << "qiushibaike.com";
    for(int i=0;i<3;i++)
    {
        rx.setPattern(rssList.at(i));
        int pos=rx.indexIn(str);
        if(pos > -1)
            return i;
    }
    return 100;
}

//各类网站的文章处理
void RssReader::ShowcnBetaPassage(QString *str)
{
    QRegExp rx("<div id=\"news_content\">(.+)<div class=\"digbox\">");
    QString content;
    int pos = rx.indexIn(*str);
    if (pos > -1)
    {
         content = rx.cap(1);
         qDebug("found");
    }
    if(pos == -1)
        qDebug("not found!");
    rx.setPattern("<div class=\"title\"><span class=\"leftBorder\"></span><span class=\"midBlock\">(.+)</span><span class=\"rightBorder\"></span></div>");
    QString title;
    pos = rx.indexIn(*str);
        if (pos > -1)
        {
             title = rx.cap(1);
             qDebug("found");
        }
        if(pos==-1)
            qDebug("not found!");
    ui->webView_Passage->setHtml("<body style=\"background-color:rgb(222,222,222);font-family:微软雅黑;\">"+title+"<p/>"+content+"</body>");
}
void RssReader::ShowSinaPassage(QString *str)
{
    QRegExp rx("<h1 id=\"artibodyTitle\".+>(.+)</h1>");
    QString title;
    QString content;
    int pos = rx.indexIn(*str);
    if(pos > -1)
    {
        title = rx.cap(1);
        qDebug("found");
    }
    if(pos == -1)
        qDebug("not found");
    rx.setPattern("<div class=\"blkContainerSblkCon\" id=\"artibody\">(.+)<!-- publish_helper_end -->");
    pos = rx.indexIn(*str);
    if(pos > -1)
    {
        content = rx.cap(1);
        qDebug("found");
    }
    if(pos == -1)
        qDebug("not found");
    ui->webView_Passage->setHtml("<body style=\"background-color:rgb(222,222,222);font-family:微软雅黑;\">"+title+"<p/>"+content+"</body>");
}
void RssReader::ShowQiushiPassage(QString *str)
{

}
