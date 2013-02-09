#include "sinawidget.h"
#include "ui_sinawidget.h"
#include "account.h"
#include "sinaweibo.h"
#include "statuswidget.h"
#include "sinaparserxml.h"
#include "showpicture.h"
#include "SinaGlobalNamespace.h"

#include <qimagereader.h>
#include <QDir>
#include <QMessageBox>
#include <QtWebKit>
#include <QtGui>
SinaWidget::SinaWidget(Account* pAccount,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SinaWidget)
{
    ui->setupUi(this);
    this->account = pAccount;
    leftWebView = new MyWebView(this);
    rightWebView = new MyWebView(this);
    rightWebView->settings()->setAttribute(QWebSettings::PluginsEnabled, true);
    leftWebView->setGeometry(0,0,480,490);
    rightWebView->setGeometry(0,0,480,490);
    conf = Configuration::getInstance();
    this->leftWebView->setPage(new QWebPage(this));
    this->showPicture = NULL;
    this->dealWebkitEvent = new DealWebkitEvent(account,this);
    this->timer = new QTimer(this);// timer
    timer->setSingleShot(true);
    QObject::connect(timer,SIGNAL(timeout()), this, SLOT(homePageButtonClicked()));
    timer->start(500);
    this->pStatusWidget = NULL;
    this->currentSelected = 0;

    ui->leftLayout->addWidget(leftWebView);
    ui->rightLayout->addWidget(rightWebView);

    QString originalHtml = "<body style=\"background-color:rgb(59,59,59);font-family:微软雅黑\"></body>";

    this->setWebviewHtml(originalHtml,Left);
    this->setWebviewHtml(originalHtml,Right);

    QFile basicHtmlFile(":/other/res/sina/SinaHomePage.html");
    if(!basicHtmlFile.open(QFile::ReadOnly))
    {
        qDebug("open Error!!!");
        qDebug(basicHtmlFile.errorString().toAscii());
    }
    QTextStream stream(&basicHtmlFile);
    basicHtml = stream.readAll();
    basicHtmlFile.close();

    myFriendsPage.load(":/images/sina/");
    myFriendsPageLighted.load(":/images/sina/");

    atMePage.load(":/images/sina/");
    atMePageLighted.load(":/images/sina/");

    newStatus.load(":/images/sina/");
    newStatusLighted.load(":/images/sina/");

    myWeiboPage.load(":/images/sina/");
    myWeiboPageLighted.load(":/images/sina/");

    directMessage.load(":/images/sina/");
    directMessageLighted.load(":/images/sina/");

    publicPage.load(":/images/sina/");
    publicPageLighted.load(":/images/sina/");

    commentsPage.load(":/images/sina/");
    commentsPageLighted.load(":/images/sina/");

    myFavouritePage.load(":/images/sina/");
    myFavouritePageLighted.load(":/images/sina/");

    changeSelectedPixmap(1,1);
    changeSelectedPixmap(2,0);
    changeSelectedPixmap(3,0);
    changeSelectedPixmap(4,0);
    changeSelectedPixmap(5,0);
    changeSelectedPixmap(6,0);
    changeSelectedPixmap(7,0);
    changeSelectedPixmap(8,0);


    this->homePageHtmlReady = false;
    this->homePageStatusReady = true;
    this->atMePageHtmlReady = false;
    this->atMePageStatusReady = false;
    this->commentPageHtmlReady = false;
    this->commentPageStatusReady = false;
    this->myWeiboPageHtmlReady = false;
    this->myWeiboPageStatusReady = false;
    this->favoritePageHtmlReady = false;
    this->favoritePageStatusReady = false;
    this->directMessagePageHtmlReady = false;
    this->directMessagePageStatusReady = false;

    this->publicStatusesHtmlReady = false;
    this->publicStatusesReady = false;

    this->squareHtmlReady = false;
    this->dailyTrendsHtmlReady = false;

    this->statusUnread=0;
    this->commentsUnread=0;
    this->atMeMentionsUnread=0;
    this->directMessageUnread=0;
    this->newFollower=0;

    this->otherAccount = NULL;

    this->currentSelected = 1;
    changeSelectedPixmap(1,1);


    QObject::connect(leftWebView->page()->mainFrame(),
                      SIGNAL(javaScriptWindowObjectCleared()),
                      this,
                      SLOT(addJavaScriptObject())
                      );

    QObject::connect(rightWebView->page()->mainFrame(),
                     SIGNAL(javaScriptWindowObjectCleared()),
                     this,
                     SLOT(addJavaScriptObject()));

    rightWebView->hide();

    this->dealWebkitEventDisconnectThenConnect();
/*
    timer->setSingleShot(true);
    QObject::connect(timer,SIGNAL(timeout()), this, SLOT(homePageButtonClicked()));
    timer->start(500);
*/

}

SinaWidget::~SinaWidget()
{
    delete ui;
}

void SinaWidget::receiveSignalFromMainWidget(int index){

//    qDebug("currentIndex:"+QString::number(index).toLocal8Bit());
//    this->currentSelected = index;
    if (index == this->currentSelected)
        return ;

    if (index != 4)
    {
        changeSelectedPixmap(this->currentSelected,0);
        this->currentSelected = index;
        changeSelectedPixmap(index,1);
    }

    switch (index){
    case 1:
        homePageButtonClicked();
        break;
    case 2:
        atMePageButtonClicked();
        break;
    case 3:
        myWeiboPageButtonClicked();
        break;
    case 4:
        newWeiboButtonClicked();
        break;
    case 5:
        commentPageButtonClicked();
        break;
    case 6:
        favouritePageButtonClicked();
        break;
    case 7:
        messagePageButtonClicked();
        break;
    case 8:
        //publicStatusesButtonClicked();
        squareButtonClicked();
        break;
    default:
        qDebug("In ReceiveSignalFromMainWidget:index error!!!");
        break;
    }
}

void SinaWidget::getNewPage(QString pageNumStr, int feature)
{

    int page = pageNumStr.toInt();
    this->preHtml();
    if(page<1) page=1;
    SinaWeibo *sina = new SinaWeibo(account);
    SinaParserXml sinaParserXml;
    QList<Status> statusList;
    QMap<QString, Status> retwStatusList;
    QString contentHtml;
    QString xmlContent;

    Q_ASSERT(page >= 1);
    Q_ASSERT(this->currentSelected >= 1);
    Q_ASSERT(this->currentSelected < 9);

    //判断当前所属页面
    switch (this->currentSelected){
    case 1:
    {
        //主页
        xmlContent = sina->getFriendsTimeline(0, 0, conf->getAccountConf(account->getId(), conf->homeCS),
                                                      page, 0, feature);
        sinaParserXml.parserStatusXml(xmlContent, statusList, retwStatusList);

        bool bEmpty = (statusList.size() == 0);

        Account::updateStatus(account->homePageStatus,account->homePageRetweeted,
                                         statusList,retwStatusList);

        contentHtml = sina->homePageStatusToHtml(statusList, retwStatusList);

        //确保刷出新内容
        if (!bEmpty && statusList.size() == 0)
            leftWebView->page()->mainFrame()->evaluateJavaScript("getNewPage()");

        homePageStatusReady = true;

        if (contentHtml == "没有更多微博了!")
        {
            contentHtml = "";
        }
    }
        break;
    case 2:
    {
        //@我的
        xmlContent = sina->getMentions(0, 0, conf->getAccountConf(account->getId(), conf->atMeCS), page);

        sinaParserXml.parserStatusXml(xmlContent, statusList, retwStatusList);

        bool bEmpty = (statusList.size() == 0);

        Account::updateStatus(account->atMePageStatus,account->atMePageRetweeted,
                                         statusList,retwStatusList);



        contentHtml = sina->atMePageStatusToHtml(statusList,retwStatusList);

        //确保刷出新内容
        if (!bEmpty && statusList.size() == 0)
            leftWebView->page()->mainFrame()->evaluateJavaScript("getNewPage()");

        atMePageStatusReady = true;

        if (contentHtml == "没有更多微博了!")
            contentHtml = "";
    }
        break;
    case 3:
    {
        //我的微博
        xmlContent = sina->getUserTimeline("0",conf->getAccountConf(account->getId(),conf->userWeiboCS),page,0,0);

        sinaParserXml.parserStatusXml(xmlContent, statusList, retwStatusList);

        bool bEmpty = (statusList.size() == 0);

        Account::updateStatus(account->userWeiboPageStatus,account->userWeiboPageRetweeted,
                                         statusList,retwStatusList);

        contentHtml = sina->atMePageStatusToHtml(statusList,retwStatusList);

        //确保刷出新内容
        if (!bEmpty && statusList.size() == 0)
            leftWebView->page()->mainFrame()->evaluateJavaScript("getNewPage()");

        myWeiboPageStatusReady = true;
        if (contentHtml == "没有更多微博了!")
            contentHtml = "";
    }
        break;
    case 4:
    {
        return;//发状态
    }
        break;
    case 5:
    {
        //评论
        xmlContent = sina->getCommentTimeline(0, 0, conf->getAccountConf(account->getId(), conf->commentCS), page);

        sinaParserXml.parserCommentTimeline(xmlContent,
                                            statusList, retwStatusList);

        bool bEmpty = (statusList.size() == 0);

        Account::updateStatus(account->commentPageComments,account->commentPageRetweeted,
                                         statusList,retwStatusList);



        contentHtml = sina->commentPageStatusToHtml(statusList,retwStatusList);

        //确保刷出新内容
        if (!bEmpty && statusList.size() == 0)
            leftWebView->page()->mainFrame()->evaluateJavaScript("getNewPage()");

        commentPageStatusReady = true;
        if (contentHtml == "没有更多评论了!")
            contentHtml = "";
    }
        break;
    case 6:
    {
        //收藏
        xmlContent = sina->getFavorites(page);

        sinaParserXml.parserStatusXml(xmlContent,statusList, retwStatusList);

        bool bEmpty = (statusList.size() == 0);

        Account::updateStatus(account->favoritesPageStatus,account->favoritesPageRetweeted,
                                         statusList,retwStatusList);


        contentHtml = sina->favoritePageStatusToHtml(statusList,retwStatusList);

        //确保刷出新内容
        if (!bEmpty && statusList.size() == 0)
            leftWebView->page()->mainFrame()->evaluateJavaScript("getNewPage()");

        favoritePageStatusReady = true;

        if (contentHtml == "没有更多收藏了!")
            contentHtml = "";

    }
        break;
    case 7:
    {
        //私信
        xmlContent = sina->getDirectMessage(0,0,
                                                conf->getAccountConf(account->getId(), conf->directMessageCS), page);

        sinaParserXml.parserDirectMessage(xmlContent, statusList,SinaParserXml::Receive);

        bool bEmpty = (statusList.size() == 0);

        //不存在QMap 故以相同QMap代替
        Account::updateStatus(account->directMessagePageStatus,retwStatusList,
                                         statusList,retwStatusList);

        contentHtml = sina->directMessagePageStatusToHtml(statusList,SinaWeibo::List);

        //确保刷出新内容
        if (!bEmpty && statusList.size() == 0)
            leftWebView->page()->mainFrame()->evaluateJavaScript("getNewPage()");

        directMessagePageStatusReady = true;

        if (contentHtml == "没有私信哦。")
            contentHtml = "";
    }
        break;
    case 8:
    {
        //广场
        return;
    }
        break;
    default:
        break;
    }

    this->leftWebView->page()->mainFrame()->evaluateJavaScript("setNew('"+contentHtml.toAscii().toBase64()+"');");
}

void SinaWidget::prepare()
{
    this->dealWebkitEventDisconnectThenConnect();
    SinaWeibo *sina = new SinaWeibo(this->account);
    homePageButtonClicked();
    delete sina;
    timer->disconnect();
    timer->stop();
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(refreshStatus()));
    timer->setSingleShot(false);
    timer->start(1000*conf->getAccountConf(account->getId(), conf->updateStatusS));//90s
}

void SinaWidget::preHtml()
{
    QObject::disconnect(leftWebView->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()),
                        this, SLOT(addJavaScriptObject()));
    QObject::disconnect(rightWebView->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()),
                        this, SLOT(addJavaScriptObject()));
}

void SinaWidget::showPictueInWidget(QString bigUrl, QString midUrl)
{
/*
    Q_ASSERT(this->parentWidget() != NULL);
    if (showPicture == NULL)
        showPicture = new ShowPicture(bigUrl, midUrl, this);
    else
        showPicture->setUrl(bigUrl,midUrl);
    showPicture->show();

    QTimer *p = new QTimer;
    p->setSingleShot(true);
    connect(p,SIGNAL(timeout()),showPicture,SLOT(downloadFile()));
    p->start(500);
*/

}

void SinaWidget::refreshStatus()
{
    timer->setInterval(1000*conf->getAccountConf(account->getId(), conf->updateStatusS));
    QWebSettings::clearIconDatabase();//clear qwebview memory cache
    QWebSettings::clearMemoryCaches();

    int preStatusUnread = statusUnread; int preAtMeMentionsUnread = atMeMentionsUnread;
    int preCommentsUnread = commentsUnread; int preDirectMessageUnread = directMessageUnread;
    SinaWeibo sinaWeibo(account);
    sinaWeibo.getUnreadInfo(statusUnread, atMeMentionsUnread, commentsUnread, directMessageUnread, newFollower);
    if((statusUnread - preStatusUnread )> 0)
    {
        sinaWeibo.initHomePageStatus();

        this->homePageHtmlReady = false;
    }
    if((commentsUnread - preCommentsUnread )> 0)
    {
        /*
         API: http://open.t.sina.com.cn/wiki/index.php/Statuses/reset_count
         1------comments
         2------atMe
         3-------directMessage
         4-------newFollower
         */
        sinaWeibo.resetCount(1);
        sinaWeibo.initCommentPageStatus();

        this->commentPageHtmlReady = false;
    }
    if((atMeMentionsUnread - preAtMeMentionsUnread)> 0)
    {
        sinaWeibo.resetCount(2);
        sinaWeibo.initAtMePageStatus();

        this->atMePageHtmlReady = false;
    }
    if((directMessageUnread - preDirectMessageUnread)> 0)
    {
        sinaWeibo.resetCount(3);
        sinaWeibo.initDirectMessagePageStatus();

        this->directMessagePageHtmlReady = false;
    }

//    this->refreshSystemTrayIconTooltip();

    QList<WithU::NotifyObject> messageList;
    if((statusUnread - preStatusUnread)>0)
    {
        WithU::NotifyObject obj;
        obj.type = WithU::HomePageNumber;
        for( int i=0; i<(statusUnread-preStatusUnread); i++)
        {
            Status status = account->homePageStatus.at(i);
            obj.status = status;
            if(!status.getRetweetedId().isEmpty())
            {
                obj.retwStatus = account->homePageRetweeted.value(status.getRetweetedId());
            }
            messageList.append(obj);
        }
    }

    if((commentsUnread - preCommentsUnread)>0)
    {
        WithU::NotifyObject obj;
        obj.type = WithU::CommentPageNumber;
        for( int i=0; i<(commentsUnread-preCommentsUnread); i++)
        {
            Status status = account->commentPageComments.at(i);
            obj.status = status;
            if(!status.getRetweetedId().isEmpty())
            {
                obj.retwStatus = account->commentPageRetweeted.value(status.getRetweetedId());
            }
            messageList.append(obj);
        }
    }

    if((atMeMentionsUnread - preAtMeMentionsUnread)>0)
    {
        WithU::NotifyObject obj;
        obj.type = WithU::AtMePageNumber;
        for( int i=0; i<(atMeMentionsUnread-preAtMeMentionsUnread); i++)
        {
            Status status = account->atMePageStatus.at(i);
            obj.status = status;
            if(!status.getRetweetedId().isEmpty())
            {
                obj.retwStatus = account->atMePageRetweeted.value(status.getRetweetedId());
            }
            messageList.append(obj);
        }
    }

    if((directMessageUnread - preDirectMessageUnread)>0)
    {
        WithU::NotifyObject obj;
        obj.type = WithU::DirectMessagePageNumber;
        for( int i=0; i<(directMessageUnread-preDirectMessageUnread); i++)
        {
            Status status = account->directMessagePageStatus.at(i);
            obj.status = status;
            messageList.append(obj);
        }
    }
    /*
     * 消息提醒
     */
    if(!messageList.isEmpty()&&conf->settings->value(account->getId()+"/"+conf->isNotifyCS, true).toBool())
    {
        /*
        Notifer *notifer = new Notifer(this);
        notifer->setAttribute(Qt::WA_DeleteOnClose);
        notifer->setNotiferContentAndShow(messageList);
        */
    }
}


void SinaWidget::getOthersTimelineByScreenName(QString screenName, int page)
{
    this->preHtml();
    if(page<1)
        page = 1;
    SinaWeibo *sina = new SinaWeibo(account);

    QString xmlContent = sina->getUserTimelineByScreenName(screenName,
                                                           conf->getAccountConf(account->getId(),conf->userWeiboCS), page, 0, 0);
    SinaParserXml sinaParserXml;
    QList<Status> statusList;
    QMap<QString, Status> retwStatusList;
    sinaParserXml.parserStatusXml(xmlContent, statusList, retwStatusList);
    QString pageHtml;
    QString contentHtml;
    if(!statusList.isEmpty()&&statusList.length()==(conf->getAccountConf(account->getId(),conf->userWeiboCS)))
    {
        if(page<=1)
        {
            pageHtml="<p><a href=javascript:void(0) onclick=getUserTimelineByScreenName(\"%1\",1)>首页</a>"
                     "<a href=javascript:void(0) onclick=getUserTimelineByScreenName(\"%2\",2)>下一页</a>"
                     "</p>";
            pageHtml = pageHtml.arg(screenName)
                    .arg(screenName);
        }
        else
        {
            pageHtml="<p><a href=javascript:void(0) onclick=getUserTimelineByScreenName(\"%1\",1)>首页</a>"
                     "<a href=javascript:void(0) onclick=getUserTimelineByScreenName(\"%2\",%3)>上一页</a>"
                     "<a href=javascript:void(0) onclick=getUserTimelineByScreenName(\"%4\",%5)>下一页</a>"
                     "</p>";
            pageHtml = pageHtml.arg(screenName)
                    .arg(screenName)
                    .arg(page-1)
                    .arg(screenName)
                    .arg(page+1);
        }
    }
    else
    {
        if(page<=1)
        {
            pageHtml="<p><a href=javascript:void(0) onclick=getUserTimelineByScreenName(\"%1\",1)>首页</a>"
                     "<a href=javascript:void(0) onclick=getUserTimelineByScreenName(\"%2\",2)>下一页</a>"
                     "</p>";
            pageHtml = pageHtml.arg(screenName)
                    .arg(screenName);
        }
        else
        {
            pageHtml="<p><a href=javascript:void(0) onclick=getUserTimelineByScreenName(\"%1\",1)>首页</a>"
                     "<a href=javascript:void(0) onclick=getUserTimelineByScreenName(\"%2\",%3)>上一页</a>"
                     "</p>";
            pageHtml = pageHtml.arg(screenName)
                    .arg(screenName)
                    .arg(page-1);
        }
    }

    contentHtml = sina->userWeiboPageStatusToHtml(statusList, retwStatusList);
    //contentHtml.append(pageHtml);

//    QString htmlStr = basicHtml.arg(contentHtml);

//    qDebug(htmlStr.toUtf8());

    if (screenName != account->getScreenName())
        this->setWebviewHtml(basicHtml.arg(contentHtml),Right);
    else
        this->setWebviewHtml(basicHtml.arg(contentHtml),Left);
//    QString countsInJsonFormat = sina->getStatusCountsByJson(statusList);
    while(!dealWebkitEvent->isDomReady())
    {
        QApplication::processEvents();
    }
//    dealWebkitEvent->sendStatusesCounts(countsInJsonFormat);
    delete sina;
}

void SinaWidget::getMoreHomePageStatus(int page, int feature)
{
    this->preHtml();
    if(page<1) page=1;
    SinaWeibo *sina = new SinaWeibo(account);
    QString xmlContent = sina->getFriendsTimeline(0, 0, conf->getAccountConf(account->getId(), conf->homeCS),
                                                  page, 0, feature);
    SinaParserXml sinaParserXml;
    QList<Status> statusList; QMap<QString, Status> retwStatusList;
    sinaParserXml.parserStatusXml(xmlContent, statusList, retwStatusList);
    QString pageHtml; QString contentHtml;
    if(!statusList.isEmpty()&&statusList.length()>=(conf->getAccountConf(account->getId(),conf->homeCS)))
    {
        if(page<=1)
        {
            pageHtml = "<p><a href=javascript:void(0) onclick=getMoreHomePageStatus(1,0)>首页</a>"
                       "<a href=javascript:void(0) onclick=getMoreHomePageStatus(2,0)>下一页</a>"
                       "</p>";
        }
        else
        {
            pageHtml="<p><a href=javascript:void(0) onclick=getMoreHomePageStatus(1,0)>首页</a>"
                     "<a href=javascript:void(0) onclick=getMoreHomePageStatus(%1,0)>上一页</a>"
                     "<a href=javascript:void(0) onclick=getMoreHomePageStatus(%2,0)>下一页</a>"
                     "</p>";
            pageHtml = pageHtml.arg(page-1).arg(page+1);
        }
    }
    else
    {
        if(page<=1)
        {
            pageHtml="<p><a href=javascript:void(0) onclick=getMoreHomePageStatus(1,0)>首页</a>"
                     "</p>";
        }
        else
        {
            pageHtml="<p><a href=javascript:void(0) onclick=getMoreHomePageStatus(1,0)>首页</a>"
                     "<a href=javascript:void(0) onclick=getMoreHomePageStatus(%1,0)>上一页</a>"
                     "</p>";
            pageHtml = pageHtml.arg(page-1);
        }
    }
    contentHtml = sina->homePageStatusToHtml(statusList, retwStatusList);
    contentHtml.append(pageHtml);
    QString tmp = basicHtml.arg(contentHtml);
    this->setWebviewHtml(tmp,Left);

    //qDebug(tmp.toLocal8Bit());

//    QString countsInJsonFormat = sina->getStatusCountsByJson(statusList);
    while(!dealWebkitEvent->isDomReady())
    {
        QApplication::processEvents();
    }
//    dealWebkitEvent->sendStatusesCounts(countsInJsonFormat);
    delete sina;
}

void SinaWidget::getMoreUserPageStatus(int page, int )
{
    //qDebug(account->getScreenName().toLocal8Bit());
    getOthersTimelineByScreenName(account->getScreenName(),page);
}

void SinaWidget::getMoreAtMePageStatus(int page)
{
    this->preHtml();
    if(page<1) page=1;
    SinaWeibo *sina = new SinaWeibo(account);
    QString xmlContent = sina->getMentions(0, 0, conf->getAccountConf(account->getId(), conf->atMeCS), page);
    SinaParserXml sinaParserXml;
    QList<Status> statusList; QMap<QString, Status> retwStatusMap;
    sinaParserXml.parserStatusXml(xmlContent, statusList, retwStatusMap);
    QString pageHtml; QString contentHtml;
    if(!statusList.isEmpty()&&statusList.length()>=(conf->getAccountConf(account->getId(), conf->atMeCS)))
    {
        if(page<=1)
            pageHtml = "<p><a href=javascript:void(0) onclick=getMoreAtMePageStatus(1)>首页</a>"
                       "<a href=javascript:void(0) onclick=getMoreAtMePageStatus(2)>下一页</a></p>";
         else {
            pageHtml = "<p><a href=javascript:void(0) onclick=getMoreAtMePageStatus(1)>首页</a>"
                       "<a href=javascript:void(0) onclick=getMoreAtMePageStatus(%1)>上一页</a>"
                       "<a href=javascript:void(0) onclick=getMoreAtMePageStatus(%2)>下一页</a></p>";
            pageHtml = pageHtml.arg(page-1).arg(page+1);
        }
     } else {
         if(page<=1) pageHtml = "<p><a href=javascript:void(0) onclick=getMoreAtMePageStatus(1)>首页</a></p>";
         else
         {
             pageHtml = "<p><a href=javascript:void(0) onclick=getMoreAtMePageStatus(1)>首页</a>"
                        "<a href=javascript:void(0) onclick=getMoreAtMePageStatus(%1)>上一页</a></p>";
             pageHtml = pageHtml.arg(page-1);
         }
     }
     contentHtml = sina->atMePageStatusToHtml(statusList, retwStatusMap);
     contentHtml.append(pageHtml);
     this->setWebviewHtml(basicHtml.arg(contentHtml),Left);
//     QString countsInJsonFormat = sina->getStatusCountsByJson(statusList);
     while(!dealWebkitEvent->isDomReady())
     {
         QApplication::processEvents();
     }
 //    dealWebkitEvent->sendStatusesCounts(countsInJsonFormat);
     delete sina;
}

void SinaWidget::getMoreCommentsPageStatus(int page)
{
    this->preHtml();
    if(page<1) page=1;
    SinaWeibo *sina = new SinaWeibo(account);
    QString xmlContent = sina->getCommentTimeline(0, 0,
                                                  conf->getAccountConf(account->getId(), conf->commentCS), page);
    SinaParserXml sinaParserXml;
    QList<Status> statusList; QMap<QString, Status> retwStatusMap;
    sinaParserXml.parserCommentTimeline(xmlContent, statusList, retwStatusMap);
    QString pageHtml; QString contentHtml;
    if(!statusList.isEmpty()&&statusList.length()>=(conf->getAccountConf(account->getId(), conf->commentCS)))
    {
        if(page<=1)
            pageHtml = "<p><a href=javascript:void(0) onclick=getMoreCommentsPageStatus(1)>首页</a>"
                       "<a href=javascript:void(0) onclick=getMoreCommentsPageStatus(2)>下一页</a></p>";
        else {
            pageHtml = "<p><a href=javascript:void(0) onclick=getMoreCommentsPageStatus(1)>首页</a>"
                       "<a href=javascript:void(0) onclick=getMoreCommentsPageStatus(%1)>上一页</a>"
                       "<a href=javascript:void(0) onclick=getMoreCommentsPageStatus(%2)>下一页</a></p>";
            pageHtml = pageHtml.arg(page-1).arg(page+1);
        }
    } else {
        if(page<=1) pageHtml = "<p><a href=javascript:void(0) onclick=getMoreCommentsPageStatus(1)>首页</a></p>";
        else
        {
            pageHtml = "<p><a href=javascript:void(0) onclick=getMoreCommentsPageStatus(1)>首页</a>"
                       "<a href=javascript:void(0) onclick=getMoreCommentsPageStatus(%1)>上一页</a></p>";
            pageHtml = pageHtml.arg(page-1);
        }
    }
    contentHtml = sina->commentPageStatusToHtml(statusList, retwStatusMap);

    this->setWebviewHtml(basicHtml.arg(contentHtml),Left);
    delete sina;
}

void SinaWidget::getMoreFavoritePageStatus(int page)
{
    this->preHtml();
    if(page<1) page=1;
    SinaWeibo *sina = new SinaWeibo(account);
    QString xmlContent = sina->getFavorites(page);
    SinaParserXml sinaParserXml;
    QList<Status> statusList; QMap<QString, Status> retwStatusMap;
    sinaParserXml.parserStatusXml(xmlContent, statusList, retwStatusMap);
    QString pageHtml; QString contentHtml;
    if(!statusList.isEmpty()&&statusList.length()>=(conf->getAccountConf(account->getId(), conf->favoriteCS)))
    {
        if(page<=1)
            pageHtml = "<p><a href=javascript:void(0) onclick=getMoreFavoritePageStatus(1)>首页</a>"
                       "<a href=javascript:void(0) onclick=getMoreFavoritePageStatus(2)>下一页</a></p>";
        else {
            pageHtml = "<p><a href=javascript:void(0) onclick=getMoreFavoritePageStatus(1)>首页</a>"
                       "<a href=javascript:void(0) onclick=getMoreFavoritePageStatus(%1)>上一页</a>"
                       "<a href=javascript:void(0) onclick=getMoreFavoritePageStatus(%2)>下一页</a></p>";
            pageHtml = pageHtml.arg(page-1).arg(page+1);
        }
    } else {
        if(page<=1) pageHtml = "<p><a href=javascript:void(0) onclick=getMoreFavoritePageStatus(1)>首页</a></p>";
        else
        {
            pageHtml = "<p><a href=javascript:void(0) onclick=getMoreFavoritePageStatus(1)>首页</a>"
                       "<a href=javascript:void(0) onclick=getMoreFavoritePageStatus(%1)>上一页</a></p>";
            pageHtml = pageHtml.arg(page-1);
        }
    }
    contentHtml = sina->favoritePageStatusToHtml(statusList, retwStatusMap);
    //contentHtml.append(pageHtml);
    this->setWebviewHtml(basicHtml.arg(contentHtml),Left);
 //   QString countsInJsonFormat = sina->getStatusCountsByJson(statusList);
    while(!dealWebkitEvent->isDomReady())
    {
        QApplication::processEvents();
    }
   // dealWebkitEvent->sendStatusesCounts(countsInJsonFormat);
    delete sina;
}

void SinaWidget::getMoreDirectMessagePageStatus(int page)
{
    this->preHtml();
    if(page<1) page=1;
    SinaWeibo *sina = new SinaWeibo(account);
    QString xmlContent = sina->getDirectMessage( 0, 0,
                                                 conf->getAccountConf(account->getId(), conf->directMessageCS), page);

    SinaParserXml sinaParserXml;
    QList<Status> statusList;
    sinaParserXml.parserDirectMessage(xmlContent, statusList,SinaParserXml::Receive);
    QString pageHtml; QString contentHtml;
    if(!statusList.isEmpty()&&statusList.length()>=(conf->getAccountConf(account->getId(), conf->directMessageCS)))
    {
        if(page<=1)
            pageHtml = "<p><a href=javascript:void(0) onclick=getMoreDirectMessagePageStatus(1)>首页</a>"
                       "<a href=javascript:void(0) onclick=getMoreDirectMessagePageStatus(2)>下一页</a></p>";
        else {
            pageHtml = "<p><a href=javascript:void(0) onclick=getMoreDirectMessagePageStatus(1)>首页</a>"
                       "<a href=javascript:void(0) onclick=getMoreDirectMessagePageStatus(%1)>上一页</a>"
                       "<a href=javascript:void(0) onclick=getMoreDirectMessagePageStatus(%2)>下一页</a></p>";
            pageHtml = pageHtml.arg(page-1).arg(page+1);
        }
    } else {
        if(page<=1) pageHtml = "<p><a href=javascript:void(0) onclick=getMoreDirectMessagePageStatus(1)>首页</a></p>";
        else
        {
            pageHtml = "<p><a href=javascript:void(0) onclick=getMoreDirectMessagePageStatus(1)>首页</a>"
                       "<a href=javascript:void(0) onclick=getMoreDirectMessagePageStatus(%1)>上一页</a></p>";
            pageHtml = pageHtml.arg(page-1);
        }
    }
    contentHtml = sina->directMessagePageStatusToHtml(statusList,SinaWeibo::List);

    this->setWebviewHtml(basicHtml.arg(contentHtml),Left);
    delete sina;
}

void SinaWidget::addJavaScriptObject()
{
    leftWebView->page()
            ->mainFrame()
            ->addToJavaScriptWindowObject("dealWebkitEvent", this->dealWebkitEvent);
    rightWebView->page()
            ->mainFrame()
            ->addToJavaScriptWindowObject("dealWebkitEvent",this->dealWebkitEvent);
}

void SinaWidget::dealWebkitEventDisconnectThenConnect()
{
    QObject::connect(dealWebkitEvent, SIGNAL(getUserTimelineByScreenNameSignal(QString, int)),
                     this, SLOT(getOthersTimelineByScreenName(QString, int)));
    QObject::connect(dealWebkitEvent, SIGNAL(getMoreHomePageStatusSignal(int,int)),
                     this, SLOT(getMoreHomePageStatus(int,int)));
    QObject::connect(dealWebkitEvent, SIGNAL(getMoreUserPageStatusSignal(int,int)),
                     this, SLOT(getMoreUserPageStatus(int,int)));
    QObject::connect(dealWebkitEvent, SIGNAL(getMoreAtMePageStatusSignal(int)),
                     this, SLOT(getMoreAtMePageStatus(int)));
    QObject::connect(dealWebkitEvent, SIGNAL(getMoreCommentsPageStatusSignal(int)),
                     this, SLOT(getMoreCommentsPageStatus(int)));
    QObject::connect(dealWebkitEvent, SIGNAL(getMoreFavoritePageStatusSignal(int)),
                     this, SLOT(getMoreFavoritePageStatus(int)));
    QObject::connect(dealWebkitEvent, SIGNAL(getMoreDirectMessagePageStatusSignal(int)),
                     this, SLOT(getMoreDirectMessagePageStatus(int)));
    QObject::connect( dealWebkitEvent, SIGNAL(showBigPicture(QString, QString)),
                      this, SLOT(showPictueInWidget(QString,QString)));
    QObject::connect(dealWebkitEvent ,SIGNAL(forwardToOtherClientSignal(QString)),
                     this,SLOT(forwardToOtherClientSlot(QString)));
    QObject::connect(dealWebkitEvent,SIGNAL(getSessionStatusSignal(QString)),
                     this,SLOT(getSessionStatus(QString)));
    QObject::connect(dealWebkitEvent,SIGNAL(showUserInfoSignal(QString)),
                     this,SLOT(showUserInfo(QString)));
    QObject::connect(dealWebkitEvent,SIGNAL(showFriendsSignal(QString)),
                     this,SLOT(showFriends(QString)));
    QObject::connect(dealWebkitEvent,SIGNAL(showFollowersSignal(QString)),
                     this,SLOT(showFollowers(QString)));
    QObject::connect(dealWebkitEvent,SIGNAL(showCommentsSignal(QString,QString)),
                     this,SLOT(showComments(QString,QString)));
    QObject::connect(dealWebkitEvent,SIGNAL(showPublicStatusSignal()),
                     this,SLOT(showpublicStatuses()));
    QObject::connect(dealWebkitEvent,SIGNAL(showDailyTrendsSignal()),
                     this,SLOT(showDailyTrends()));
    QObject::connect(dealWebkitEvent,SIGNAL(playVedioSignal(QString)),
                     this,SLOT(playVedio(QString)));


     QObject::connect(this->dealWebkitEvent,SIGNAL(getNewPageInfoSignal(QString,int)),this,SLOT(getNewPage(QString,int)));

}

void SinaWidget::newWeiboButtonClicked()
{
    if (this->pStatusWidget == NULL)
    {
        pStatusWidget = new StatusWidget(this->account,this);
        pStatusWidget->setWindowTitle(tr("发新微博"));
    }
    pStatusWidget->freeSendButton();
    pStatusWidget->clearContent();
    pStatusWidget->show();
}

void SinaWidget::homePageButtonClicked()
{
    /*
    QLabel *pLabel = new QLabel("正在载入...",this->parentWidget());
    pLabel->setGeometry(100,100,50,100);
    pLabel->show();
    */
    this->preHtml();
    SinaWeibo *sina = new SinaWeibo(account);
    if(!homePageHtmlReady)
    {
        if(!homePageStatusReady)
        {
            sina->initHomePageStatus();
            homePageStatusReady = true;
        }
        homePageHtml = basicHtml
                       .arg(sina->homePageStatusToHtml(
                               account->homePageStatus,
                               account->homePageRetweeted)
                            );
//        homePageHtmlReady = true;
        this->statusUnread=0;

    }
    this->setWebviewHtml(homePageHtml,Left);

    QFile file(QCoreApplication::applicationDirPath() + "/homePageHtml.html");
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        qDebug(file.errorString().toLocal8Bit());
    }

    file.write(homePageHtml.toLocal8Bit());
    file.close();

    while(!dealWebkitEvent->isDomReady())
    {
        QApplication::processEvents();
    }
//    dealWebkitEvent->sendStatusesCounts(countsInJsonFormat);
    delete sina;
}

void SinaWidget::myWeiboPageButtonClicked()
{
    this->preHtml();
    SinaWeibo *sina = new SinaWeibo(account);
    if(!myWeiboPageHtmlReady)
    {
        if(!myWeiboPageStatusReady)
        {
            sina->initUserWeiboPageStatus();
        }
        myWeiboPageHtml = basicHtml
                          .arg(sina->userWeiboPageStatusToHtml(
                                  account->userWeiboPageStatus,
                                  account->userWeiboPageRetweeted)
                               );
        /*
        myWeiboPageHtml.append( "<p><a href=javascript:void(0) onclick=getMoreUserPageStatus(1,0)>首页</a>"
                                 "<a href=javascript:void(0) onclick=getMoreUserPageStatus(2,0)>下一页</a>"
                                 "</p>");
                                 */
    }
    this->setWebviewHtml(myWeiboPageHtml,Left);
 //   QString countsInJsonFormat = sina->getStatusCountsByJson(account->userWeiboPageStatus);
    while(!dealWebkitEvent->isDomReady())
    {
        QApplication::processEvents();
    }
  //  dealWebkitEvent->sendStatusesCounts(countsInJsonFormat);
    delete sina;
}

void SinaWidget::atMePageButtonClicked()
{
    this->preHtml();
    SinaWeibo *sina = new SinaWeibo(account);
    if(!atMePageHtmlReady)
    {
        if(!atMePageStatusReady)
        {
            sina->initAtMePageStatus();
            atMePageStatusReady = true;
        }
        atMePageHtml = basicHtml
                       .arg(sina->atMePageStatusToHtml(
                               account->atMePageStatus,
                               account->atMePageRetweeted)
                            );

        this->atMeMentionsUnread = 0;
    }
    this->setWebviewHtml(atMePageHtml,Left);
    while(!dealWebkitEvent->isDomReady())
    {
        QApplication::processEvents();
    }
    delete sina;
}

void SinaWidget::commentPageButtonClicked()
{
    this->preHtml();
    if(!commentPageHtmlReady)
    {
        SinaWeibo *sina = new SinaWeibo(account);
        if(!commentPageStatusReady)
        {
            sina->initCommentPageStatus();
            commentPageStatusReady = true;
        }
        commentPageHtml = basicHtml
                          .arg(sina->commentPageStatusToHtml(
                                  account->commentPageComments,
                                  account->commentPageRetweeted)
                               );

//        commentPageHtmlReady = true;
        delete sina;

        this->commentsUnread = 0;
//        this->refreshSystemTrayIconTooltip();
    }

    this->setWebviewHtml(commentPageHtml,Left);
}

void SinaWidget::favouritePageButtonClicked()
{
    this->preHtml();
    SinaWeibo *sina = new SinaWeibo(account);
    if(!favoritePageHtmlReady)
    {
        if(!favoritePageStatusReady)
        {
            sina->initFavoritePageStatus();
        }
        favoritePageHtml = basicHtml
                           .arg(sina->favoritePageStatusToHtml(
                                   account->favoritesPageStatus,
                                   account->favoritesPageRetweeted)
                                );
    }
    this->setWebviewHtml(favoritePageHtml,Left);
//    QString countsInJsonFormat = sina->getStatusCountsByJson(account->favoritesPageStatus);
    while(!dealWebkitEvent->isDomReady())
    {
        QApplication::processEvents();
    }
 //   dealWebkitEvent->sendStatusesCounts(countsInJsonFormat);
    delete sina;
}

void SinaWidget::messagePageButtonClicked()
{
    this->preHtml();
    if(!directMessagePageHtmlReady)
    {
        SinaWeibo *sina = new SinaWeibo(account);
        if(!directMessagePageStatusReady)
        {
            sina->initDirectMessagePageStatus();
            directMessagePageStatusReady = true;
        }
        directMessagePageHtml = basicHtml
                                .arg(sina->directMessagePageStatusToHtml(
                                        account->directMessagePageStatus,SinaWeibo::List));
//        this->directMessagePageHtmlReady = true;
        delete sina;

        this->directMessageUnread = 0;

    }
    this->setWebviewHtml(directMessagePageHtml,Left);
}

void SinaWidget::configureButtonClicked()
{

}

void SinaWidget::publicStatusesButtonClicked()
{
    this->preHtml();

    SinaWeibo *sina = new SinaWeibo(account);

    if(!publicStatusesHtmlReady)
    {
        if(!publicStatusesReady)
        {
            sina->initPublicStatus();
        }

        publicStatusesHtml = basicHtml
                       .arg(sina->homePageStatusToHtml(account->publicStatus,account->publicRetweeted)
                            );
    }


    this->setWebviewHtml(publicStatusesHtml,Right);

    while(!dealWebkitEvent->isDomReady())
    {
        QApplication::processEvents();
    }

    delete sina;


}

void SinaWidget::squareButtonClicked()
{
    this->preHtml();

    SinaWeibo *sina = new SinaWeibo(account);

    if (!squareHtmlReady)
    {
        squareHtml = sina->getSquareHtml();
        squareHtmlReady = true;
    }

    this->setWebviewHtml(basicHtml.arg(squareHtml),Left);

    while(!dealWebkitEvent->isDomReady())
    {
        QApplication::processEvents();
    }

    delete sina;
}

void SinaWidget::systemTrayIconAction(QSystemTrayIcon::ActivationReason reason)
{
    if(reason==QSystemTrayIcon::DoubleClick)
    {
        this->showNormal();
        QApplication::alert(this,1000);
    }
    if(reason==QSystemTrayIcon::Trigger)
    {
        this->showNormal();
        QApplication::alert(this,1000);
    }
}

void SinaWidget::createSystemTrayIcon()
{

    this->systemTrayIcon = new QSystemTrayIcon(this);
    this->restoreAction = new QAction(tr("打开主面板"), this);
    QObject::connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));
    QObject::connect(systemTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                     this, SLOT(systemTrayIconAction(QSystemTrayIcon::ActivationReason)));
    this->quitAction = new QAction(tr("退出"), this);
    QObject::connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    this->systemTrayMenu = new QMenu(this);
    this->systemTrayMenu->addAction(restoreAction);
    this->systemTrayMenu->addAction(quitAction);
    this->systemTrayIcon->setContextMenu(this->systemTrayMenu);
    this->systemTrayIcon->setVisible(true);

}

void SinaWidget::refreshSystemTrayIconTooltip()
{
    QString msg = "未读微博%1条,\n新评论%2条,\n新私信%3条,\n有%4条微博提到我";
    msg = msg
          .arg(statusUnread)
          .arg(commentsUnread)
          .arg(directMessageUnread)
      //    .arg(newFollower)
          .arg(atMeMentionsUnread);
    systemTrayIcon->setToolTip(msg);
}


void SinaWidget::setWebviewHtml(QString html,WebViewPos pos)
{
    QWebView *pWebView = NULL;
    if (pos == Left)
        pWebView = leftWebView;
    else
    {
        if (!rightWebView->isVisible())
            rightWebView->show();
        pWebView = rightWebView;
        QString tmpStr = "<script type=\"text/javascript\">"
                         "window.onscroll = function(){}"
                         "</script>"
                         "</body>";

        if (!html.contains(tmpStr))
            html.replace("</body>",tmpStr);
    }

    QObject::connect( leftWebView->page()->mainFrame(),
                      SIGNAL(javaScriptWindowObjectCleared()),
                      this,
                      SLOT(addJavaScriptObject())
                      );

    QObject::connect(rightWebView->page()->mainFrame(),
                     SIGNAL(javaScriptWindowObjectCleared()),
                     this,
                     SLOT(addJavaScriptObject()));

    pWebView->setHtml(html);
}

void SinaWidget::updateIcon(QPixmap *pixmap, int unreadCount)
{
    QPainter painter(pixmap);
    painter.setPen(Qt::red);
    QFont font("Arial", 10, 10);
    font.setBold(true);
    painter.setFont(font);
    painter.drawText(0, 0, 32, 32, Qt::AlignCenter, QString::number(unreadCount), 0);
}

QString SinaWidget::getImageUrl(){

    if (this->account == NULL){
        return QString("");
    }
    else{
        return this->account->getProfileImageUrl();
    }

}

bool SinaWidget::getUserInfo(QString userId)
{
    if (this->otherAccount == NULL)
        this->otherAccount = new Account;
    SinaWeibo *pSina = new SinaWeibo(this->account);

    if ( false == pSina->getSinaAccountByID(*(this->otherAccount),userId) )
    {
        delete pSina;
        return false;
    }
    else
    {
        delete pSina;
        return true;
    }
}

void SinaWidget::forwardToOtherClientSlot(QString str)
{
    emit forwardToOtherClientSignal(str);
}

void SinaWidget::forward_from_otherClient(QString str)
{
    QRegExp imgRegExp("<img src=.*>.*</img>");
    imgRegExp.setMinimal(true);
    if (str.indexOf(imgRegExp))
        if(imgRegExp.cap(0).length() != 0)
            str.replace(imgRegExp.cap(0),"");

    if (str.length() > 135)
    {
        str = str.left(135);
    }
    if (pStatusWidget == NULL)
    {
        pStatusWidget = new StatusWidget(this->account,this);
    }

    pStatusWidget->freeSendButton();
    pStatusWidget->clearContent();
    pStatusWidget->setText(str);
    pStatusWidget->show();
}

QPixmap SinaWidget::getPixmapFromUrl(QUrl url)
{
    QNetworkRequest request; 
    QString urlStr = url.toString().replace(QRegExp("/50/"),"/180/");
    url.setUrl(urlStr);
    request.setUrl(url);
    Http *http = new Http(this);
    QByteArray byteArray = http->getImageByteArrayFromUrl(request);
    QImage img = QImage::fromData(byteArray);
    QPixmap image = QPixmap::fromImage(img);

    http->deleteLater();

    return image;
}



void SinaWidget::changeSelectedPixmap(int index, int type)
{
    QString styleSheet="QWidget#%1%2%3{background-image:url(:/images/sina/%4.png)}";

    if (type == 0)//remove
    {
        styleSheet = styleSheet.arg("label_").arg("left_").arg(QString::number(index));
        switch (index)
        {
        case 1:
            styleSheet = styleSheet.arg("myFriendsPage");
            ui->left_1->setStyleSheet(styleSheet);
            break;
        case 2:
            styleSheet = styleSheet.arg("atMePage");
            ui->left_2->setStyleSheet(styleSheet);
            break;
        case 3:
            styleSheet = styleSheet.arg("myWeiboPage");
            ui->left_3->setStyleSheet(styleSheet);
            break;
        case 4:
            styleSheet = styleSheet.arg("newStatus");
            ui->left_4->setStyleSheet(styleSheet);
            break;
        case 5:
            styleSheet = styleSheet.arg("commentsPage");
            ui->left_5->setStyleSheet(styleSheet);
            break;
        case 6:
            styleSheet = styleSheet.arg("myFavouritePage");
            ui->left_6->setStyleSheet(styleSheet);
            break;
        case 7:
            styleSheet = styleSheet.arg("directMessage");
            ui->left_7->setStyleSheet(styleSheet);
            break;
        case 8:
            styleSheet = styleSheet.arg("publicPage");
            ui->left_8->setStyleSheet(styleSheet);
            break;
        }
    }
    else if (type == 1)//add
    {
        styleSheet = styleSheet.arg("label_").arg("left_").arg(QString::number(index));
        switch (index)
        {
        case 1:
            styleSheet = styleSheet.arg("myFriendsPageLighted");
            ui->left_1->setStyleSheet(styleSheet);
            break;
        case 2:
            styleSheet = styleSheet.arg("atMePageLighted");
            ui->left_2->setStyleSheet(styleSheet);
            break;
        case 3:
            styleSheet = styleSheet.arg("myWeiboPageLighted");
            ui->left_3->setStyleSheet(styleSheet);
            break;
        case 4:
            styleSheet = styleSheet.arg("newStatusLighted");
            ui->left_4->setStyleSheet(styleSheet);
            break;
        case 5:
            styleSheet = styleSheet.arg("commentsPageLighted");
            ui->left_5->setStyleSheet(styleSheet);
            break;
        case 6:
            styleSheet = styleSheet.arg("myFavouritePageLighted");
            ui->left_6->setStyleSheet(styleSheet);
            break;
        case 7:
            styleSheet = styleSheet.arg("directMessageLighted");
            ui->left_7->setStyleSheet(styleSheet);
            break;
        case 8:
            styleSheet = styleSheet.arg("publicPageLighted");
            ui->left_8->setStyleSheet(styleSheet);
            break;
    }
    }
    else{
        qDebug("changeSelectedPixmap type error!!!");
    }

}

void SinaWidget::getSessionStatus(QString senderId)
{
    SinaWeibo sina(this->account);

    this->account->RemoveAll(this->account->directMessagePageStatusInSession);
    sina.getDMSessionStatus(this->account->directMessagePageStatusInSession,senderId);
    QString sessionHtml = sina.directMessagePageStatusToHtml(this->account->directMessagePageStatusInSession,SinaWeibo::Session);

    this->setWebviewHtml(basicHtml.arg(sessionHtml),Right);
}

void SinaWidget::showUserInfo(QString userId)
{
    QString userInfoHtml;
    QFile userInfoFile(":/other/res/sina/UserProfile.html");
    if(!userInfoFile.open(QFile::ReadOnly))
    {
        qDebug("open Error!!!");
        qDebug(userInfoFile.errorString().toAscii());
    }
    QTextStream stream(&userInfoFile);
    userInfoHtml = stream.readAll();
    userInfoFile.close();

    QString genderImgSrc;

    if (false == getUserInfo(userId))
    {

    }
    else{

        userInfoHtml.replace("userid",this->otherAccount->getId());

        if (this->otherAccount->getGender() == "Male")
        {
            genderImgSrc = "qrc:/images/sina/male.png";
        }
        else if (this->otherAccount->getGender() == "Female")
        {
            genderImgSrc = "qrc:/images/sina/female.png";
        }

        QString atBtn = "<a class=\"btn blue\" onclick=sendStatusWithAt('"+this->otherAccount->getScreenName()+"')>@Ta</a>";
        QString dmBtn = "<a class=\"btn blue\" onclick=sendDirectMessage('"+this->otherAccount->getId()+"')>发私信</a>";
        QString space = "";

        userInfoHtml = userInfoHtml.arg(this->otherAccount->getProfileImageUrl().replace("/50/","/180/"))
                                    .arg(this->otherAccount->getScreenName())
                                    .arg(genderImgSrc)//need change
                                    .arg(this->otherAccount->getLocation())
                                    .arg(this->otherAccount->getDescription())
                                    .arg(this->otherAccount->getFriends())
                                    .arg(this->otherAccount->getFollower())
                                    .arg(this->otherAccount->getWeiboCount())
                                    .arg(atBtn + space +dmBtn)
                                    ;
    }

    SinaWeibo sina(this->account);
    SinaParserXml parser;

    QString content = sina.getUserTimeline(userId,10);
    this->otherAccount->RemoveAll(this->otherAccount->userWeiboPageStatus);

    parser.parserStatusXml(content,this->otherAccount->userWeiboPageStatus,this->otherAccount->userWeiboPageRetweeted);

    QString statusHtml = sina.userWeiboPageStatusToHtml(this->otherAccount->userWeiboPageStatus,this->otherAccount->userWeiboPageRetweeted);


    this->setWebviewHtml(basicHtml.arg(userInfoHtml.append(statusHtml)),Right);

}

void SinaWidget::showFriends(QString userId)
{
    SinaWeibo sina(this->account);
    QString content = sina.getFriends(userId);
    QList<Account> accountList;
    SinaParserXml parser;
    parser.parserFriendsAndFollowersXml(accountList,content);
    QString htmlContent = basicHtml.arg(sina.FriendsAndFollowersToHtml(accountList));

    this->setWebviewHtml(htmlContent,Right);


}

void SinaWidget::showFollowers(QString userId)
{
    SinaWeibo sina(this->account);
    QString content = sina.getFollowers(userId);
    QList<Account> accountList;
    SinaParserXml parser;
    parser.parserFriendsAndFollowersXml(accountList,content);
    QString htmlContent = basicHtml.arg(sina.FriendsAndFollowersToHtml(accountList));
    this->setWebviewHtml(htmlContent,Right);

}

void SinaWidget::showComments(QString userId,QString statusId)
{
    SinaWeibo sina(this->account);
    QString content = sina.getCommentsList(statusId);
    QString contentFrame =  "<div style=\"font-family:微软雅黑;\">  %1 </div>";

    QList<Status> statusList;
    SinaParserXml parser;
    parser.parserOthersComments(content,statusList);

    QString commentsHtml = sina.othersCommentsToHtml(statusList);

    if(commentsHtml.length() == 0)
        commentsHtml = contentFrame.arg("<p>暂时还没有评论哦~</p>");

    QFile commentsFile(":/other/res/sina/Comments.html");
    if (!commentsFile.open(QFile::ReadOnly))
    {
        qDebug(commentsFile.errorString().toLocal8Bit());
    }
    QTextStream commentsStream(&commentsFile);
    QString commentHeaderHtml = commentsStream.readAll();
    commentsFile.close();

    sina.getUserTimeline(userId);

    if (false != getUserInfo(userId))
    {

        QString genderImgSrc;

        if (this->otherAccount->getGender() == "Male")
        {
            genderImgSrc = "qrc:/images/sina/male.png";
        }
        else if (this->otherAccount->getGender() == "Female")
        {
            genderImgSrc = "qrc:/images/sina/female.png";
        }

        QString tmpStr = sina.getSingleStatus(statusId);

        QRegExp regexp("<text>(.*)</text>");

        regexp.setMinimal(true);

        regexp.indexIn(tmpStr);

        QString statusContent = regexp.cap(1);


        commentHeaderHtml =  contentFrame.arg(commentHeaderHtml.arg(this->otherAccount->getProfileImageUrl().replace("/50/","/180/"))
                                            .arg(this->otherAccount->getScreenName())
                                            .arg(genderImgSrc)
                                            .arg(sina.statusTextFormat(statusContent)));

        commentHeaderHtml.replace("userid",this->otherAccount->getId());
    }

    this->setWebviewHtml(basicHtml.arg(commentHeaderHtml.append("<p></p>"+commentsHtml)),Right);

}

void SinaWidget::showpublicStatuses()
{
    publicStatusesButtonClicked();
}

void SinaWidget::showDailyTrends()
{
    SinaWeibo *pSina = new SinaWeibo(this->account);
    if (!dailyTrendsHtmlReady)
    {
        QString xmlContent = pSina->getDailyTrends();
        QStringList nameList,queryList;
        SinaParserXml parser;
        parser.parserTrends(xmlContent,nameList,queryList);
        dailyTrendsHtml =  pSina->dailyTrendsToHtml(nameList,queryList);
        dailyTrendsHtmlReady = true;
    }

    this->setWebviewHtml(basicHtml.arg(dailyTrendsHtml),Right);
    delete pSina;
}

void SinaWidget::playVedio(QString url)
{
    if (url.contains("youku"))
    {
        QRegExp rx("id_(.+)\.html");
        url.indexOf(rx);
        QString video_id;
        video_id = rx.cap(1);
        QString url="http://player.youku.com/player.php/sid/"+video_id+"/v.swf";
        rightWebView->load(QUrl(url));
        rightWebView->show();
    }
    else if (url.contains("tudou"))
    {
        url.replace("programs/view","v");
        url.append("v.swf");
        rightWebView->load(QUrl(url));
        rightWebView->show();
    }
}

