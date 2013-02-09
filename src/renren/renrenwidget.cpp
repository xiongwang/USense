#include "renrenwidget.h"
#include "ui_renrenwidget.h"

#include <QtWebKit>


RenrenWidget::RenrenWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RenrenWidget)
{
    ui->setupUi(this);
    this->leftWebView = new MyWebView(this);
    this->rightWebView = new MyWebView(this);
    ui->leftLayout->addWidget(leftWebView);
    ui->rightLayout->addWidget(rightWebView);

    this->dealRenrenEvent = new Dealrenrenevent(this->prenrenAccount);
    this->pEmotionApi = new EmotionApi();
    this->pFeedApi = new FeedApi;
    this->pGetFriendsApi = new GetFriendsApi;
    this->pStatusApi = new StatusApi();
    this->pUserApi = new UserApi;
    this->pBlogApi = new BlogApi();
    this->pPhotoApi = new PhotoApi();
    this->pShareApi = new ShareApi();

    this->pStatusWidget = NULL;


    leftWebView->setHtml("<body style=\"background-color:rgb(59,59,59);font-family:微软雅黑\"></body>");
    rightWebView->setHtml("<body style=\"background-color:rgb(59,59,59);font-family:微软雅黑\"></body>");
    rightWebView->settings()->setAttribute(QWebSettings::PluginsEnabled, true);
    //leftWebView->settings()->setFontFamily();
    connect(this->dealRenrenEvent,SIGNAL(getShareInfoSignal(QString)),this,SLOT(ReadFeed(QString)));
    connect(this->dealRenrenEvent,SIGNAL(getNewPageSignal(int)),this,SLOT(SetNewPage(int)));
    connect(this->dealRenrenEvent,SIGNAL(getProfileInfoSignal(QString)),this,SLOT(setProfilePage(QString)));
    connect(this->dealRenrenEvent,SIGNAL(getStatusInfoSignal(QString)),this,SLOT(setStatusPage(QString)));
    connect(this->dealRenrenEvent,SIGNAL(getAlbumsInfoSignal(QString)),this,SLOT(setAlbumsPage(QString)));
    connect(this->dealRenrenEvent,SIGNAL(getBlogsInfoSignal(QString)),this,SLOT(setBlogsPage(QString)));
    connect(this->dealRenrenEvent,SIGNAL(getSameFriendsSignal()),this,SLOT(setSameFriends()));
    connect(this->dealRenrenEvent,SIGNAL(readStatusSignal(QString,QString)),this,SLOT(ReadStatusFeed(QString,QString)));
    connect(this->dealRenrenEvent,SIGNAL(readBlogSignal(QString,QString)),this,SLOT(ReadBlogFeed(QString,QString)));

    QString basicHtml;
    QFile basicHtmlFile(":/other/res/renren/RenrenPage.html");
    if(!basicHtmlFile.open(QFile::ReadOnly))
    {
        qDebug("open Error!!!");
        qDebug(basicHtmlFile.errorString().toAscii());
    }
    QTextStream stream(&basicHtmlFile);
    basicHtml = stream.readAll();
    basicHtmlFile.close();

    myFriends.load(":/images/renren/myFriends.png");
    myFriendsLighted.load(":/images/renren/myFriendsLighted.png");

    sendStatus.load(":/images/renren/sendStatus.png");
    sendStatus.load(":/images/renren/sendStatusLighted.png");

    newThings.load(":/images/renren/newThings.png");
    newThingsLighted.load(":/images/renren/newThingsLighted.png");

    this->currentSelected = 1;
    changeSelectedPixmap(1,1);

    QObject::connect( leftWebView->page()->mainFrame(),
                      SIGNAL(javaScriptWindowObjectCleared()),
                      this,
                      SLOT(addJavaScriptObject())
                      );

    QObject::connect( rightWebView->page()->mainFrame(),
                      SIGNAL(javaScriptWindowObjectCleared()),
                      this,
                      SLOT(addJavaScriptObject())
                      );


    QObject::connect(this->dealRenrenEvent,SIGNAL(createStatusWidgetSignal(QString,QString,QString,QString,int)),
                     this,SLOT(createStatusWidgetSlot(QString,QString,QString,QString,int)));
    QObject::connect(this->dealRenrenEvent,SIGNAL(forwardToOtherClientSignal(QString)),
                     this,SLOT(forwardToOtherClientSlot(QString)));
    QObject::connect(this->dealRenrenEvent,SIGNAL(playVideoSignal(QString)),
                     this,SLOT(PlayVideo(QString)));
}

RenrenWidget::~RenrenWidget()
{
    delete ui;
}

void RenrenWidget::addJavaScriptObject()
{
    leftWebView->page()->mainFrame()->addToJavaScriptWindowObject("dealRenrenEvent",this->dealRenrenEvent);
    rightWebView->page()->mainFrame()->addToJavaScriptWindowObject("dealRenrenEvent",this->dealRenrenEvent);
}

void RenrenWidget::getFeed()
{
    /*
    if (statusPage.length() != 0)
    {
        this->leftWebView->setHtml(statusPage);
        return;
    }
    */
    int page = 1;
    pFeedApi->vFeedInfo.clear();

    QDateTime currentDT = QDateTime::currentDateTime();
    QUrl url = this->pUserApi->getUserInfo(this->pUserApi->account.getUid()).headurl;
    httpGet.initPixMap(url);

    pFeedApi->getFeed(page);

    QFile feedHtmlFile(":/other/res/renren/Feed.html");
    if (!feedHtmlFile.open(QFile::ReadOnly))
    {
        //qDebug(feedHtmlFile.errorString().toLocal8Bit());
    }
    QTextStream stream1(&feedHtmlFile);
    QString everyFeedHtml = stream1.readAll();
    feedHtmlFile.close();

    QString tmp;
    QString html;

    for(int i=0; i<pFeedApi->vFeedInfo.size(); i++)
    {
        FeedInfo feed = pFeedApi->vFeedInfo.at(i);
        QString feedHtml = everyFeedHtml;
        feedHtml = feedHtml.arg(feed.headurl).arg(feed.post_id);

        if(feed.feed_type == "10" && feed.prefix.contains("转自", Qt::CaseInsensitive) || feed.feed_type == "20" || feed.feed_type == "21")
            html += feed.prefix+" "
                    "<div class=\"arrow\">"
                    "<div class=\"arrow_border\"></div>"
                    "<div class=\"arrow_content\"></div>"
                    "</div>"
                    "<div class=\"main\">"
                            "<b class=\"b1\"></b>"
                            "<b class=\"b2\"></b>"
                            "<b class=\"b3\"></b>"
                            "<b class=\"b4\"></b>"
                            "<div class=\"content\" style=\"font-size:smaller;\">"
                            "<a>"+feed.message.left(80)+"</a>"
                            "<a>"+feed.title +"</a>"
                            "<a>"+feed.description.left(80)+"...</a>"
                            "</div>"
                            "<b class=\"b5\"></b>"
                            "<b class=\"b6\"></b>"
                            "<b class=\"b7\"></b>"
                            "<b class=\"b8\"></b>"
                    "</div>";
        else
            if(feed.feed_type == "10")
                html += "<a>"+feed.message+"</a>"
                        "<a>"+feed.title +"</a>"
                        "<a>"+feed.description+"</a>";
            else
                html += feed.prefix+" "
                        "<a>"+feed.message+"</a>"
                        "<a>"+feed.title+"</a>"
                        "<a>"+feed.description+"</a>";

        uint min = (feed.vfeed_media.size() > 2) ? 2 : feed.vfeed_media.size();

        for(uint j = 0; j < min; j++)
        {
            if(j==0)
            {
                if(feed.feed_type == "50")
                {
                    html += "<div onclick=\"playVedio('"+feed.vfeed_media.at(j).href+"')\" style=\"width:120px;height:90px;background-repeat:no-repeat;background-position:center;background-image: url("+ feed.vfeed_media.at(j).src +"); \">"
                            "<img src=\"http://img.t.sinajs.cn/t35/style/images/common/feedvideoplay.gif\" style=\"cursor: pointer; display: block;float:none; margin:auto; padding-top:30px; opacity: 0.5;\" ></img>"
                            "</div>";
                }
                else
                    html += "<div><img  src=" + feed.vfeed_media.at(j).src + "></image></div>";
            }
            else
                html += "<div><img  src=" + feed.vfeed_media.at(j).src + "></image></div>";
        }

        feedHtml = feedHtml.arg(feed.name + " " + html);
        html.clear();
        QString tmpmenu;
        QString menu;
        if(feed.feed_type == "10")
        {

            tmpmenu = "<li><a  onclick=createStatusWidget("+feed.source_id+","+feed.actor_id +","+"\"\""+",\"\""+",2)>回复</a></li>";
            tmpmenu += "<li><a onclick=createStatusWidget("+feed.source_id+","+feed.actor_id +","+"\"\""+",\"\""+",1)>转发</a></li>";
            tmpmenu += "<li><a  onclick=\"forwardToOtherClient('"+pEmotionApi->RemoveEmotion(feed.message)+"')\">plus</a></li>";
            menu =tmpmenu;
            html += "<ul class=\"menu\" onclick=dropmenu(this,'"+menu.toAscii().toBase64()+"')><a>+</a></ul>";
            //html+="<div onclick=alert('123123')>menu</div>";
        }
        if(feed.feed_type == "20")
        {
            tmpmenu = "<li><a onclick=createStatusWidget("+feed.source_id+","+feed.actor_id +","+"\"\""+",\"\""+",3)>评论</a></li>";
            tmpmenu += "<li><a onclick=createStatusWidget(1,"+feed.source_id+","+feed.actor_id +","+"\"\""+",6)>分享</a></li>";
            menu =tmpmenu;
            html += "<ul class=\"menu\" onclick=dropmenu(this,'"+menu.toAscii().toBase64()+"')><a>+</a></ul>";
        }
        if(feed.feed_type == "21")
        {
            tmpmenu = "<li><a  onclick=createStatusWidget("+feed.source_id+","+feed.actor_id +","+"\"\""+",\"\""+",4)>评论</a></li>";
            tmpmenu += "<li><a  onclick=createStatusWidget(20,"+feed.source_id+","+feed.actor_id +","+"\"\""+",6)>分享</a></li>";
            menu =tmpmenu;
            html += "<ul class=\"menu\" onclick=dropmenu(this,'"+menu.toAscii().toBase64()+"')><a>+</a></ul>";
        }
        if(feed.feed_type == "30")
        {
            tmpmenu = "<li><a  onclick=createStatusWidget(\"\"," +feed.vfeed_media.at(0).media_id+","+feed.vfeed_media.at(0).owner_id +","+"\"\""+",5)>评论</a></li>";
            tmpmenu += "<li><a  onclick=createStatusWidget(2,"+feed.vfeed_media.at(0).media_id+","+feed.vfeed_media.at(0).owner_id +","+"\"\""+",6)>分享</a></li>";
            menu = tmpmenu;
            html += "<ul class=\"menu\" onclick=dropmenu(this,'"+menu.toAscii().toBase64()+"')><a>+</a></ul>";
        }
        if(feed.feed_type == "32")
        {
            tmpmenu = "<li><a  onclick=createStatusWidget("+feed.source_id+","+feed.actor_id +","+"\"\""+",\"\""+",4)>评论</a></li>";
            tmpmenu += "<li><a  onclick=createStatusWidget(20,"+feed.source_id+","+feed.actor_id +","+"\"\""+",6)>分享</a></li>";
            menu = tmpmenu;
            html += "<ul class=\"menu\" onclick=dropmenu(this,'"+menu.toAscii().toBase64()+"')><a>+</a></ul>";
        }
        if(feed.feed_type == "33")
        {
            tmpmenu = "<li><a onclick=createStatusWidget("+feed.source_id+","+feed.actor_id +","+"\"\""+",\"\""+",4)>评论</a></li>";
            tmpmenu += "<li><a  onclick=createStatusWidget(20,"+feed.source_id+","+feed.actor_id +","+"\"\""+",6)>分享</a></li>";
            menu = tmpmenu;
            html += "<ul class=\"menu\" onclick=dropmenu(this,'"+menu.toAscii().toBase64()+"')><a>+</a></ul>";
        }
        if(feed.feed_type == "50")
        {
            tmpmenu = "<li><a  onclick=createStatusWidget("+feed.source_id+","+feed.actor_id +","+"\"\""+",\"\""+",4)>评论</a></li>";
            tmpmenu += "<li><a  onclick=createStatusWidget(20,"+feed.source_id+","+feed.actor_id +","+"\"\""+",6)>分享</a></li>";
            menu = tmpmenu;
            html += "<ul class=\"menu\" onclick=dropmenu(this,'"+menu.toAscii().toBase64()+"')><a>+</a></ul>";
        }

        feedHtml = feedHtml.arg(html);
        html.clear();

        QString publish;
        QString num;
        QDate currentDate = currentDT.date();
        QTime currentTime = currentDT.time();
        if(currentDate.year()==feed.update_time.left(4).toInt())
            if(currentDate.month()==feed.update_time.mid(5,2).toInt())
                if(currentDate.day()==feed.update_time.mid(8,2).toInt())
                    if(currentTime.hour()==feed.update_time.mid(11,2).toInt())
                        if(currentTime.minute()==feed.update_time.mid(14,2).toInt())
                        {
                            publish = "发布于"+num.setNum(currentTime.second()-feed.update_time.mid(17,2).toInt())+"秒前";
                        }
                        else
                        {
                            publish = "发布于"+num.setNum(currentTime.minute()-feed.update_time.mid(14,2).toInt())+"分钟前";
                        }
                    else
                    {
                        publish = "发布于"+num.setNum(currentTime.hour()-feed.update_time.mid(11,2).toInt())+"小时前";
                    }
                else
                {
                    publish = "发布于"+num.setNum(currentDate.day()-feed.update_time.mid(8,2).toInt())+"天前";
                }
            else
            {
                publish = "发布于"+num.setNum(currentDate.month()-feed.update_time.mid(5,2).toInt())+"月前";
            }
        else
            publish = feed.update_time;

    feedHtml = feedHtml.arg(publish+" 评论数："+feed.count);
    feedHtml = feedHtml.arg(feed.actor_id);
    tmp.append(feedHtml);
    }

    QString basicHtml;
    QFile basicHtmlFile(":/other/res/renren/RenrenHomePage.html");
    if(!basicHtmlFile.open(QFile::ReadOnly))
    {
        qDebug("open Error!!!");
        qDebug(basicHtmlFile.errorString().toAscii());
    }
    QTextStream basic(&basicHtmlFile);
    basicHtml = basic.readAll();
    basicHtmlFile.close();

    basicHtml = basicHtml.arg(tmp).arg("微软雅黑");
    basicHtml = this->pEmotionApi->TextHandler(basicHtml);
    statusPage = basicHtml;

    this->leftWebView->setHtml(basicHtml);
}

void RenrenWidget::getFriends()
{
    pGetFriendsApi->getFriendsFromAccount();
}


void RenrenWidget::SetFriendPage()
{
    if (friendsPage.length() != 0)
    {
        this->leftWebView->setHtml(friendsPage);
        return;
    }
    QString html;

    for(uint i = 0; i < pGetFriendsApi->vFriends.size(); i+=3)
    {
        html+="<table width=\"100%\"><tr><td width=\"33%\">"
                "<div style=\"float:left;overflow:hidden;\">"
                "<img src=" + pGetFriendsApi->vFriends.at(i).tinyurl + " width=\"50\"  height=\"50\" onclick=\"getProfileInfo('"+ pGetFriendsApi->vFriends.at(i).id +"')\"></img>"
                "<div><small>"+ pGetFriendsApi->vFriends.at(i).name + "</small></div>"
                "</div>"
                "</td>";
        if(i+2<=pGetFriendsApi->vFriends.size())
            html+="<td width=\"33%\">"
                    "<div style=\"float:left;overflow:hidden;\">"
                    "<img src=" + pGetFriendsApi->vFriends.at(i+1).tinyurl + " width=\"50\"  height=\"50\" onclick=\"getProfileInfo('"+ pGetFriendsApi->vFriends.at(i+1).id +"')\"></img>"
                    "<div>"+ pGetFriendsApi->vFriends.at(i+1).name + "</div>"
                    "</div>"
                    "</td>";
        if(i+3<=pGetFriendsApi->vFriends.size())
            html+="<td width=\"33%\">"
                    "<div style=\"float:left;overflow:hidden;\">"
                    "<img src=" + pGetFriendsApi->vFriends.at(i+2).tinyurl + " width=\"50\"  height=\"50\" onclick=\"getProfileInfo('"+ pGetFriendsApi->vFriends.at(i+2).id +"')\"></img>"
                    "<div>"+ pGetFriendsApi->vFriends.at(i+2).name + "</div>"
                    "</div>"
                    "</td></tr></table>";
    }
    QString basicHtml;
    QFile basicHtmlFile(":/other/res/renren/RenrenHomePage2.html");
    if(!basicHtmlFile.open(QFile::ReadOnly))
    {
        qDebug("open Error!!!");
        qDebug(basicHtmlFile.errorString().toAscii());
    }
    QTextStream basic(&basicHtmlFile);
    basicHtml = basic.readAll();
    basicHtmlFile.close();

    basicHtml = basicHtml.arg(html).arg("微软雅黑");
    basicHtml = this->pEmotionApi->TextHandler(basicHtml);
    friendsPage = basicHtml;
    this->leftWebView->setHtml(basicHtml);
}

void RenrenWidget::setSameFriends()
{
    QString html;

    for(uint i = 0; i < vSameFriend.size(); i+=3)
    {
        html+="<table width=\"100%\"><tr><td width=\"33%\">"
                "<div style=\"float:left;overflow:hidden;\">"
                "<img src=" + vSameFriend.at(i).tinyurl + " width=\"50\"  height=\"50\" onclick=\"getProfileInfo('"+ vSameFriend.at(i).id +"')\"></img>"
                "<div><small>"+ vSameFriend.at(i).name + "</small></div>"
                "</div>"
                "</td>";
        if(i+2<=vSameFriend.size())
            html+="<td width=\"33%\">"
                    "<div style=\"float:left;overflow:hidden;\">"
                    "<img src=" + pGetFriendsApi->vFriends.at(i+1).tinyurl + " width=\"50\"  height=\"50\" onclick=\"getProfileInfo('"+ vSameFriend.at(i+1).id +"')\"></img>"
                    "<div>"+ pGetFriendsApi->vFriends.at(i+1).name + "</div>"
                    "</div>"
                    "</td>";
        if(i+3<=vSameFriend.size())
            html+="<td width=\"33%\">"
                    "<div style=\"float:left;overflow:hidden;\">"
                    "<img src=" + vSameFriend.at(i+2).tinyurl + " width=\"50\"  height=\"50\" onclick=\"getProfileInfo('"+ vSameFriend.at(i+2).id +"')\"></img>"
                    "<div>"+ vSameFriend.at(i+2).name + "</div>"
                    "</div>"
                    "</td></tr></table>";
    }
    QString basicHtml;
    QFile basicHtmlFile(":/other/res/renren/RenrenHomePage2.html");
    if(!basicHtmlFile.open(QFile::ReadOnly))
    {
        qDebug("open Error!!!");
        qDebug(basicHtmlFile.errorString().toAscii());
    }
    QTextStream basic(&basicHtmlFile);
    basicHtml = basic.readAll();
    basicHtmlFile.close();

    basicHtml = basicHtml.arg(html).arg("微软雅黑");
    basicHtml = this->pEmotionApi->TextHandler(basicHtml);
    this->rightWebView->setHtml(basicHtml);
}

void RenrenWidget::setStatusPage(QString uid)
{
    QDateTime currentDT = QDateTime::currentDateTime();

    QFile feedHtmlFile(":/other/res/renren/Feed2.html");
    if (!feedHtmlFile.open(QFile::ReadOnly))
    {
        qDebug(feedHtmlFile.errorString().toLocal8Bit());
    }
    QTextStream stream(&feedHtmlFile);
    QString everyFeedHtml = stream.readAll();
    feedHtmlFile.close();

    QString tmp;

    vector<StatusInfo> vStatus = this->pStatusApi->GetAllStatusFromId(uid);
    UserInfo userInfo = this->pUserApi->getUserInfo(uid);

    QString html;
    for(uint i = 0; i < vStatus.size(); i++)
    {
        StatusInfo statusInfo = vStatus.at(i);
        QString feedHtml = everyFeedHtml;
        feedHtml = feedHtml.arg(userInfo.headurl).arg(statusInfo.statusId).arg(uid);

        html += "<a>"+statusInfo.message+"</a>";

        feedHtml = feedHtml.arg(userInfo.name + ": " + html);
        html.clear();

        html += "<a class=\"btn blue\" onclick=createStatusWidget("+statusInfo.statusId+","+uid +","+"\"\""+",\"\""+",2)>回复</a>";
        html += "<a class=\"btn blue\" onclick=createStatusWidget("+statusInfo.statusId+","+uid +","+"\"\""+",\"\""+",1)>转发</a>";
        html += "<a class=\"btn blue\" onclick=\"forwardToOtherClient('"+pEmotionApi->RemoveEmotion(statusInfo.message)+"')\">plus</a>";

        feedHtml = feedHtml.arg(html);
        html.clear();

        QString publish;
        QString num;
        QDate currentDate = currentDT.date();
        QTime currentTime = currentDT.time();
        if(currentDate.year()==statusInfo.time.left(4).toInt())
            if(currentDate.month()==statusInfo.time.mid(5,2).toInt())
                if(currentDate.day()==statusInfo.time.mid(8,2).toInt())
                    if(currentTime.hour()==statusInfo.time.mid(11,2).toInt())
                        if(currentTime.minute()==statusInfo.time.mid(14,2).toInt())
                        {
                            publish = "发布于"+num.setNum(currentTime.second()-statusInfo.time.mid(17,2).toInt())+"秒前";
                        }
                        else
                        {
                            publish = "发布于"+num.setNum(currentTime.minute()-statusInfo.time.mid(14,2).toInt())+"分钟前";
                        }
                    else
                    {
                        publish = "发布于"+num.setNum(currentTime.hour()-statusInfo.time.mid(11,2).toInt())+"小时前";
                    }
                else
                {
                    publish = "发布于"+num.setNum(currentDate.day()-statusInfo.time.mid(8,2).toInt())+"天前";
                }
            else
            {
                publish = "发布于"+num.setNum(currentDate.month()-statusInfo.time.mid(5,2).toInt())+"月前";
            }
        else
            publish = statusInfo.time;

        feedHtml = feedHtml.arg(publish+" 评论数："+statusInfo.comment_count);
        feedHtml = feedHtml.arg(uid);
        tmp.append(feedHtml);
    }

    QString basicHtml;
    QFile basicHtmlFile(":/other/res/renren/RenrenHomePage2.html");
    if(!basicHtmlFile.open(QFile::ReadOnly))
    {
        qDebug("open Error!!!");
        qDebug(basicHtmlFile.errorString().toAscii());
    }
    QTextStream basic(&basicHtmlFile);
    basicHtml = basic.readAll();
    basicHtmlFile.close();

    basicHtml = basicHtml.arg(tmp).arg("微软雅黑");
    basicHtml = this->pEmotionApi->TextHandler(basicHtml);
    this->rightWebView->setHtml(basicHtml);
}

void RenrenWidget::setAlbumsPage(QString uid)
{

}

void RenrenWidget::setBlogsPage(QString uid)
{
    QDateTime currentDT = QDateTime::currentDateTime();

    QFile feedHtmlFile(":/other/res/renren/Feed3.html");
    if (!feedHtmlFile.open(QFile::ReadOnly))
    {
        qDebug(feedHtmlFile.errorString().toLocal8Bit());
    }
    QTextStream stream(&feedHtmlFile);
    QString everyFeedHtml = stream.readAll();
    feedHtmlFile.close();

    QString tmp;

    vector<BlogInfo> vBlogs = this->pBlogApi->GetAllBlogsFromUid(uid);
    UserInfo userInfo = this->pUserApi->getUserInfo(uid);

    QString html;
    for(uint i = 0; i < vBlogs.size(); i++)
    {
        BlogInfo blogInfo = vBlogs.at(i);
        QString feedHtml = everyFeedHtml;
        feedHtml = feedHtml.arg(userInfo.headurl).arg(blogInfo.id).arg(uid);

        html += "<a>"+blogInfo.content+"</a>";

        feedHtml = feedHtml.arg(userInfo.name + " " + html);
        html.clear();

        html += "<a class=\"btn blue\" onclick=createStatusWidget("+blogInfo.id+","+uid +","+"\"\""+",\"\""+",2)>回复</a>";
        html += "<a class=\"btn blue\" onclick=createStatusWidget("+blogInfo.id+","+uid +","+"\"\""+",\"\""+",1)>转发</a>";
        html += "<a class=\"btn blue\" onclick=\"forwardToOtherClient('"+pEmotionApi->RemoveEmotion(blogInfo.content)+"')\">plus</a>";

        feedHtml = feedHtml.arg(html);
        html.clear();

        QString publish;
        QString num;
        QDate currentDate = currentDT.date();
        QTime currentTime = currentDT.time();
        if(currentDate.year()==blogInfo.time.left(4).toInt())
            if(currentDate.month()==blogInfo.time.mid(5,2).toInt())
                if(currentDate.day()==blogInfo.time.mid(8,2).toInt())
                    if(currentTime.hour()==blogInfo.time.mid(11,2).toInt())
                        if(currentTime.minute()==blogInfo.time.mid(14,2).toInt())
                        {
                            publish = "发布于"+num.setNum(currentTime.second()-blogInfo.time.mid(17,2).toInt())+"秒前";
                        }
                        else
                        {
                            publish = "发布于"+num.setNum(currentTime.minute()-blogInfo.time.mid(14,2).toInt())+"分钟前";
                        }
                    else
                    {
                        publish = "发布于"+num.setNum(currentTime.hour()-blogInfo.time.mid(11,2).toInt())+"小时前";
                    }
                else
                {
                    publish = "发布于"+num.setNum(currentDate.day()-blogInfo.time.mid(8,2).toInt())+"天前";
                }
            else
            {
                publish = "发布于"+num.setNum(currentDate.month()-blogInfo.time.mid(5,2).toInt())+"月前";
            }
        else
            publish = blogInfo.time;

        feedHtml = feedHtml.arg(publish+" 评论数："+blogInfo.comment_count);
        feedHtml = feedHtml.arg(uid);
        tmp.append(feedHtml);
    }

    QString basicHtml;
    QFile basicHtmlFile(":/other/res/renren/RenrenHomePage2.html");
    if(!basicHtmlFile.open(QFile::ReadOnly))
    {
        qDebug("open Error!!!");
        qDebug(basicHtmlFile.errorString().toAscii());
    }
    QTextStream basic(&basicHtmlFile);
    basicHtml = basic.readAll();
    basicHtmlFile.close();

    basicHtml = basicHtml.arg(tmp).arg("微软雅黑");
    basicHtml = this->pEmotionApi->TextHandler(basicHtml);
    this->rightWebView->setHtml(basicHtml);
}

void RenrenWidget::receiveSignalFromMainWidget(int index)
{
    if (index <= 0)
        return;
    if (1 <= this->currentSelected && this->currentSelected < 3 && index != 3)
    {
        changeSelectedPixmap(this->currentSelected,0);
    }
    switch(index)
    {
    case 1:
        changeSelectedPixmap(1,1);
        this->getFeed();
        this->currentSelected = 1;
        break;
    case 2:
        changeSelectedPixmap(2,1);
        this->SetFriendPage();
        this->currentSelected = 2;
        break;
    case 3:
        createStatusWidgetSlot("","","","",0);
        break;

    }
}

void RenrenWidget::ReadFeed(QString post_id)
{
    vector <FeedInfo> ::iterator it;
    for(it = pFeedApi->vFeedInfo.begin(); it != pFeedApi->vFeedInfo.end(); )
    {
        if( it->post_id == post_id)
        {
            int itype = it->feed_type.toInt();
            switch(itype)
            {
            case 10:
                this->ReadStatusFeed(it->source_id,it->actor_id);
                break;
            case 20:
                this->ReadBlogFeed(it->source_id,it->actor_id);
                break;
            case 21:
                this->ReadBlogShareFeed(it->vfeed_media.at(0).media_id,it->vfeed_media.at(0).owner_id,it->actor_id);
                break;
            case 30:
                this->ReadPhotoUploadFeed(it->vfeed_media);
                break;
            case 32:
                this->ReadPhotoShareFeed(it->vfeed_media);
                break;
            case 33:
                this->ReadPhotoAlbumShareFeed(it->vfeed_media.at(0).media_id,it->vfeed_media.at(0).owner_id);
                break;
            }
            break;
        }

        else it++;
    }
    for(it = pFeedApi->vUserFeed.begin(); it != pFeedApi->vUserFeed.end(); )
    {
        if( it->post_id == post_id)
        {
            int itype = it->feed_type.toInt();
            switch(itype)
            {
            case 10:
                this->ReadStatusFeed(it->source_id,it->actor_id);
                break;
            case 20:
                this->ReadBlogFeed(it->source_id,it->actor_id);
                break;
            case 21:
                this->ReadBlogShareFeed(it->vfeed_media.at(0).media_id,it->vfeed_media.at(0).owner_id,it->actor_id);
                break;
            case 30:
                this->ReadPhotoUploadFeed(it->vfeed_media);
                break;
            case 32:
                this->ReadPhotoShareFeed(it->vfeed_media);
                break;
            case 33:
                this->ReadPhotoAlbumShareFeed(it->vfeed_media.at(0).media_id,it->vfeed_media.at(0).owner_id);
                break;
            }
            break;
        }

        else it++;
    }
}

void RenrenWidget::setProfilePage(QString uid)
{
    QDateTime currentDT = QDateTime::currentDateTime();
    pFeedApi->getFeed(1, uid);

    QFile profileHtmlFile(":/other/res/renren/Profile.html");
    if (!profileHtmlFile.open(QFile::ReadOnly))
    {
        qDebug(profileHtmlFile.errorString().toLocal8Bit());
    }
    QTextStream stream(&profileHtmlFile);
    QString profileHtml = stream.readAll();
    profileHtmlFile.close();


    UserInfo userInfo = this->pUserApi->getUserInfo(uid);
    this->pUserApi->getProfileInfo(uid);

    profileHtml = profileHtml.arg(userInfo.mainurl).arg(userInfo.name);
    if(userInfo.sex.toInt() == 1)
        profileHtml = profileHtml.arg("male.png");
    else
        profileHtml = profileHtml.arg("female.png");
    profileHtml = profileHtml.arg("生日："+userInfo.birthday);
    profileHtml = profileHtml.arg("学校："+userInfo.univInfo.name+" "+userInfo.univInfo.department);
    profileHtml = profileHtml.arg("家乡："+userInfo.hometown.province+" "+userInfo.hometown.city);
    profileHtml = profileHtml.arg("状态："+QString::number(this->pUserApi->profileInfo.status_count)).arg("相册："+QString::number(this->pUserApi->profileInfo.albums_count)).arg("日志："+QString::number(this->pUserApi->profileInfo.blogs_count));
    QFile feedHtmlFile(":/other/res/renren/Feed.html");
    if (!feedHtmlFile.open(QFile::ReadOnly))
    {
        qDebug(feedHtmlFile.errorString().toLocal8Bit());
    }
    QTextStream stream2(&feedHtmlFile);
    QString everyFeedHtml = stream2.readAll();
    feedHtmlFile.close();

    QString tmp;
    QString html;

    for(int i=0; i<pFeedApi->vUserFeed.size(); i++)
    {
        FeedInfo feed = pFeedApi->vUserFeed.at(i);
        QString feedHtml = everyFeedHtml;
        feedHtml = feedHtml.arg(feed.headurl).arg(feed.post_id);

        if(feed.feed_type == "10" && feed.prefix.contains("转自", Qt::CaseInsensitive) || feed.feed_type == "20" || feed.feed_type == "21")
            html += feed.prefix+" "
                    "<div class=\"arrow\">"
                    "<div class=\"arrow_border\"></div>"
                    "<div class=\"arrow_content\"></div>"
                    "</div>"
                    "<div class=\"main\">"
                            "<b class=\"b1\"></b>"
                            "<b class=\"b2\"></b>"
                            "<b class=\"b3\"></b>"
                            "<b class=\"b4\"></b>"
                            "<div class=\"content\" style=\"font-size:smaller;\">"
                            "<a>"+feed.message.left(80)+"</a>"
                            "<a>"+feed.title +"</a>"
                            "<a>"+feed.description.left(80)+"...</a>"
                            "</div>"
                            "<b class=\"b5\"></b>"
                            "<b class=\"b6\"></b>"
                            "<b class=\"b7\"></b>"
                            "<b class=\"b8\"></b>"
                    "</div>";
        else
            if(feed.feed_type == "10")
                html += "<a>"+feed.message+"</a>"
                        "<a>"+feed.title +"</a>"
                        "<a>"+feed.description+"</a>";
            else
                html += feed.prefix+" "
                        "<a>"+feed.message+"</a>"
                        "<a>"+feed.title+"</a>"
                        "<a>"+feed.description+"</a>";

        uint min = (feed.vfeed_media.size() > 2) ? 2 : feed.vfeed_media.size();

        for(uint j = 0; j < min; j++)
        {
            if(j==0)
            {
                if(feed.feed_type == "50")
                {
                    html += "<div onclick=\"playVedio('"+feed.vfeed_media.at(j).href+"')\" style=\"width:120px;height:90px;background-repeat:no-repeat;background-position:center;background-image: url("+ feed.vfeed_media.at(j).src +"); \">"
                            "<img src=\"http://img.t.sinajs.cn/t35/style/images/common/feedvideoplay.gif\" style=\"cursor: pointer; display: block;float:none; margin:auto; padding-top:30px; opacity: 0.5;\" ></img>"
                            "</div>";
                }
                else
                    html += "<div><img src=" + feed.vfeed_media.at(j).src + "></image></div>";
            }
            else
                html += "<div><img src=" + feed.vfeed_media.at(j).src + "></image></div>";
        }

        feedHtml = feedHtml.arg(feed.name + " " + html);
        html.clear();
        QString tmpmenu;
        QString menu;
        if(feed.feed_type == "10")
        {

            tmpmenu = "<li><a  onclick=createStatusWidget("+feed.source_id+","+feed.actor_id +","+"\"\""+",\"\""+",2)>回复</a></li>";
            tmpmenu += "<li><a  onclick=createStatusWidget("+feed.source_id+","+feed.actor_id +","+"\"\""+",\"\""+",1)>转发</a></li>";
            tmpmenu += "<li><a  onclick=\"forwardToOtherClient('"+pEmotionApi->RemoveEmotion(feed.message)+"')\">plus</a></li>";
            menu =tmpmenu;
            html += "<ul class=\"menu\" onclick=dropmenu(this,'"+menu.toAscii().toBase64()+"')><a>+</a></ul>";
            //html+="<div onclick=alert('123123')>menu</div>";
        }
        if(feed.feed_type == "20")
        {
            tmpmenu = "<li><a  onclick=createStatusWidget("+feed.source_id+","+feed.actor_id +","+"\"\""+",\"\""+",3)>评论</a></li>";
            tmpmenu += "<li><a  onclick=createStatusWidget(1,"+feed.source_id+","+feed.actor_id +","+"\"\""+",6)>分享</a></li>";
            menu =tmpmenu;
            html += "<ul class=\"menu\" onclick=dropmenu(this,'"+menu.toAscii().toBase64()+"')><a>+</a></ul>";
        }
        if(feed.feed_type == "21")
        {
            tmpmenu = "<li><a  onclick=createStatusWidget("+feed.source_id+","+feed.actor_id +","+"\"\""+",\"\""+",4)>评论</a></li>";
            tmpmenu += "<li><a  onclick=createStatusWidget(20,"+feed.source_id+","+feed.actor_id +","+"\"\""+",6)>分享</a></li>";
            menu =tmpmenu;
            html += "<ul class=\"menu\" onclick=dropmenu(this,'"+menu.toAscii().toBase64()+"')><a>+</a></ul>";
        }
        if(feed.feed_type == "30")
        {
            tmpmenu = "<li><a  onclick=createStatusWidget(\"\"," +feed.vfeed_media.at(0).media_id+","+feed.vfeed_media.at(0).owner_id +","+"\"\""+",5)>评论</a></li>";
            tmpmenu += "<li><a  onclick=createStatusWidget(2,"+feed.vfeed_media.at(0).media_id+","+feed.vfeed_media.at(0).owner_id +","+"\"\""+",6)>分享</a></li>";
            menu = tmpmenu;
            html += "<ul class=\"menu\" onclick=dropmenu(this,'"+menu.toAscii().toBase64()+"')><a>+</a></ul>";
        }
        if(feed.feed_type == "32")
        {
            tmpmenu = "<li><a  onclick=createStatusWidget("+feed.source_id+","+feed.actor_id +","+"\"\""+",\"\""+",4)>评论</a></li>";
            tmpmenu += "<li><a  onclick=createStatusWidget(20,"+feed.source_id+","+feed.actor_id +","+"\"\""+",6)>分享</a></li>";
            menu = tmpmenu;
            html += "<ul class=\"menu\" onclick=dropmenu(this,'"+menu.toAscii().toBase64()+"')><a>+</a></ul>";
        }
        if(feed.feed_type == "33")
        {
            tmpmenu = "<li><a  onclick=createStatusWidget("+feed.source_id+","+feed.actor_id +","+"\"\""+",\"\""+",4)>评论</a></li>";
            tmpmenu += "<li><a  onclick=createStatusWidget(20,"+feed.source_id+","+feed.actor_id +","+"\"\""+",6)>分享</a></li>";
            menu = tmpmenu;
            html += "<ul class=\"menu\" onclick=dropmenu(this,'"+menu.toAscii().toBase64()+"')><a>+</a></ul>";
        }
        if(feed.feed_type == "50")
        {
            tmpmenu = "<li><a  onclick=createStatusWidget("+feed.source_id+","+feed.actor_id +","+"\"\""+",\"\""+",4)>评论</a></li>";
            tmpmenu += "<li><a  onclick=createStatusWidget(20,"+feed.source_id+","+feed.actor_id +","+"\"\""+",6)>分享</a></li>";
            menu = tmpmenu;
            html += "<ul class=\"menu\" onclick=dropmenu(this,'"+menu.toAscii().toBase64()+"')><a>+</a></ul>";
        }

        feedHtml = feedHtml.arg(html);
        html.clear();

        QString publish;
        QString num;
        QDate currentDate = currentDT.date();
        QTime currentTime = currentDT.time();
        if(currentDate.year()==feed.update_time.left(4).toInt())
            if(currentDate.month()==feed.update_time.mid(5,2).toInt())
                if(currentDate.day()==feed.update_time.mid(8,2).toInt())
                    if(currentTime.hour()==feed.update_time.mid(11,2).toInt())
                        if(currentTime.minute()==feed.update_time.mid(14,2).toInt())
                        {
                            publish = "发布于"+num.setNum(currentTime.second()-feed.update_time.mid(17,2).toInt())+"秒前";
                        }
                        else
                        {
                            publish = "发布于"+num.setNum(currentTime.minute()-feed.update_time.mid(14,2).toInt())+"分钟前";
                        }
                    else
                    {
                        publish = "发布于"+num.setNum(currentTime.hour()-feed.update_time.mid(11,2).toInt())+"小时前";
                    }
                else
                {
                    publish = "发布于"+num.setNum(currentDate.day()-feed.update_time.mid(8,2).toInt())+"天前";
                }
            else
            {
                publish = "发布于"+num.setNum(currentDate.month()-feed.update_time.mid(5,2).toInt())+"月前";
            }
        else
            publish = feed.update_time;

    feedHtml = feedHtml.arg(publish+" 评论数："+feed.count);
    feedHtml = feedHtml.arg(feed.actor_id);
    tmp.append(feedHtml);
    }

    profileHtml = profileHtml.arg(tmp);

    vSameFriend = this->pGetFriendsApi->getSameFriends(uid);
    html.clear();
    html+="<table width=\"100%\"><td>共同好友："+QString::number(vSameFriend.size())+"人</td> <td onclick=\"getSameFriends()\" width=\"20%\">更多...</td></table>";
    for(uint k = 0; k < (vSameFriend.size()<6?vSameFriend.size():6) ; k++)
    {
        html+="<div style=\"width:50px;padding:5px;float:left;\">"
                "<img src="+vSameFriend.at(k).tinyurl+" width=\"50\"  height=\"50\" onclick=\"getProfileInfo('"+vSameFriend.at(k).id+"')\"></img><small>"
                + vSameFriend.at(k).name + "</small></div>";
    }

    profileHtml = profileHtml.arg(html);
    profileHtml = profileHtml.arg(uid).arg(uid).arg(uid);

    QString basicHtml;
    QFile basicHtmlFile(":/other/res/renren/RenrenHomePage2.html");
    if(!basicHtmlFile.open(QFile::ReadOnly))
    {
        qDebug("open Error!!!");
        qDebug(basicHtmlFile.errorString().toAscii());
    }
    QTextStream basic(&basicHtmlFile);
    basicHtml = basic.readAll();
    basicHtmlFile.close();

    basicHtml = basicHtml.arg(profileHtml).arg("微软雅黑");
    basicHtml = this->pEmotionApi->TextHandler(basicHtml);

    //qDebug(profileHtml.toLocal8Bit());
    this->rightWebView->setHtml(basicHtml);
}

void RenrenWidget::SetNewPage(int page)
{
    QDateTime currentDT = QDateTime::currentDateTime();
    pFeedApi->getFeed(page);

    QFile feedHtmlFile(":/other/res/renren/Feed.html");
    if (!feedHtmlFile.open(QFile::ReadOnly))
    {
        qDebug(feedHtmlFile.errorString().toLocal8Bit());
    }
    QTextStream stream(&feedHtmlFile);
    QString everyFeedHtml = stream.readAll();
    feedHtmlFile.close();

    QString tmp;
    QString html;

    for(int i=30*(page-1); i<pFeedApi->vFeedInfo.size(); i++)
    {
        FeedInfo feed = pFeedApi->vFeedInfo.at(i);
        QString feedHtml = everyFeedHtml;
        feedHtml = feedHtml.arg(feed.headurl).arg(feed.post_id);

        if(feed.feed_type == "10" && feed.prefix.contains("转自", Qt::CaseInsensitive) || feed.feed_type == "20" || feed.feed_type == "21")
            html += feed.prefix+" "
                    "<div class=\"arrow\">"
                    "<div class=\"arrow_border\"></div>"
                    "<div class=\"arrow_content\"></div>"
                    "</div>"
                    "<div class=\"main\">"
                            "<b class=\"b1\"></b>"
                            "<b class=\"b2\"></b>"
                            "<b class=\"b3\"></b>"
                            "<b class=\"b4\"></b>"
                            "<div class=\"content\" style=\"font-size:smaller;\">"
                            "<a>"+feed.message.left(80)+"</a>"
                            "<a>"+feed.title +"</a>"
                            "<a>"+feed.description.left(80)+"...</a>"
                            "</div>"
                            "<b class=\"b5\"></b>"
                            "<b class=\"b6\"></b>"
                            "<b class=\"b7\"></b>"
                            "<b class=\"b8\"></b>"
                    "</div>";
        else
            if(feed.feed_type == "10")
                html += "<a>"+feed.message+"</a>"
                        "<a>"+feed.title +"</a>"
                        "<a>"+feed.description+"</a>";
            else
                html += feed.prefix+" "
                        "<a>"+feed.message+"</a>"
                        "<a>"+feed.title+"</a>"
                        "<a>"+feed.description+"</a>";

        uint min = (feed.vfeed_media.size() > 2) ? 2 : feed.vfeed_media.size();

        for(uint j = 0; j < min; j++)
        {
            if(j==0)
            {
                if(feed.feed_type == "50")
                {
                    html += "<div onclick=\"playVedio('"+feed.vfeed_media.at(j).href+"')\" style=\"width:120px;height:90px;background-repeat:no-repeat;background-position:center;background-image: url("+ feed.vfeed_media.at(j).src +"); \">"
                            "<img src=\"http://img.t.sinajs.cn/t35/style/images/common/feedvideoplay.gif\" style=\"cursor: pointer; display: block;float:none; margin:auto; padding-top:30px; opacity: 0.5;\" ></img>"
                            "</div>";
                }
                else
                    html += "<div><img  src=" + feed.vfeed_media.at(j).src + "></image></div>";
            }
            else
                html += "<div><img  src=" + feed.vfeed_media.at(j).src + "></image></div>";
        }

        feedHtml = feedHtml.arg(feed.name + " " + html);
        html.clear();
        QString tmpmenu;
        QString menu;
        if(feed.feed_type == "10")
        {

            tmpmenu = "<li><a  onclick=createStatusWidget("+feed.source_id+","+feed.actor_id +","+"\"\""+",\"\""+",2)>回复</a></li>";
            tmpmenu += "<li><a  onclick=createStatusWidget("+feed.source_id+","+feed.actor_id +","+"\"\""+",\"\""+",1)>转发</a></li>";
            tmpmenu += "<li><a  onclick=\"forwardToOtherClient('"+pEmotionApi->RemoveEmotion(feed.message)+"')\">plus</a></li>";
            menu =tmpmenu;
            html += "<ul class=\"menu\" onclick=dropmenu(this,'"+menu.toAscii().toBase64()+"')><a>+</a></ul>";
            //html+="<div onclick=alert('123123')>menu</div>";
        }
        if(feed.feed_type == "20")
        {
            tmpmenu = "<li><a  onclick=createStatusWidget("+feed.source_id+","+feed.actor_id +","+"\"\""+",\"\""+",3)>评论</a></li>";
            tmpmenu += "<li><a  onclick=createStatusWidget(1,"+feed.source_id+","+feed.actor_id +","+"\"\""+",6)>分享</a></li>";
            menu =tmpmenu;
            html += "<ul class=\"menu\" onclick=dropmenu(this,'"+menu.toAscii().toBase64()+"')><a>+</a></ul>";
        }
        if(feed.feed_type == "21")
        {
            tmpmenu = "<li><a  onclick=createStatusWidget("+feed.source_id+","+feed.actor_id +","+"\"\""+",\"\""+",4)>评论</a></li>";
            tmpmenu += "<li><a  onclick=createStatusWidget(20,"+feed.source_id+","+feed.actor_id +","+"\"\""+",6)>分享</a></li>";
            menu =tmpmenu;
            html += "<ul class=\"menu\" onclick=dropmenu(this,'"+menu.toAscii().toBase64()+"')><a>+</a></ul>";
        }
        if(feed.feed_type == "30")
        {
            tmpmenu = "<li><a  onclick=createStatusWidget(\"\"," +feed.vfeed_media.at(0).media_id+","+feed.vfeed_media.at(0).owner_id +","+"\"\""+",5)>评论</a></li>";
            tmpmenu += "<li><a  onclick=createStatusWidget(2,"+feed.vfeed_media.at(0).media_id+","+feed.vfeed_media.at(0).owner_id +","+"\"\""+",6)>分享</a></li>";
            menu = tmpmenu;
            html += "<ul class=\"menu\" onclick=dropmenu(this,'"+menu.toAscii().toBase64()+"')><a>+</a></ul>";
        }
        if(feed.feed_type == "32")
        {
            tmpmenu = "<li><a  onclick=createStatusWidget("+feed.source_id+","+feed.actor_id +","+"\"\""+",\"\""+",4)>评论</a></li>";
            tmpmenu += "<li><a  onclick=createStatusWidget(20,"+feed.source_id+","+feed.actor_id +","+"\"\""+",6)>分享</a></li>";
            menu = tmpmenu;
            html += "<ul class=\"menu\" onclick=dropmenu(this,'"+menu.toAscii().toBase64()+"')><a>+</a></ul>";
        }
        if(feed.feed_type == "33")
        {
            tmpmenu = "<li><a  onclick=createStatusWidget("+feed.source_id+","+feed.actor_id +","+"\"\""+",\"\""+",4)>评论</a></li>";
            tmpmenu += "<li><a  onclick=createStatusWidget(20,"+feed.source_id+","+feed.actor_id +","+"\"\""+",6)>分享</a></li>";
            menu = tmpmenu;
            html += "<ul class=\"menu\" onclick=dropmenu(this,'"+menu.toAscii().toBase64()+"')><a>+</a></ul>";
        }
        if(feed.feed_type == "50")
        {
            tmpmenu = "<li><a  onclick=createStatusWidget("+feed.source_id+","+feed.actor_id +","+"\"\""+",\"\""+",4)>评论</a></li>";
            tmpmenu += "<li><a  onclick=createStatusWidget(20,"+feed.source_id+","+feed.actor_id +","+"\"\""+",6)>分享</a></li>";
            menu = tmpmenu;
            html += "<ul class=\"menu\" onclick=dropmenu(this,'"+menu.toAscii().toBase64()+"')><a>+</a></ul>";
        }

        feedHtml = feedHtml.arg(html);
        html.clear();

        QString publish;
        QString num;
        QDate currentDate = currentDT.date();
        QTime currentTime = currentDT.time();
        if(currentDate.year()==feed.update_time.left(4).toInt())
            if(currentDate.month()==feed.update_time.mid(5,2).toInt())
                if(currentDate.day()==feed.update_time.mid(8,2).toInt())
                    if(currentTime.hour()==feed.update_time.mid(11,2).toInt())
                        if(currentTime.minute()==feed.update_time.mid(14,2).toInt())
                        {
                            publish = "发布于"+num.setNum(currentTime.second()-feed.update_time.mid(17,2).toInt())+"秒前";
                        }
                        else
                        {
                            publish = "发布于"+num.setNum(currentTime.minute()-feed.update_time.mid(14,2).toInt())+"分钟前";
                        }
                    else
                    {
                        publish = "发布于"+num.setNum(currentTime.hour()-feed.update_time.mid(11,2).toInt())+"小时前";
                    }
                else
                {
                    publish = "发布于"+num.setNum(currentDate.day()-feed.update_time.mid(8,2).toInt())+"天前";
                }
            else
            {
                publish = "发布于"+num.setNum(currentDate.month()-feed.update_time.mid(5,2).toInt())+"月前";
            }
        else
            publish = feed.update_time;

    feedHtml = feedHtml.arg(publish+" 评论数："+feed.count);
    feedHtml = feedHtml.arg(feed.actor_id);
    tmp.append(feedHtml);
    }
    this->leftWebView->page()->mainFrame()->evaluateJavaScript("setNew('"+tmp.toAscii().toBase64()+"')");
}

void RenrenWidget::ReadStatusFeed(QString status_id, QString owner_id)
{
    StatusInfo statusinfo = this->pStatusApi->GetSingleStatus(status_id ,owner_id);
    vector<CommentInfo> vComments = this->pStatusApi->GetCommentsFromStatusId(status_id,owner_id);
    UserInfo userinfo =this->pUserApi->getUserInfo(owner_id);
    QString html;
    html +="<table style=\"width:100%;\">";
    html += "<tr>";
    html += "<td  valign=\"top\" style=\"width:100px;padding:5px;\" onclick=\"getProfileInfo('"+ owner_id +"')\"><center><img src=" + userinfo.headurl + " minwidth=\"100\"  maxheight=\"100\" /></center></td>";
    html += "<td valign=\"top\" style=\"padding:5px; width:350px;\"><div>"+ userinfo.name + "</div>"
            "<div>"+ statusinfo.message + "</div>"
               "</td>";
    html += "<td><a class=\"btn blue\"  onclick=createStatusWidget("+status_id+","+owner_id+","+"\"\""+",\"\""+",2)>回复</a></td>";
    html +="</tr></table>";
    for(uint i = 0; i < vComments.size(); i++)
    {
        html +="<table style=\"width:100%;\">";
        html += "<tr>";
        html += "<td  valign=\"top\" style=\"width:50px;padding:5px;\" onclick=\"getProfileInfo('"+ vComments.at(i).uid +"')\"><img src=" + vComments.at(i).tinyurl + " width=\"50\"  height=\"50\"></img></td>";
        html += "<td valign=\"top\" style=\"padding:5px; width:400px;\"><div>"+ vComments.at(i).name + "</div>"
                "<div>"+ vComments.at(i).text + "</div>"
                   "</td>";
        html += "<td><a class=\"btn blue\"  onclick=createStatusWidget("+status_id+","+owner_id+","+vComments.at(i).uid+",\"\""+",2)>回复</a></td>";
        html +="</tr></table>";
    }
    //qDebug(html.toAscii());

    QString basicHtml;
    QFile basicHtmlFile(":/other/res/renren/RenrenHomePage2.html");
    if(!basicHtmlFile.open(QFile::ReadOnly))
    {
        qDebug("open Error!!!");
        qDebug(basicHtmlFile.errorString().toAscii());
    }
    QTextStream basic(&basicHtmlFile);
    basicHtml = basic.readAll();
    basicHtmlFile.close();

    basicHtml = basicHtml.arg(html).arg("微软雅黑");
    basicHtml = this->pEmotionApi->TextHandler(basicHtml);

    this->rightWebView->setHtml(basicHtml);
}



void RenrenWidget::setCookie(QNetworkRequest &request)
{
    QMap<QString,QString> param;
    param.insert("expires",this->pFeedApi->account.getExpires());
    param.insert("session_key",this->pFeedApi->account.getSessionKey());
    param.insert("ss",this->pFeedApi->account.getSessionSecret());
    param.insert("user",this->pFeedApi->account.getUid());

    QString sig = Utility::getSignature(param);

    QString header = Config::APIKey + "_expires=" +this->pFeedApi->account.getExpires()+
                               Config::APIKey + "_session_key=" +this->pFeedApi->account.getSessionKey()+
                               Config::APIKey + "_ss=" +this->pFeedApi->account.getSessionSecret()+
                               Config::APIKey + "_user=" +this->pFeedApi->account.getUid()+
                               Config::APIKey +"="+sig;
    request.setRawHeader("cookie",header.toAscii());
}

void RenrenWidget::createStatusWidgetSlot(QString arg_1, QString arg_2,QString arg_3,QString arg_4,int widgetType)
{
    if (pStatusWidget == NULL)
    {
        pStatusWidget = new RenrenStatusWidget(this->prenrenAccount,arg_1,arg_2,widgetType,this);
        QObject::connect(pStatusWidget,SIGNAL(sendStatus(QString,QString,QString,QString,QString,int)),
                         this,SLOT(StatusSlot(QString,QString,QString,QString,QString,int)));
    }

    if (widgetType == 1)
    {
        pStatusWidget->setForward_id(arg_1);
        pStatusWidget->setForward_owner(arg_2);
    }
    else if (widgetType ==2)
    {
        pStatusWidget->setStatus_id(arg_1);
        pStatusWidget->setOwner_id(arg_2);
        pStatusWidget->setReply_id(arg_3);
    }
    else if (widgetType == 3)
    {
        pStatusWidget->setLog_id(arg_1);
        pStatusWidget->setOwner_id(arg_2);
        pStatusWidget->setReply_id(arg_3);
    }

    else if (widgetType == 4)
    {
        pStatusWidget->setShare_id(arg_1);
        pStatusWidget->setOwner_id(arg_2);
        pStatusWidget->setToUser_id(arg_3);
    }
    else if (widgetType == 5)
    {
        pStatusWidget->setAlbum_id(arg_1);
        pStatusWidget->setPhoto_id(arg_2);
        pStatusWidget->setOwner_id(arg_3);
        pStatusWidget->setReply_id(arg_4);
    }
    else if (widgetType == 6)
    {
        pStatusWidget->setShare_Type(arg_1);
        pStatusWidget->setUgc_id(arg_2);
        pStatusWidget->setOwner_id(arg_3);
        pStatusWidget->setShare_Url(arg_4);
    }

    pStatusWidget->setWidgetType(widgetType);
    pStatusWidget->clearContent();
    pStatusWidget->show();

}

void RenrenWidget::StatusSlot(QString content,QString arg_1, QString arg_2, QString arg_3,QString arg_4,int widgetType)
{
    qDebug(content.toLocal8Bit());
    qDebug(arg_1.toLocal8Bit());
    qDebug(arg_2.toLocal8Bit());
    qDebug(arg_3.toLocal8Bit());
    qDebug(arg_4.toLocal8Bit());
    qDebug(QString::number(widgetType).toLocal8Bit());
    if (widgetType == 0) //新状态
    {
        this->pStatusApi->StatusUpdate(content,"","");
    }
    else if (widgetType == 1)//转发状态
    {
        this->pStatusApi->StatusUpdate(content,arg_1,arg_2);
    }
    else if (widgetType == 2)//评论状态及状态回复
    {
        this->pStatusApi->StatusAddComment(arg_1,arg_2,content,arg_3);
    }
    else if (widgetType == 3)//评论日志
    {
        this->pBlogApi->AddComment(content,arg_1,arg_2,arg_3);
    }
    else if (widgetType == 4)//评论分享
    {
        this->pShareApi->AddComment(content,arg_1,arg_2,arg_3);
    }
    else if (widgetType == 5)//评论照片
    {
        this->pPhotoApi->AddComment(content,arg_1,arg_2,arg_3,arg_4);

    }
    else if (widgetType == 6)
    {
        this->pShareApi->Share(content,arg_1,arg_2,arg_3,arg_4);
    }

}

void RenrenWidget::ReadBlogFeed(QString id, QString uid)
{
    BlogInfo bloginfo = this->pBlogApi->GetBlogFromId(id,uid);
    UserInfo userinfo =this->pUserApi->getUserInfo(uid);
    vector<BlogComment> vBlogComment = this->pBlogApi->GetComment(id,uid);
    QString html;

    html+="<div style=\"float:left;width:100%;overflow:hidden;\">"
            "<div style=\"float:left;\" onclick=\"getProfileInfo('"+ uid +"')\"><img src=" + userinfo.tinyurl + " width=\"50\"  height=\"50\"></img></div>"
            "<div><b>"+ bloginfo.title +"</b></div>"
               "</div><br />"+ bloginfo.content ;

    for(uint i = 0; i < vBlogComment.size(); i++)
    {
        html +="<table style=\"width:100%;\">";
        html += "<tr>";
        html += "<td  valign=\"top\" style=\"width:50px;padding:5px;\" onclick=\"getProfileInfo('"+ vBlogComment.at(i).uid +"')\"><img src=" + vBlogComment.at(i).headurl + " width=\"50\"  height=\"50\"></img></td>";
        html += "<td valign=\"top\" style=\"padding:5px; width:400px;\"><div>"+ vBlogComment.at(i).name + "</div>"
                "<div>"+ vBlogComment.at(i).content + "</div>"
                   "</td>";
        html +="</tr></table>";
    }

    QString basicHtml;
    QFile basicHtmlFile(":/other/res/renren/RenrenHomePage2.html");
    if(!basicHtmlFile.open(QFile::ReadOnly))
    {
        qDebug("open Error!!!");
        qDebug(basicHtmlFile.errorString().toAscii());
    }
    QTextStream basic(&basicHtmlFile);
    basicHtml = basic.readAll();
    basicHtmlFile.close();

    basicHtml = basicHtml.arg(html).arg("微软雅黑");
    basicHtml = this->pEmotionApi->TextHandler(basicHtml);
    this->rightWebView->setHtml(basicHtml);
}

void RenrenWidget::ReadBlogShareFeed(QString id ,QString uid,QString actor_id)
{
    BlogInfo bloginfo = this->pBlogApi->GetBlogFromId(id,uid);
    UserInfo userinfo =this->pUserApi->getUserInfo(actor_id);
    vector<BlogComment> vBlogComment = this->pBlogApi->GetComment(id,uid);
    QString html;

    html+="<div style=\"float:left;width:100%;overflow:hidden;\">"
            "<div style=\"float:left;\" onclick=\"getProfileInfo('"+ actor_id +"')\"><img src=" + userinfo.tinyurl + " width=\"50\"  height=\"50\"></img></div>"
            "<div>"+ userinfo.name + "</div>"
            "<div><b>"+ bloginfo.title + "</b></div>"
               "</div><br />"+ bloginfo.content ;

    for(uint i = 0; i < vBlogComment.size(); i++)
    {
        html +="<table style=\"width:100%;\">";
        html += "<tr>";
        html += "<td  valign=\"top\" style=\"width:50px;padding:5px;\" onclick=\"getProfileInfo('"+ vBlogComment.at(i).uid +"')\"><img src=" + vBlogComment.at(i).headurl + " width=\"50\"  height=\"50\"></img></td>";
        html += "<td valign=\"top\" style=\"padding:5px; width:400px;\"><div>"+ vBlogComment.at(i).name + "</div>"
                "<div>"+ vBlogComment.at(i).content + "</div>"
                   "</td>";
        html +="</tr></table>";
    }
    html = this->pEmotionApi->TextHandler(html);

    QString basicHtml;
    QFile basicHtmlFile(":/other/res/renren/RenrenHomePage2.html");
    if(!basicHtmlFile.open(QFile::ReadOnly))
    {
        qDebug("open Error!!!");
        qDebug(basicHtmlFile.errorString().toAscii());
    }
    QTextStream basic(&basicHtmlFile);
    basicHtml = basic.readAll();
    basicHtmlFile.close();

    basicHtml = basicHtml.arg(html).arg("微软雅黑");
    basicHtml = this->pEmotionApi->TextHandler(basicHtml);
    this->rightWebView->setHtml(basicHtml);
}

void RenrenWidget::ReadPhotoUploadFeed(vector<Feed_media> vfeed_media)
{
    QString html;
    QString id = vfeed_media.at(0).media_id;
    QString uid = vfeed_media.at(0).owner_id;
    UserInfo userinfo =this->pUserApi->getUserInfo(uid);
    vector<PhotoInfo> vPhotoinfo = this->pPhotoApi->GetPhotosFromId(id,uid);
    html +="<div ><img src=" + userinfo.tinyurl + " width=\"50\" height=\"50\" onclick=\"getProfileInfo('"+ uid +"')\"></img>"+ userinfo.name + ":"+vPhotoinfo.at(0).caption+"</div>""<img class=\"pic\" src=" + vPhotoinfo.at(0).url_large + "></img></div>";

    QString basicHtml;
    QFile basicHtmlFile(":/other/res/renren/RenrenHomePage2.html");
    if(!basicHtmlFile.open(QFile::ReadOnly))
    {
        qDebug("open Error!!!");
        qDebug(basicHtmlFile.errorString().toAscii());
    }
    QTextStream basic(&basicHtmlFile);
    basicHtml = basic.readAll();
    basicHtmlFile.close();

    basicHtml = basicHtml.arg(html).arg("微软雅黑");
    basicHtml = this->pEmotionApi->TextHandler(basicHtml);
    this->rightWebView->setHtml(basicHtml);
}

void RenrenWidget::ReadPhotoShareFeed(vector<Feed_media> vfeed_media)
{
    QString id = vfeed_media.at(0).media_id;
    QString uid = vfeed_media.at(0).owner_id;
    UserInfo userinfo =this->pUserApi->getUserInfo(uid);
    vector<PhotoInfo> vPhotoinfo = this->pPhotoApi->GetPhotosFromId(id,uid);
    QString html ="<div><img src=" + userinfo.tinyurl + " width=\"50\"  height=\"50\" onclick=\"getProfileInfo('"+ uid +"')\"></img>"+ userinfo.name + ":"+vPhotoinfo.at(0).caption+"</div>""<img class=\"pic\" src=" + vPhotoinfo.at(0).url_large + "></img></div>";

    QString basicHtml;
    QFile basicHtmlFile(":/other/res/renren/RenrenHomePage2.html");
    if(!basicHtmlFile.open(QFile::ReadOnly))
    {
        qDebug("open Error!!!");
        qDebug(basicHtmlFile.errorString().toAscii());
    }
    QTextStream basic(&basicHtmlFile);
    basicHtml = basic.readAll();
    basicHtmlFile.close();

    basicHtml = basicHtml.arg(html).arg("微软雅黑");
    basicHtml = this->pEmotionApi->TextHandler(basicHtml);
    this->rightWebView->setHtml(basicHtml);
}

void RenrenWidget::ReadPhotoAlbumShareFeed(QString aid,QString uid)
{
    vector<PhotoInfo> vPhotoinfo = this->pPhotoApi->GetPhotoAlbumFromId(aid,uid);
    UserInfo userinfo =this->pUserApi->getUserInfo(uid);
    QString html;
    html +="<div><img src=" + userinfo.tinyurl + " width=\"50\" height=\"50\" onclick=\"getProfileInfo('"+ uid +"')\"></img>"+ userinfo.name + ":"+vPhotoinfo.at(0).caption+"</div>""<img class=\"pic\" src=" + vPhotoinfo.at(0).url_large + "></img></div>";

    QString basicHtml;
    QFile basicHtmlFile(":/other/res/renren/RenrenHomePage2.html");
    if(!basicHtmlFile.open(QFile::ReadOnly))
    {
        qDebug("open Error!!!");
        qDebug(basicHtmlFile.errorString().toAscii());
    }
    QTextStream basic(&basicHtmlFile);
    basicHtml = basic.readAll();
    basicHtmlFile.close();

    basicHtml = basicHtml.arg(html).arg("微软雅黑");
    basicHtml = this->pEmotionApi->TextHandler(basicHtml);
    this->rightWebView->setHtml(basicHtml);
}

QPixmap RenrenWidget::getPixmap()
{
    return httpGet.getPixMapFromUrl(QUrl(""));
}

void RenrenWidget::forward_from_otherClient(QString str)
{
    if (str.length() > 135)
    {
        str = str.left(135);
    }

    if (pStatusWidget == NULL)
    {
        pStatusWidget = new RenrenStatusWidget(this->prenrenAccount,this);
        QObject::connect(pStatusWidget,SIGNAL(sendStatus(QString,QString,QString,QString,QString,int)),
                         this,SLOT(StatusSlot(QString,QString,QString,QString,QString,int)));
    }
    this->pStatusWidget->setWidgetType(0);
    this->pStatusWidget->clearContent();
    this->pStatusWidget->setContent(str);
    this->pStatusWidget->show();
}

void RenrenWidget::forwardToOtherClientSlot(QString str)
{
    qDebug("in Renrenwidget forwardToOtherClientSlot");
    emit forwardToOtherClientSignal(str);
}

void RenrenWidget::changeSelectedPixmap(int index, int type)
{
    //QString styleSheet="QWidget#%1%2%3{background-image:url(:/images/renren/%4.png)}";

    if (type == 0)//remove
    {
        //styleSheet = styleSheet.arg("label_").arg("left_").arg(QString::number(index));
        switch (index)
        {
        case 1:
            //styleSheet = styleSheet.arg("newThings");
            ui->label_left_1->setPixmap(newThings);
            break;
        case 2:
            //styleSheet = styleSheet.arg("myFriends");
            ui->label_left_2->setPixmap(myFriends);
            break;
        case 3:
            //styleSheet = styleSheet.arg("sendStatus");
            //ui->label_left_3->setPixmap(sendStatus);
            break;
        default:
            qDebug("styleSheet Error!!!");
        }

    }
    else if (type == 1)//add
    {
        //styleSheet = styleSheet.arg("label_").arg("left_").arg(QString::number(index));
        switch (index)
        {
        case 1:
            //styleSheet = styleSheet.arg("newThingsLighted");
            ui->label_left_1->setPixmap(newThingsLighted);
            break;
        case 2:
            //styleSheet = styleSheet.arg("myFriendsLighted");
            ui->label_left_2->setPixmap(myFriendsLighted);
            break;
        case 3:
            //styleSheet = styleSheet.arg("sendStatusLighted");
            //ui->label_left_3->setPixmap(sendStatusLighted);
            break;
        default:
            qDebug("styleSheet Error!!!");
    }
    }
    else{
        qDebug("changeSelectedPixmap type error!!!");
    }

}

void RenrenWidget::PlayVideo(QString str)
{
    HttpGet http;
    http.get(QUrl(str));
    QString finalUrl = http.getUrl();
    QRegExp rx("id_(.+).html");
    QString video_id;
    int pos = rx.indexIn(finalUrl);
    if (pos > -1)
    {
          video_id = rx.cap(1);
    }
    QString url="http://player.youku.com/player.php/sid/"+video_id+"/v.swf";
    rightWebView->load(QUrl(url));

}
