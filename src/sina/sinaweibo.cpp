

#include "sinaweibo.h"
#include "http.h"
#include "sinaparserxml.h"
#include "sinaservices.h"
#include <QtNetwork>
#include <QMessageBox>
#include <QByteArray>


/*! \brief SinaWeibo类的构造函数
 *  \param [in] account 指向Account类的指针
 *  
 */
SinaWeibo::SinaWeibo(Account *account)
{
    this->account = account;
    /* Basic Auth*/
    QString concatenated = account->getUsername() + ":" +account->getPassword() ;
    QByteArray data = concatenated.toLocal8Bit().toBase64();
    this->basicAuthInfo = "Basic " + data;

    QFile statusHtmlFile(":/other/res/sina/Status.html");
    if (!statusHtmlFile.open(QFile::ReadOnly))
    {
        qDebug(statusHtmlFile.errorString().toLocal8Bit());
    }
    QTextStream stream(&statusHtmlFile);
    everyStatusHtml = stream.readAll();
    statusHtmlFile.close();

    QFile retwStatusHtmlFile(":/other/res/sina/RetwStatus.html");
    if (!retwStatusHtmlFile.open(QFile::ReadOnly))
    {
        qDebug(retwStatusHtmlFile.errorString().toLocal8Bit());
    }
    QTextStream retwStream(&retwStatusHtmlFile);
    retwStatusHtml = retwStream.readAll();
    retwStatusHtmlFile.close();

    servicesInstance = SinaServices::getInstance();
    conf = Configuration::getInstance();
    this->contentFrame = "<div style=\"font-family:微软雅黑;\">  %1 </div>";
}

SinaWeibo::~SinaWeibo()
{
    //qDebug("sinaweibo deleting");
}

/*! \brief 验证当前用户身份是否合法
 * 
 *  API: http://open.t.sina.com.cn/wiki/index.php/Account/verify_credentials
 *  \return 成功返回true并初始化account中相关内容，反之返回false
 */
bool SinaWeibo::verifyCredentials()
{
    QUrl url;
    url.setUrl(servicesInstance->getServices("VerifyCredentials"));
    QNetworkRequest request;
    request.setUrl(url);

    QMap<QString,QString> params;
    this->setOAuthAuthorize( request, OAuth::GET, servicesInstance->getServices("VerifyCredentials"),params );

    Http *http = new Http(this);
    QString content = http->get(request);
    if(http->hasNetworkError())
    {
        switch(http->getNetworkErrorCode())
        {
        case QNetworkReply::HostNotFoundError: /* Host not found error */
            QMessageBox::information(NULL, tr("网络错误"), tr("请确定已连接网络!"),\
                                     QMessageBox::Yes, QMessageBox::Yes);
            break;
        case QNetworkReply::TimeoutError: /* Timeout error */
            QMessageBox::information(NULL, tr("网络错误"), tr("网络超时!"),\
                                     QMessageBox::Yes, QMessageBox::Yes);
            break;
        case QNetworkReply::AuthenticationRequiredError: /* Login error */
            QMessageBox::information(NULL, tr("网络错误"), tr("用户名或密码错误!"),\
                                     QMessageBox::Yes, QMessageBox::Yes);
            break;
        }
        return false;
    }
    delete http;
    SinaParserXml xml;
    return xml.parserSinaAccountInformation(account, content);
}

/*! \brief 获取当前用户的收藏列表
 * 
 *  新浪API地址: http://open.t.sina.com.cn/wiki/index.php/Favorites
 *  \param [in] page 页码，每页默认20条无法更改
 *  \return 成功返回xml格式的收藏列表，反之返回QString::null
 */
QString SinaWeibo::getFavorites(int page)
{
    QMap<QString,QString> params;
    QString urlString(servicesInstance->getServices("Favorites"));
    QUrl url;
    url.setUrl(urlString);
    QNetworkRequest request;
    if(page>1)
    {
        url.addQueryItem("page", QString::number(page));
        params.insert("page", QString::number(page));
    }
    request.setUrl(url);
    this->setOAuthAuthorize( request, OAuth::GET, servicesInstance->getServices("Favorites"), params);
    Http *http = new Http(this);
    QString content = http->get(request);
    if(http->hasNetworkError())
    {
        return QString::null;
    }
    delete http;
    return content;
}

/*! \brief 根据微博消息ID返回某条微博消息的评论列表
 *
 * API: http://open.t.sina.com.cn/wiki/index.php/Statuses/comments
 * \param [in] id 要获取的评论列表所属的微博消息ID
 * \param [in] page 评论列表的页码
 * \param [in] count 每页评论条数
 * \return 成功返回xml格式的评论列表,反之返回QString::null
 */
QString SinaWeibo::getCommentsList(QString id, int page, int count)
{
    QMap<QString,QString> params;
    QString urlString(servicesInstance->getServices("Comments"));
    QUrl url(urlString);
    if(!id.isEmpty())
    {
        params.insert("id", id);
        url.addQueryItem("id", id);
    }
    if(page>1)
    {
        params.insert("page", QString::number(page));
        url.addQueryItem("page", QString::number(page));
    }
    if(count>0&&count!=20)
    {
        params.insert("count", QString::number(count));
        url.addQueryItem("count", QString::number(count));
    }
    params.insert("source",servicesInstance->getServices("Source"));
    url.addQueryItem("source",servicesInstance->getServices("Source"));

    QNetworkRequest request(url);
    this->setOAuthAuthorize( request, OAuth::GET, servicesInstance->getServices("Comments"), params);
    Http *http = new Http(this);
    QString xmlContent = http->get(request);
    if(http->hasNetworkError())
    {
        qDebug("error code is %d", http->getNetworkErrorCode());
        return QString::null;
    }
    delete http;
    return xmlContent;
}

QString SinaWeibo::favoritePageStatusToHtml(QList<Status> &statusList,
                                            QMap<QString, Status> &retwStatusMap)
{
    setVedioPic(statusList);
    setVedioPic(retwStatusMap);
    QString tmp;

    if(statusList.isEmpty())
        return tr("没有更多收藏了!");

    for(int i=0; i<statusList.length(); i++)
    {
        Status status = statusList.at(i);
        QString statusHtml = everyStatusHtml;

        statusHtml.replace("statusIdInHtml",status.getId());

        QString imgHtml = getImgAndVedioHtml(status);

        statusHtml = statusHtml.arg(status.getUserProfileImageUrl()).arg(status.getUserName()).arg(statusTextFormat(status.getText().replace("%","5Zu+5qC35Zu+5qOu56C0")) + imgHtml);

        statusHtml.replace("userid",status.getUserId());

        if (!status.getRetweetedId().isEmpty())
        {
            Status retwStatus = retwStatusMap.value(status.getRetweetedId());
            QString retwHtml = retwStatusHtml;

            retwHtml.replace("userid",retwStatus.getUserId());
            retwHtml.replace("statusid",retwStatus.getId());

            QString imgHtml = getImgAndVedioHtml(retwStatus);

            QString  reText="<a href=javascript:void(0) onclick=showUserInfo(\""+retwStatus.getUserId()+"\",1);event.cancelBubble = true;>"
                    +"@"
                    +retwStatus.getUserName()+":  </a>";

            statusHtml = statusHtml.arg(retwHtml.arg(reText
                                                                        +statusTextFormat(retwStatus.getText())
                                                                        +imgHtml));

        }
        else
            statusHtml = statusHtml.arg("");
        QString strbutton;
        QString tmpmenu;
        QString menu;
        tmpmenu += "<li><a onclick=retw(\""+status.getId()+"\",\""+"转发微博。"+"\""+ ")>转发</a></li>";
        tmpmenu += "<li><a onclick=comment("+status.getId()+")>评论</a></li>";
        tmpmenu += "<li><a onclick=deleteFavorite("+status.getId()+")>删除</a></li>";
        tmpmenu += "<li><a onclick=\"forwardToOtherClient('"+statusTextFormat(status.getText())+"')\">plus</a></li>";
        menu = tmpmenu;
        strbutton += "<ul class=\"menu\" onclick=dropmenu(this,'"+menu.toAscii().toBase64()+"')><a>+</a></ul>";
        statusHtml = statusHtml.arg(strbutton);
        tmp.append(statusHtml);
    }


    return contentFrame.arg(tmp.replace("5Zu+5qC35Zu+5qOu56C0","%"));
}

/*! \brief 获得最新At到自己的微博消息
 *
 * API详情: http://open.t.sina.com.cn/wiki/index.php/Statuses/mentions
 *  \return 成功返回提到我的微博列表(xml形式), 反之返回QString::null
 */
QString SinaWeibo::getMentions(qint64 sinceId, qint64 maxId, int count, int page)
{
    QMap<QString,QString> params;
    QUrl url;
    QString urlString( servicesInstance->getServices("Mentions"));
    url.setUrl(urlString);
    if( sinceId>0 )
    {
        params.insert("since_id", QString::number(sinceId));
        url.addQueryItem("since_id", QString::number(sinceId));
    }
    if( maxId>0 )
    {
        params.insert("max_id", QString::number(maxId));
        url.addQueryItem("max_id", QString::number(maxId));
    }
    if( count>0 && count!=20 )
    {
        params.insert("count", QString::number(count));
        url.addQueryItem("count", QString::number(count));
    }
    if( page>1 )
    {
        params.insert("page", QString::number(page));
        url.addQueryItem("page", QString::number(page));
    }
    
    QNetworkRequest request;
    request.setUrl(url);
    this->setOAuthAuthorize( request, OAuth::GET, servicesInstance->getServices("Mentions"), params);
    Http *http = new Http(this);
    QString content = http->get(request);
    if(http->hasNetworkError())
    {
        return QString::null;
    }
    delete http;
    return content;//basicHtml.arg(atMePageStatusToHtml(account->atMePageStatus, account->atMePageRetweeted)); 
}

QString SinaWeibo::atMePageStatusToHtml(QList<Status> &statusList,
                                        QMap<QString, Status> &retwStatusMap)
{
    setVedioPic(statusList);
    setVedioPic(retwStatusMap);

    QString tmp;

    if(statusList.isEmpty())
        return tr("没有更多微博了!");

    for(int i=0; i<statusList.length(); i++)
    {
        Status status = statusList.at(i);
        QString statusHtml = everyStatusHtml;
        QString imgHtml = getImgAndVedioHtml(status);

        statusHtml = statusHtml.arg(status.getUserProfileImageUrl()).arg(status.getUserName()).arg(statusTextFormat(status.getText().replace("%","5Zu+5qC35Zu+5qOu56C0")) + imgHtml);

        statusHtml.replace("userid",status.getUserId());

        QString downStr = "<div style=\"rgb(87,121,167)\"><small>%1</small></div>";
        downStr = downStr.arg(status.getPlainCreatedTime());

        if (!status.getRetweetedId().isEmpty())
        {
            Status retwStatus = retwStatusMap.value(status.getRetweetedId());
            QString retwHtml =  retwStatusHtml;

            retwHtml.replace("userid",retwStatus.getUserId());
            retwHtml.replace("statusid",retwStatus.getId());

            QString imgHtml = getImgAndVedioHtml(retwStatus);

            QString  reText="<a href=javascript:void(0) onclick=showUserInfo(\""+retwStatus.getUserId()+"\",1);event.cancelBubble = true;>"
                    +"@"
                    +retwStatus.getUserName()+":  </a>";

            statusHtml = statusHtml.arg(retwHtml.arg(reText
                                                                        +statusTextFormat(retwStatus.getText())
                                                                        +imgHtml) + downStr);

        }
        else
            statusHtml = statusHtml.arg(downStr);
        QString strbutton;
        QString tmpmenu;
        QString menu;
        tmpmenu += "<li><a onclick=retw(\""+status.getId()+"\",\""+"转发微博。"+"\""+ ")>转发</a></li>";
        tmpmenu += "<li><a onclick=comment("+status.getId()+")>评论</a></li>";
        tmpmenu += "<li><a onclick=addFavorite("+status.getId()+")>收藏</a></li>";
        menu = tmpmenu;
        strbutton += "<ul class=\"menu\" onclick=dropmenu(this,'"+menu.toAscii().toBase64()+"')><a>+</a></ul>";
        statusHtml = statusHtml.arg(strbutton);
        tmp.append(statusHtml);
    }


    //qDebug(tmp.toAscii());
    return contentFrame.arg(tmp.replace("5Zu+5qC35Zu+5qOu56C0","%"));
}

/*! \brief 根据id获取用户发布的微博消息列表
 *
 *API详情: http://open.t.sina.com.cn/wiki/index.php/Statuses/user_timeline
 *  
 * \return 成功则返回微博消息列表(xml格式)，反之返回QString::null
 */
QString SinaWeibo::getUserTimeline(QString userId, int count, int page, int baseApp, int feature)
{
    QUrl url;
    QString urlString, servicesString;
    QMap<QString,QString> params;
    if(userId == "0"||this->account->getId() == userId)
    {
        servicesString = servicesInstance->getServices("MyTimeline");
        urlString.append(servicesString);
    }
    else
    {

        servicesString = servicesInstance->getServices("OthersTimeline").arg(userId);
        urlString.append(servicesString);
    }
    url.setUrl(urlString);  
    if(count>0&&count!=20)
    {
        params.insert("count", QString::number(count));
        url.addQueryItem("count", QString::number(count));
    }
    if(page>1)
    {
        params.insert("page", QString::number(page));
        url.addQueryItem("page", QString::number(page));
    }
    if(baseApp==1)
    {
        params.insert("base_app", QString::number(baseApp));
        url.addQueryItem("base_app", QString::number(baseApp));
    }
    if(feature>0&&feature<5)
    {
        params.insert("feature", QString::number(feature));
        url.addQueryItem("feature", QString::number(feature));
    }
    QNetworkRequest request;
    request.setUrl(url);
    this->setOAuthAuthorize( request, OAuth::GET, servicesString, params);
    Http *http = new Http(this);
    QString content = http->get(request);
    if(http->hasNetworkError())
    {
        return QString::null;
    }
    delete http;
    return content;
}

/*! \brief 根据用户昵称来获取发布的微博消息列表
 *
 * API详情: http://open.t.sina.com.cn/wiki/index.php/Statuses/user_timeline
 *  \return 成功则返回微博消息列表(xml格式)，反之返回QString::null
 */
QString SinaWeibo::getUserTimelineByScreenName(QString screenName, int count, int page, int baseApp, int feature)
{
    QUrl url;
    QString urlString;
    QMap<QString, QString> params;
    urlString.append(servicesInstance->getServices("MyTimeline"));
    url.setUrl(urlString);
    
    params.insert("screen_name", screenName.toAscii().toPercentEncoding());
    url.addQueryItem("screen_name", screenName.toUtf8());
    
    if(count>0&&count!=20)
    {
        params.insert("count", QString::number(count));
        url.addQueryItem("count", QString::number(count));
    }
    if(page>1)
    {
        params.insert("page", QString::number(page));
        url.addQueryItem("page", QString::number(page));
    }
    if(baseApp==1)
    {
        params.insert("base_app", QString::number(baseApp));
        url.addQueryItem("base_app", QString::number(baseApp));
    }
    if(feature>0&&feature<5)
    {
        params.insert("feature", QString::number(feature));
        url.addQueryItem("feature", QString::number(feature));
    }
    QNetworkRequest request;
    request.setUrl(url);
    this->setOAuthAuthorize( request, OAuth::GET, servicesInstance->getServices("MyTimeline"), params);
    Http *http = new Http(this);
    QString content = http->get(request);
    if(http->hasNetworkError())
    {
        qDebug("%d", http->getNetworkErrorCode());
        return NULL;
    }
    delete http;

    return content;
}

QString SinaWeibo::userWeiboPageStatusToHtml(QList<Status> &statusList,
                                             QMap<QString, Status> &retwStatusMap)
{

    setVedioPic(statusList);
    setVedioPic(retwStatusMap);

    QString tmp;

    if(statusList.isEmpty())
        return tr("没有更多微博了!");

    for(int i=0; i<statusList.length(); i++)
    {
        Status status = statusList.at(i);
        QString statusHtml = everyStatusHtml;
        QString imgHtml = getImgAndVedioHtml(status);

        statusHtml = statusHtml.arg(status.getUserProfileImageUrl()).arg(status.getUserName()).arg(statusTextFormat(status.getText().replace("%","5Zu+5qC35Zu+5qOu56C0")) + imgHtml);

        statusHtml.replace("userid",status.getUserId());
        statusHtml.replace("statusid",status.getId());

        QString downStr = "<div style=\"rgb(87,121,167)\"><small>%1</small></div>";
        downStr = downStr.arg(status.getPlainCreatedTime());

        if (!status.getRetweetedId().isEmpty())
        {
            Status retwStatus = retwStatusMap.value(status.getRetweetedId());
            QString retwHtml =  retwStatusHtml;

            retwHtml.replace("userid",retwStatus.getUserId());
            retwHtml.replace("statusid",retwStatus.getId());

            QString imgHtml= getImgAndVedioHtml(retwStatus);

            QString  reText="<a href=javascript:void(0) onclick=showUserInfo(\""+retwStatus.getUserId()+"\",1);event.cancelBubble = true;>"
                    +"@"
                    +retwStatus.getUserName()+":  </a>";

            statusHtml = statusHtml.arg(retwHtml.arg(reText
                                                     +statusTextFormat(retwStatus.getText())
                                                     +imgHtml)
                                        +downStr);

        }
        else
            statusHtml = statusHtml.arg(downStr);

        QString zfwb = "转发微博。";
        QString strbutton;
        QString tmpmenu;
        QString menu;
        if(!status.getRetweetedId().isEmpty())
        {
            QString aText = "<a href=javascript:void(0) onclick=showUserInfo(\""+status.getUserId()+"\",1);event.cancelBubble = true;>//@"+status.getUserName()+":"+statusTextFormat(status.getText())+"</a>";
            Status retwStatus = retwStatusMap.value(status.getRetweetedId());
            tmpmenu += "<li><a onclick=retw(\""+status.getId()+"\",\""+aText.toAscii().toBase64()+"\""+ ")>转发</a></li>";
            tmpmenu += "<li><a onclick=comment("+status.getId()+")>评论</a></li>";
            tmpmenu += "<li><a onclick=addFavorite("+status.getId()+")>收藏</a></li>";
            tmpmenu += "<li><a onclick=retw(\""+retwStatus.getId()+"\",\""+zfwb.toAscii().toBase64()+"\""+ ")>原文转发</a></li>";
            tmpmenu += "<li><a onclick=\"forwardToOtherClient('"+statusTextFormat(status.getText())+"')\">plus</a></li>";
            menu = tmpmenu;
            strbutton += "<ul class=\"menu\" onclick=dropmenu(this,'"+menu.toAscii().toBase64()+"')><a>+</a></ul>";
        }
        else
        {
            tmpmenu += "<li><a onclick=retw(\""+status.getId()+"\",\""+zfwb.toAscii().toBase64()+"\""+ ")>转发</a></li>";
            tmpmenu += "<li><a onclick=comment("+status.getId()+")>评论</a></li>";
            tmpmenu += "<li><a onclick=addFavorite("+status.getId()+")>收藏</a></li>";
            tmpmenu += "<li><a onclick=\"forwardToOtherClient('"+statusTextFormat(status.getText())+"')\">plus</a></li>";
            menu = tmpmenu;
            strbutton += "<ul class=\"menu\" onclick=dropmenu(this,'"+menu.toAscii().toBase64()+"')><a>+</a></ul>";
        }
        statusHtml = statusHtml.arg(strbutton);
        tmp.append(statusHtml);
    }


    return contentFrame.arg(tmp.replace("5Zu+5qC35Zu+5qOu56C0","%"));
}

/*! \brief 获取当前登录用户及其所关注用户的最新微博消息
 *
 * API详情: http://open.t.sina.com.cn/wiki/index.php/Statuses/friends_timeline
 * \return 成功返回微博列表(xml),反之返回QString::null
 */
QString SinaWeibo::getFriendsTimeline( qint64 sinceId, qint64 maxId, int count, int page, int baseApp, int feature)
{
    QUrl url;
    QMap<QString, QString> params;
    QString urlString(servicesInstance->getServices("FriendsTimeline"));
    url.setUrl(urlString);
    if(sinceId>0)
    {
        params.insert("since_id", QString::number(sinceId));
        url.addQueryItem("since_id", QString::number(sinceId));
    }
    if(maxId>0)
    {
        params.insert("max_id", QString::number(maxId));
        url.addQueryItem("max_id", QString::number(maxId));
    }
    if(count!=20)
        if(count>0)
        {
            params.insert("count", QString::number(count));
            url.addQueryItem("count", QString::number(count));
        }
    if(page>1)
    {
        params.insert("page", QString::number(page));
        url.addQueryItem("page", QString::number(page));
    }
    if(baseApp==1)
    {
        params.insert("base_app", QString::number(baseApp));
        url.addQueryItem("base_app", QString::number(baseApp));
    }
    if(feature>0&&feature<5)
    {
        params.insert("feature", QString::number(baseApp));
        url.addQueryItem("feature", QString::number(baseApp));
    }
    
    QNetworkRequest request;
    request.setUrl(url);
    this->setOAuthAuthorize( request, OAuth::GET, servicesInstance->getServices("FriendsTimeline"), params);
    Http *http = new Http(this);
    QString content = http->get(request);
    if(http->hasNetworkError())
    {
        return QString::null;
    }
    delete http;
    return content; 
}

QString SinaWeibo::homePageStatusToHtml(QList<Status> &statusList,
                                        QMap<QString, Status> &retwStatusMap)
{
    setCommentsAndRetweetNum(statusList);
    setVedioPic(statusList);
    setVedioPic(retwStatusMap);


    QString tmp;

    if(statusList.isEmpty())
        return tr("没有更多微博了!");

    for(int i=0; i < statusList.length(); i++)
    {
        Status status = statusList.at(i);
        QString statusHtml = everyStatusHtml;

        QString imgHtml = getImgAndVedioHtml(status);


        statusHtml = statusHtml.arg(status.getUserProfileImageUrl()).arg(status.getUserName()).arg(statusTextFormat(status.getText().replace("%","5Zu+5qC35Zu+5qOu56C0")) + imgHtml);
        statusHtml.replace("userid",status.getUserId());
        statusHtml.replace("statusid",status.getId());

        QString downStr = "<div style=\"color:rgb(87,121,167)\"><small>%1&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;转发:%2  评论:%3</small></div>";
        downStr = downStr.arg(status.getPlainCreatedTime())
                        .arg(status.getRetweetedNum())
                        .arg(status.getCommentsNum());

        if (!status.getRetweetedId().isEmpty())
        {
            Status retwStatus = retwStatusMap.value(status.getRetweetedId());
            QString retwHtml =  retwStatusHtml;

            retwHtml.replace("userid",retwStatus.getUserId());
            retwHtml.replace("statusid",retwStatus.getId());

            QString imgHtml = getImgAndVedioHtml(retwStatus);

            QString  reText="<a href=javascript:void(0) onclick=showUserInfo(\""+retwStatus.getUserId()+"\",1);event.cancelBubble = true;>"
                    +"@"
                    +retwStatus.getUserName()+":  </a>";

            statusHtml = statusHtml.arg(retwHtml.arg(reText
                                                     +statusTextFormat(retwStatus.getText())
                                                     +imgHtml)+downStr);
        }
        else
            statusHtml = statusHtml.arg(downStr);

        QString zfwb = "转发微博。";
        QString strbutton;
        QString tmpmenu;
        if(!status.getRetweetedId().isEmpty())
        {
            QString menu;
            QString aText = "<a href=javascript:void(0) onclick=showUserInfo(\""+status.getUserId()+"\",1);event.cancelBubble = true;>//@"+status.getUserName()+":"+status.getText() + "</a>";
            Status retwStatus = retwStatusMap.value(status.getRetweetedId());
            tmpmenu += "<li><a  onclick=retw(\""+status.getId()+"\",\""+aText.toAscii().toBase64()+"\""+ ")>转发</a></li>";
            tmpmenu += "<li><a  onclick=comment("+status.getId()+")>评论</a></li>";
            tmpmenu += "<li><a  onclick=addFavorite("+status.getId()+")>收藏</a></li>";
            tmpmenu += "<li><a  onclick=retw(\""+retwStatus.getId()+"\",\""+zfwb.toAscii().toBase64()+"\""+ ")>原文转发</a></li>";
            tmpmenu += "<li><a  onclick=\"forwardToOtherClient('"+statusTextFormat(status.getText())+"')\">plus</a></li>";
            menu = tmpmenu;
            strbutton += "<ul class=\"menu\" onclick=dropmenu(this,'"+menu.toAscii().toBase64()+"')><a>+</a></ul>";
        }
        else
        {
            QString menu;
            tmpmenu += "<li><a  onclick=retw(\""+status.getId()+"\",\""+zfwb.toAscii().toBase64()+"\""+ ")>转发</a></li>";
            tmpmenu += "<li><a  onclick=comment("+status.getId()+")>评论</a></li>";
            tmpmenu += "<li><a  onclick=addFavorite("+status.getId()+")>收藏</a></li>";
            tmpmenu += "<li><a  onclick=\"forwardToOtherClient('"+statusTextFormat(status.getText())+"')\">plus</a></li>";
            menu = tmpmenu;
            strbutton += "<ul class=\"menu\" onclick=dropmenu(this,'"+menu.toAscii().toBase64()+"')><a>+</a></ul>";
        }
        statusHtml = statusHtml.arg(strbutton);
        tmp.append(statusHtml);
    }

//    qDebug(tmp.replace("5Zu+5qC35Zu+5qOu56C0","%").toLocal8Bit());
    return contentFrame.arg(tmp.replace("5Zu+5qC35Zu+5qOu56C0","%"));
}

/*! \brief 返回最新n条发送及收到的评论
 *
 * API详情: http://open.t.sina.com.cn/wiki/index.php/Statuses/comments_timeline
 * \return 收到的评论列表(xml)，反之返回QString::null
 */
QString SinaWeibo::getCommentTimeline(qint64 sinceId, qint64 maxId, int count, int page)
{
    QUrl url;
    QMap<QString, QString> params;
    QString urlString(servicesInstance->getServices("CommentsTimeline"));
    url.setUrl(urlString);
    if(sinceId>0)
    {
        params.insert("since_id", QString::number(sinceId));
        url.addQueryItem("since_id", QString::number(sinceId));
    }
    if(maxId>0)
    {
        params.insert("max_id", QString::number(maxId));
        url.addQueryItem("max_id", QString::number(maxId));
    }
    if(count!=20)
        if(count>0)
        {
            params.insert("count", QString::number(count));
            url.addQueryItem("count", QString::number(count));
        }
    if(page>1)
    {
        params.insert("page", QString::number(page));
        url.addQueryItem("page", QString::number(page));
    }
    QNetworkRequest request;
    request.setUrl(url);
    this->setOAuthAuthorize( request, OAuth::GET, servicesInstance->getServices("CommentsTimeline"), params);
    Http *http = new Http(this);
    QString content = http->get(request);
    if(http->hasNetworkError())
    {
        return QString::null;
    }
    delete http;
/*
    QFile tmpFile(QCoreApplication::applicationDirPath() + "xmlContent.txt");

    if (!tmpFile.open(QFile::ReadWrite))
    {
        qDebug("tmpFile open Error!!!");
        return content;
    }

    tmpFile.write(content.toLocal8Bit());
*/
     return content; 
}

QString SinaWeibo::commentPageStatusToHtml(QList<Status> &statusList,
                                           QMap<QString, Status> &retwStatusMap)
{
    setVedioPic(statusList);
    setVedioPic(retwStatusMap);
    QString tmp;

    if(statusList.isEmpty())
        return tr("没有更多评论了!");

    for(int i=0; i<statusList.length(); i++)
    {
        Status status = statusList.at(i);
        QString statusHtml = everyStatusHtml;
        QString imgHtml  = getImgAndVedioHtml(status);

        QString downStr = "<div style=\"rgb(87,121,167)\"><small>%1</small></div>";
        downStr = downStr.arg(status.getPlainCreatedTime());

        QString statusText = statusTextFormat(status.getText().replace("%","5Zu+5qC35Zu+5qOu56C0")).replace("a href=javascript:void(0)","a href=javascript:void(0) onclick=showUserInfo('"+retwStatusMap.value(status.getRetweetedId()).getUserId()+"')");
        statusHtml = statusHtml.arg(status.getUserProfileImageUrl()).arg(status.getUserName()).arg(statusText + imgHtml);

        statusHtml.replace("userid",status.getUserId());

//        status.getCreatedAt();

        if (!status.getRetweetedId().isEmpty())
        {
            Status retwStatus = retwStatusMap.value(status.getRetweetedId());
            QString retwHtml =  retwStatusHtml;

            retwHtml.replace("userid",retwStatus.getUserId());
            retwHtml.replace("statusid",retwStatus.getId());

            QString imgHtml = getImgAndVedioHtml(retwStatus);

            QString  reText="<a href=javascript:void(0) onclick=showUserInfo(\""+retwStatus.getUserId()+"\",1);event.cancelBubble = true;>"
                    +"@"
                    +retwStatus.getUserName()+":  </a>";

            QString tmp = statusTextFormat(retwStatus.getText()).replace("<a href=javascript:void(0)>:","<a href=javascript:void(0) onclick=showUserInfo('"+retwStatus.getUserId()+"')>:");

            statusHtml = statusHtml.arg(retwHtml.arg(reText
                                                     +tmp
                                                     +imgHtml)
                                        +downStr);

        }
        else
            statusHtml = statusHtml.arg(downStr);
        statusHtml = statusHtml.arg("<a class=\"btn blue\" onclick=replyComment(\""+status.getId()+"\",\""+status.getRetweetedId()+"\")>回复</a>");//button
        tmp.append(statusHtml);
    }


    //qDebug(tmp.toAscii());
    return contentFrame.arg(tmp.replace("5Zu+5qC35Zu+5qOu56C0","%"));
}

QString SinaWeibo::directMessagePageStatusToHtml(QList<Status> &statusList,DMType dmType)
{
        setVedioPic(statusList);
        QString tmp;

        if(statusList.isEmpty())
            return tr("没有私信哦。");

        for(int i=0; i<statusList.length(); i++)
        {
            Status status = statusList.at(i);
            QString statusHtml = everyStatusHtml;

            statusHtml.replace("userid",status.getUserId());


            QString imgHtml = getImgAndVedioHtml(status);

            if (dmType == SinaWeibo::List)
            {
                statusHtml = statusHtml.arg(status.getUserProfileImageUrl()).arg(status.getUserName()).arg(statusTextFormat(status.getText().replace("%","5Zu+5qC35Zu+5qOu56C0")) + imgHtml).arg("");
                QString strbutton = "<a class=\" btn blue\" onclick=getDMSessionStatus(\"%1\")>查看</a>";
                statusHtml = statusHtml.arg(strbutton.arg(status.getSenderId()));
            }
            else if (dmType == SinaWeibo::Session)
            {
                if (status.getUserId() != this->account->getId())
                {
                    statusHtml = statusHtml.arg(status.getUserProfileImageUrl()).arg(status.getUserName()).arg(statusTextFormat(status.getText().replace("%","5Zu+5qC35Zu+5qOu56C0")) + imgHtml).arg("");
                    QString strbutton = "<a class=\"btn blue\" onclick=sendDirectMessage(\"%1\")>回复</a>";
                    statusHtml = statusHtml.arg(strbutton.arg(status.getUserId()));
                }
                else
                {
                    statusHtml = statusHtml.arg(status.getUserProfileImageUrl()).arg("我").arg(statusTextFormat(status.getText()) + imgHtml).arg("");
                    statusHtml = statusHtml.arg("");
                }
            }
            tmp.append(statusHtml);
        }


        return contentFrame.arg(tmp.replace("5Zu+5qC35Zu+5qOu56C0","%"));

}

QString SinaWeibo::commentsListToHtml(QList<Status> &statusList)
{
    setVedioPic(statusList);

    /*
     1----------userinfo html
     2----------username html
     3----------tool html
     4----------status text
     5----------time html
     */
    QString commentsHtml="<li><div class=minicomments><div class=miniuserinfo>%1</div>"
                         "<div class=miniweibocontent>%2<div class=minitool>%3</div><p>%4</p>%5</div>"
                         "<div style=\"font-family:\"微软雅黑\";font: 0px/0px ;clear: both;display: block\"> </div></div></li>";
    QString tmp;
    for( int i=0; i<statusList.length(); i++)
    {
        Status status = statusList.at(i);
        tmp.append( commentsHtml
                    .arg("<img height=30 width=30 src="+status.getUserProfileImageUrl()+" />")
                    .arg("<a href=javascript:void(0) onclick=getUserTimelineByScreenName(\""+status.getUserName()+"\",1)>"+
                                                                              status.getUserName()+"</a>:")
                    .arg("<a href=javascript:void(0) onclick=replyComment(\""+status.getId()+"\",\""+status.getRetweetedId()
                         +"\")>回复</a>")
                    .arg(statusTextFormat(statusTextFormat(status.getText().replace("%","5Zu+5qC35Zu+5qOu56C0"))))
                    .arg(this->timeFormatHtml(status.getCreatedAt()))
                );
    }
    tmp.prepend("<hr><ul>");
    tmp.append("</ul>");
    //qDebug(tmp.toAscii());
    return tmp.replace("5Zu+5qC35Zu+5qOu56C0","%");
}

QString SinaWeibo::statusToHtmlForNotifier(Status &status)
{
    QString html = "%1:\n%2";
//    if(!status.getBmiddlePic().isEmpty())
//        html.append("<a style=color:red >[图]</a>");
    return html.arg(status.getUserName()).arg(status.getText());
}

QString SinaWeibo::statusToHtmlForNotifier(Status &status, Status &retwStatus)
{
    QString html = "%1:\n%2\n"
                   "%3:\n%4";
//    if(!retwStatus.getBmiddlePic().isEmpty())
//        html.append("<a style=color:red>[图]</a>");
    return html
            .arg(status.getUserName())
            .arg(status.getText())
            .arg(retwStatus.getUserName())
            .arg(retwStatus.getText());
}

/*! \brief 登录用户的最新收到的n条私信
 *
 * API详情: http://open.t.sina.com.cn/wiki/index.php/Direct_messages
 *
 * \return 私信列表(xml),反之返回QString::null
 */
QString SinaWeibo::getDirectMessage(qint64 sinceId, qint64 maxId, int count, int page)
{
    QString urlString(servicesInstance->getServices("DirectMessages"));
    QUrl url(urlString);
    QMap<QString, QString> params;
    if(sinceId>0)
    {
        params.insert("since_id", QString::number(sinceId));
        url.addQueryItem("since_id", QString::number(sinceId));
    }
    if(maxId>0)
    {
        params.insert("max_id", QString::number(maxId));
        url.addQueryItem("max_id", QString::number(maxId));
    }
    if(count!=20&&count>0)
    {
        params.insert("count", QString::number(count));
        url.addQueryItem("count", QString::number(count));
    }
    if(page>1)
    {
        params.insert("page", QString::number(page));
        url.addQueryItem("page", QString::number(page));
    }
    QNetworkRequest request(url);
    this->setOAuthAuthorize( request, OAuth::GET, servicesInstance->getServices("DirectMessages"), params);
    Http *http = new Http(this);
    QString content = http->get(request);
    if(http->hasNetworkError())
    {
        delete http;
        return NULL;
    }
    delete http;
    return content;
}

QString SinaWeibo::getSendDirectMessage(qint64 sinceId, qint64 maxId, int count, int page)
{
    QString urlString(servicesInstance->getServices("MyDirectMessages"));
    QUrl url(urlString);
    QMap<QString, QString> params;
    if(sinceId>0)
    {
        params.insert("since_id", QString::number(sinceId));
        url.addQueryItem("since_id", QString::number(sinceId));
    }
    if(maxId>0)
    {
        params.insert("max_id", QString::number(maxId));
        url.addQueryItem("max_id", QString::number(maxId));
    }
    if(count!=20&&count>0)
    {
        params.insert("count", QString::number(count));
        url.addQueryItem("count", QString::number(count));
    }
    if(page>1)
    {
        params.insert("page", QString::number(page));
        url.addQueryItem("page", QString::number(page));
    }
    QNetworkRequest request(url);
    this->setOAuthAuthorize( request, OAuth::GET, servicesInstance->getServices("MyDirectMessages"), params);
    Http *http = new Http(this);
    QString content = http->get(request);
    if(http->hasNetworkError())
    {
        delete http;
        return NULL;
    }
    delete http;
    return content;
}


/*! \brief Http Basic 认证
 *
 * \warning 已废弃开始采用OAuth认证
 */
void SinaWeibo::setBasicAuth(QNetworkRequest &request)
{
    request.setRawHeader("Authorization", this->basicAuthInfo.toAscii());
}

/*! \brief 获取用户头像
 *
 */
QIcon SinaWeibo::getProfileIcon(const QString &urlString)
{
    QUrl iconUrl;
    iconUrl.setUrl(urlString);
    QNetworkRequest request;
    request.setUrl(iconUrl);
    Http *http = new Http(this);
    QPixmap image = http->getImageFromUrl(request);
    delete http;
    QIcon icon(image);
    return icon;
}
/*! \brief 返回新浪微博官方所有表情
 *
 *  API详情: http://open.t.sina.com.cn/wiki/index.php/Emotions
 *  \return 表情列表(xml),反之返回QString::null
 */
QString SinaWeibo::getEmotions()
{
    QUrl url(servicesInstance->getServices("Emotions")+"?source="+servicesInstance->getServices("Source"));
    QNetworkRequest request;
    request.setUrl(url);
    Http *http = new Http(this);
    QString content = http->get(request);
    delete http;
    return content;
}

/*! \brief 获取当前用户Web主站未读消息数
 *
 * API详情: http://open.t.sina.com.cn/wiki/index.php/Statuses/unread
 * \return 成功返回xml格式列表，否则返回QString::null
 */
QString SinaWeibo::getUnread(QString sinceId)
{
    QString urlString = servicesInstance->getServices("Unread");
    QUrl url(urlString);
    QMap<QString, QString> params;
    if(!sinceId.isEmpty())
    {
        params.insert("with_new_status", QString::number(1));
        url.addQueryItem("with_new_status", QString::number(1));
        params.insert("since_id", sinceId);
        url.addQueryItem("since_id", sinceId);
    }
    QNetworkRequest request;
    request.setUrl(url);
    this->setOAuthAuthorize( request, OAuth::GET, urlString, params);
    Http *http = new Http(this);
    QString content = http->get(request);
    if(http->hasNetworkError())
        return QString::null;
    delete http;
//    qDebug(content.toAscii());
    return content;
}

/*! \brief 批量获取一组微博的评论数及转发数
 *
 * API详情: http://open.t.sina.com.cn/wiki/index.php/Statuses/counts
 *
 * \return 成功时返回评论数和转发数(json),反之返回QStirng::null
 */
QString SinaWeibo::getStatusCountsByJson(QList<Status> &statusList)
{
    QStringList idStrings;
    for( int i=0; i<statusList.length(); i++)
    {
        Status status = statusList.at(i);
        idStrings.append(status.getId());
        /*
        if(!status.getRetweetedId().isEmpty())
        {
            idStrings.append(status.getRetweetedId());
        }
        */
    }
    if(idStrings.isEmpty())
        return QString::null;
    QMap<QString, QString> params;
    QString urlString(servicesInstance->getServices("Counts"));
    QUrl url(urlString);
    params.insert("ids", idStrings.join(",").toAscii().toPercentEncoding());
    url.addQueryItem("ids", idStrings.join(","));
    QNetworkRequest request(url);
    this->setOAuthAuthorize( request, OAuth::GET, urlString, params);
    Http *http = new Http(this);
    QString content = http->get(request);
    if(http->hasNetworkError())
    {
        qDebug("error code %d", http->getNetworkErrorCode());
        delete http;
        return QString::null;
    }
    delete http;
    return content;
}

/*! \brief 表情列表初始化
 *
 */
void SinaWeibo::initEmotions()
{
    //get emotions xml file
    QString emotionXml;
    QFile file(conf->getProfilesDir()+"/emotions.xml");
    if(!file.exists())
    {
        emotionXml = getEmotions();
        if(file.open(QIODevice::WriteOnly))
        {
            QTextStream out(&file);
            out<<emotionXml;
        }
    }
    else
    {
       if( file.open(QIODevice::ReadOnly))
           emotionXml.append(file.readAll());
       else
           emotionXml = getEmotions();
                
    }
    file.close();
    if(!emotionXml.isEmpty())
    {
        SinaServices *sina = SinaServices::getInstance();
        sina->emoticonMap.clear();
        sina->emotionCategoryList.clear();
        sina->emotionList.clear();
        QList<Emotion> &emotionList=sina->emotionList;
        SinaParserXml sinaPX;
        sinaPX.parserEmotions(emotionXml,emotionList);//got the emotion list
        for( int i=0; i<emotionList.length(); i++)
        {
            Emotion emotion = emotionList.at(i);
            sina->emoticonMap.insert(emotion.getPhrase(), emotion.getUrl());
            sina->addToEmotionCategoryList(emotion.getCategory());
        }
    }
}

/*! \brief 发布一条微博信息
 *
 * API详情: http://open.t.sina.com.cn/wiki/index.php/Statuses/update
 * \return 成功时返回QNetworkReply::NoError, 反之返回错误代码
 */
int SinaWeibo::sendStatusWithoutPicture(QString statusContent)
{
    QMap<QString, QString> params;
    params.insert("status", QUrl::toPercentEncoding(statusContent));
    QByteArray data = "status="+QUrl::toPercentEncoding(statusContent.toUtf8());
    QString urlString = servicesInstance->getServices("Update");
    QNetworkRequest request(QUrl::fromUserInput(urlString));
    this->setOAuthAuthorize( request, OAuth::POST, urlString, params);
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                         "application/x-www-form-urlencoded");
    request.setHeader(QNetworkRequest::ContentLengthHeader, 
                         QByteArray::number(data.length()));
    Http *http = new Http(this);
    http->post(request, data);
    if(http->hasNetworkError())
    {
        return http->getNetworkErrorCode();
    }
    delete http;
    return QNetworkReply::NoError;
}

/*! \brief 将当前登录用户的某种新消息的未读数设置为0
 *
 * API详情: http://open.t.sina.com.cn/wiki/index.php/Statuses/reset_count
 * \return 成功返回QNetworkReply::NoError,反之返回相应错误码
 */
int SinaWeibo::resetCount(int type)
{
    if(type<1||type>4)
        return QNetworkReply::NoError;//no error -_-!!!
    QMap<QString, QString> params;
    params.insert("type", QString::number(type));
    QString dataString =  "type="+QString::number(type);
    QNetworkRequest request(QUrl::fromUserInput(servicesInstance->getServices("ResetCount")));
    this->setOAuthAuthorize( request, OAuth::POST, servicesInstance->getServices("ResetCount"), params);
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                         "application/x-www-form-urlencoded");
    request.setHeader(QNetworkRequest::ContentLengthHeader, 
                         QByteArray::number(dataString.toAscii().length()));
    Http *http = new Http(this);
    http->post(request, dataString.toAscii());
    if(http->hasNetworkError())
        return http->getNetworkErrorCode();
    delete http;
    return QNetworkReply::NoError;
}

/*! \brief 转发微博
 *
 * API详情: http://open.t.sina.com.cn/wiki/index.php/Statuses/repost
 * \return 成功返回QNetworkReply::NoError,反之返回相应错误码
 */
int SinaWeibo::repost(QString id, QString status)
{
    QMap<QString, QString> params;
    params.insert("id", id);
    params.insert("status", QUrl::toPercentEncoding(status));
    QByteArray data = "id=" + id.toAscii() + "&status=" + QUrl::toPercentEncoding(status.toUtf8());
    QString urlString = servicesInstance->getServices("Repost");
    QNetworkRequest request(QUrl::fromUserInput(urlString));
    this->setOAuthAuthorize( request, OAuth::POST, urlString, params);
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      "application/x-www-form-urlencoded");
    request.setHeader(QNetworkRequest::ContentLengthHeader,
                      QByteArray::number(data.length()));
    Http *http = new Http(this);
    http->post(request, data);
    if(http->hasNetworkError())
    {
        return http->getNetworkErrorCode();
    }
    delete http;
    return QNetworkReply::NoError;
}

/*! \brief 对一条微博信息进行评论
 *
 * API详情: http://open.t.sina.com.cn/wiki/index.php/Statuses/comment
 * \return 成功返回QNetworkReply::NoError,反之返回相应错误码
 */
int SinaWeibo::commentStatus(QString id, QString status)
{
    QMap<QString, QString> params;
    QByteArray data = "id="+ id.toAscii()+"&comment=" + QUrl::toPercentEncoding(status.toUtf8());
    params.insert("id", id);
    params.insert("comment", QUrl::toPercentEncoding(status));
    QString urlString = servicesInstance->getServices("Comment");
    QNetworkRequest request(QUrl::fromUserInput(urlString));
    this->setOAuthAuthorize( request, OAuth::POST, urlString, params);
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      "application/x-www-form-urlencoded");
    request.setHeader(QNetworkRequest::ContentLengthHeader,
                      QByteArray::number(data.length()));
    Http *http = new Http(this);
    http->post(request, data);
    if(http->hasNetworkError())
    {
        return http->getNetworkErrorCode();
    }
    delete http;
    return QNetworkReply::NoError;
}

/*! \brief 回复评论
 *
 * API详情: http://open.t.sina.com.cn/wiki/index.php/Statuses/reply
 * \return 成功返回QNetworkReply::NoError,反之返回相应错误码
 */
int SinaWeibo::replyComment(QString cid, QString id, QString statusText)
{
    QByteArray data = "id="+ id.toAscii() + "&cid="+ cid.toAscii()+
                      "&comment=" + QUrl::toPercentEncoding(statusText.toUtf8());
    QMap<QString, QString> params;
    params.insert("id", id);
    params.insert("cid", cid);
    params.insert("comment", QUrl::toPercentEncoding(statusText));
    QString urlString = servicesInstance->getServices("CommentReply");
    QNetworkRequest request(QUrl::fromUserInput(urlString));
    this->setOAuthAuthorize( request, OAuth::POST, urlString, params);
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      "application/x-www-form-urlencoded");
    request.setHeader(QNetworkRequest::ContentLengthHeader,
                      QByteArray::number(data.length()));
    Http *http = new Http(this);
    http->post(request, data);
    if(http->hasNetworkError())
    {
        delete http;
        return http->getNetworkErrorCode();
    }
    delete http;
    return QNetworkReply::NoError;
}

/*! \brief 发送私信
 *
 * API详情: http://open.t.sina.com.cn/wiki/index.php/Direct_messages/new
 * \warning 私信的接收方必须是发送方的粉丝(什么破限制)
 * \return 成功返回QNetworkReply::NoError,反之返回相应错误码
 */
int SinaWeibo::sendDirectMessage(QString uid, QString statusText)
{
    QByteArray data = "id="+ uid.toAscii()+ "&text=" + QUrl::toPercentEncoding(statusText.toUtf8());
    QMap<QString, QString> params;
    params.insert("id", uid);
    params.insert("text", QUrl::toPercentEncoding(statusText));
    QString urlString = servicesInstance->getServices("SendDirectMessage");
    QNetworkRequest request(QUrl::fromUserInput(urlString));
    this->setOAuthAuthorize( request, OAuth::POST, urlString, params);
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      "application/x-www-form-urlencoded");
    request.setHeader(QNetworkRequest::ContentLengthHeader,
                      QByteArray::number(data.length()));
    Http *http = new Http(this);
    http->post(request, data);
    if(http->hasNetworkError())
    {
        delete http;
        return http->getNetworkErrorCode();
    }
    delete http;
    return QNetworkReply::NoError;
}

void SinaWeibo::getUnreadInfo(int &statusUnread, int &atMeMentionsUnread, 
                              int &commentsUnread, int &directMessageUnread, int &newFollower)
{
    QString xmlContent;
    if(!account->homePageStatus.isEmpty())
    {
        Status status = account->homePageStatus.at(0);
        xmlContent = getUnread(status.getId());
    }
    else
    {
        xmlContent = getUnread("");
    }
    //qDebug(xmlContent.toAscii());
    if(xmlContent.isEmpty())
    {
        statusUnread += 0;
        atMeMentionsUnread += 0;
        commentsUnread += 0;
        directMessageUnread += 0;
        newFollower += 0;
        return;
    }
    SinaParserXml sinaParserXml;
    sinaParserXml.parserUnreadXml(xmlContent, statusUnread, atMeMentionsUnread, commentsUnread, 
                                  directMessageUnread, newFollower);
}

/*! \brief 添加收藏
 *
 * API详情: http://open.t.sina.com.cn/wiki/index.php/Favorites/create
 * \return 成功返回QNetworkReply::NoError,反之返回相应错误码
 */
int SinaWeibo::addFavorite(QString id)
{
    QByteArray data = "id="+id.toAscii();
    QMap<QString, QString> params;
    params.insert("id", id);

    QString urlString = servicesInstance->getServices("AddFavorite");
    QNetworkRequest request(QUrl::fromUserInput(urlString));

    this->setOAuthAuthorize( request, OAuth::POST, urlString, params);

    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      "application/x-www-form-urlencoded");
    request.setHeader(QNetworkRequest::ContentLengthHeader,
                      QByteArray::number(data.length()));
    Http *http = new Http(this);
    http->post(request, data);
    if(http->hasNetworkError())
    {
        return http->getNetworkErrorCode();
    }
    delete http;
    return QNetworkReply::NoError;
}

/*! \brief 删除收藏
 *
 * API详情:  http://open.t.sina.com.cn/wiki/index.php/Favorites/destroy
 * \return 成功返回QNetworkReply::NoError,反之返回相应错误码
 */
int SinaWeibo::deleteFavorite(QString id)
{
    QMap<QString, QString> params;
    QByteArray data;
    QString urlString = servicesInstance->getServices("DeleteFavorite").arg(id);
    QNetworkRequest request(QUrl::fromUserInput(urlString));
    this->setOAuthAuthorize( request, OAuth::POST, urlString, params);
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      "application/x-www-form-urlencoded");
    request.setHeader(QNetworkRequest::ContentLengthHeader,
                      QByteArray::number(data.length()));
    Http *http = new Http(this);
    http->post(request, data);
    if(http->hasNetworkError())
    {
        return http->getNetworkErrorCode();
    }
    delete http;
    return QNetworkReply::NoError;
}

/*! \brief 删除微博
 *
 * API详情: http://open.t.sina.com.cn/wiki/index.php/Statuses/destroy 
 * 
 * \return 成功返回QNetworkReply::NoError,反之返回相应错误码
 */
int SinaWeibo::deleteStatus(QString id)
{
    QByteArray data;
    QMap<QString, QString> params;
    QString urlString = servicesInstance->getServices("DeleteStatus").arg(id);
    QNetworkRequest request(QUrl::fromUserInput(urlString));
    this->setOAuthAuthorize( request, OAuth::POST, urlString, params);
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      "application/x-www-form-urlencoded");
    request.setHeader(QNetworkRequest::ContentLengthHeader,
                      QByteArray::number(data.length()));
    Http *http = new Http(this);
    http->post(request, data);
    if(http->hasNetworkError())
    {
        return http->getNetworkErrorCode();
    }
    delete http;
    return QNetworkReply::NoError;
}

/*! \brief 发表微博并上传图片
 *
 * API详情: http://open.t.sina.com.cn/wiki/index.php/Statuses/upload
 * \return 成功返回QNetworkReply::NoError,反之返回相应错误码
 */
int SinaWeibo::sendStatusWithPicture(QString statusContent, QByteArray pictureInByteArray, QString fileName)
{
    QString boundary = "---------------------------723690991551375881941828858";

    QByteArray data(QString("--" + boundary + "\r\n").toAscii());
    //statusContent
    data += "Content-Disposition: form-data; name=\"status\"\r\n\r\n";
    data += QUrl::toPercentEncoding(statusContent.toUtf8());
    data += "\r\n";
    //pic
    data += QString("--" + boundary + "\r\n").toAscii();
    data += "Content-Disposition: form-data; name=\"pic\"; filename=\""+fileName.toUtf8()+"\"\r\n";
    data += "Content-Type: image/jpeg\r\n\r\n";
    data += pictureInByteArray;
    data += "\r\n";
    data += QString("--" + boundary + "--\r\n").toAscii();
    //end of data
    
    QMap<QString, QString> params;
    params.insert("status", QUrl::toPercentEncoding(QUrl::toPercentEncoding(statusContent)));
    
    QNetworkRequest request(QUrl::fromUserInput(servicesInstance->getServices("Upload")));
    this->setOAuthAuthorize( request, OAuth::POST, servicesInstance->getServices("Upload"), params);
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      "multipart/form-data; boundary=" + boundary);
    Http *http = new Http(this);
    http->post(request, data);
    if(http->hasNetworkError())
        return http->getNetworkErrorCode();
    delete http;
    return QNetworkReply::NoError;
}

void SinaWeibo::initHomePageStatus()
{
    QString xmlContent = getFriendsTimeline(0, 0, conf->getAccountConf(account->getId(),conf->homeCS), 1, 0, 0);
    account->RemoveAll(account->homePageStatus, account->homePageRetweeted);
    SinaParserXml sinaParserXml;
    sinaParserXml.parserStatusXml(xmlContent, account->homePageStatus, account->homePageRetweeted);
}

void SinaWeibo::initUserWeiboPageStatus()
{
    QString xmlContent = getUserTimeline("0",conf->getAccountConf(account->getId(),conf->userWeiboCS),1,0,0);
    account->RemoveAll(account->userWeiboPageStatus, account->userWeiboPageRetweeted);
    SinaParserXml sinaParserXml;
    sinaParserXml.parserStatusXml(xmlContent, account->userWeiboPageStatus, account->userWeiboPageRetweeted);
}

void SinaWeibo::initAtMePageStatus()
{
    QString xmlContent = getMentions(0, 0, conf->getAccountConf(account->getId(), conf->atMeCS), 1);
    account->RemoveAll(account->atMePageStatus, account->atMePageRetweeted);
    SinaParserXml sinaParserXml;
    sinaParserXml.parserStatusXml(xmlContent, account->atMePageStatus, account->atMePageRetweeted);
}

void SinaWeibo::initFavoritePageStatus()
{
    QString xmlContent = getFavorites(1);
    account->RemoveAll(account->favoritesPageStatus, account->favoritesPageRetweeted);
    SinaParserXml sinaParserXml;
    sinaParserXml.parserStatusXml(xmlContent,account->favoritesPageStatus, account->favoritesPageRetweeted);
}

void SinaWeibo::initCommentPageStatus()
{
    QString xmlContent = getCommentTimeline(0, 0, conf->getAccountConf(account->getId(), conf->commentCS), 1);
    account->RemoveAll(account->commentPageComments, account->commentPageRetweeted);
    SinaParserXml sinaParserXml;
    sinaParserXml.parserCommentTimeline(xmlContent, 
                                        account->commentPageComments, account->commentPageRetweeted);
}

void SinaWeibo::initDirectMessagePageStatus()
{
    QString xmlContent = this->getDirectMessage(0,0,
                                                conf->getAccountConf(account->getId(), conf->directMessageCS), 1);

    account->RemoveAll(account->directMessagePageStatus);

    SinaParserXml sinaParserXml;
    sinaParserXml.parserDirectMessage(xmlContent, account->directMessagePageStatus,SinaParserXml::Receive);
}

QString SinaWeibo::statusTextFormat(QString statusText)
{
    QRegExp urlRE("http://([\\w-]+\\.)+[\\w-]+(/[A-Za-z0-9]*)");
    int s = -1;
    QString reText;
    SinaServices * sina = SinaServices::getInstance();
    SinaParserXml parser;
    while((s=urlRE.indexIn(statusText, s+1))>=0)
    {
        reText = "<a href=javascript:void(0)>"+urlRE.cap(0)+"</a>";
        statusText.replace(s, urlRE.cap(0).length(), reText);
        s += reText.length();
        /*
        if(!urlRE.cap(0).contains("http://t.cn/"))
        {
            reText = "<a href=javascript:void(0)>"+urlRE.cap(0)+"</a>";
            statusText.replace(s, urlRE.cap(0).length(), reText);
            s += reText.length();
        }
        else
        {
            QString vedioUrl;
            QString vedioPicUrl;
            QList<QString> listString;
            QMap<QString,QString> urlMap;
            listString<<urlRE.cap(0);
            QString xmlContent = this->getLongUrl(listString);
            parser.fillUrlMap(xmlContent,urlMap);
            QString longUrl = urlMap.value(urlRE.cap(0));
            if(longUrl.contains("http://v.youku.com"))
            {
                vedioUrl = longUrl;
                MyNetworkCookieJar cookieJar;
                vedioPicUrl = cookieJar.getImageUrl(vedioUrl);
            }

            reText = "<a href=javascript:void(0) onclick=alert('"+vedioUrl+"');event.cancelBubble = true;>"+urlRE.cap(0)+"</a>";
            if (vedioPicUrl.size() != 0)
                reText += "<p><img src=" + vedioPicUrl +"></img><p>";
            statusText.replace(s, urlRE.cap(0).length(), reText);
            s += reText.length();
        }
*/
    }
    QRegExp emotionRE("\\[\\S+\\]");
    emotionRE.setMinimal(true);
    s = -1;
    while( (s = emotionRE.indexIn(statusText, s+1)) >= 0)
    {
        if(!sina->emoticonMap.value(emotionRE.cap(0),"").isEmpty())
        {
            reText = "<img src="+sina->emoticonMap.value(emotionRE.cap(0))+" alt="+emotionRE.cap(0)+" title="+
                     emotionRE.cap(0)+" />";
            statusText.replace(s, emotionRE.cap(0).length(), reText);
            s+= reText.length()-1;
        }
        else
            continue;
    }
    QRegExp trendsRE("#(.+)#");
    trendsRE.setMinimal(true);
    s = -1;
    while((s=trendsRE.indexIn(statusText, s+1))>=0)
    {
        reText="<a href=javascript:void(0)>"+trendsRE.cap(0)+"</a>";
        statusText.replace(s, trendsRE.cap(0).length(),reText);
        s += reText.length();
    }
    QRegExp atUsername("@([A-Za-z0-9\\x4e00-\\x9fa5_-]{2,20})");
    s = -1;
    while((s=atUsername.indexIn(statusText, s+1))>=0)
    {
        reText="<a href=javascript:void(0)>"
               +atUsername.cap(0)+"</a>";
        statusText.replace(s, atUsername.cap(0).length(), reText);
        s += reText.length();
    }
    return statusText;
}

QString SinaWeibo::timeFormatHtml(QDateTime datetime)
{
    if(datetime.isValid())
    {
        QDate date = datetime.date();
        QTime time = datetime.time();
        if(date==QDate::currentDate())//today
        {
            int pastSeconds = datetime.secsTo(QDateTime::currentDateTime());
            if(pastSeconds>0&&pastSeconds<60)// 0~60s
                return "<a href=javascript:void(0) >刚刚</a>";
            else if(pastSeconds<3600)
                return "<a href=javascript:void(0) >"+QString::number((int) pastSeconds/60)+"分钟前</a>";
            else
            {
                return "<a href=javascript:void(0) >今天 "+QString::number(time.hour())+":"+
                        QString::number(time.minute())+"</a>";
            }
        }
        else
        {
            return "<a href=javascript:void(0) >"+QString::number(date.month())+"月"+QString::number(date.day())+"日 "+
                    QString::number(time.hour())+":"+QString::number(time.minute())+"</a>";
        }
    }
    return NULL;
}

/*! \brief 设置OAuth Http Authorization
 *  \param params 请求参数，不包括OAuth认证参数部分
 */
void SinaWeibo::setOAuthAuthorize(QNetworkRequest &request, OAuth::HttpMethod httpMethod, QString url, QMap<QString, QString> &params )
{
    OAuth oauth;
    QMap<QString,QString> oauthParams;
    oauthParams.insert(OAuth::oauthConsumerKeyString, servicesInstance->getServices("Source"));
    oauthParams.insert(OAuth::oauthNonceString, oauth.createNonce());
    oauthParams.insert(OAuth::oauthSignatureMethodString, "HMAC-SHA1");
    oauthParams.insert(OAuth::oauthTimestampString, oauth.createTimeStamp());
    oauthParams.insert(OAuth::oauthVersionString, "1.0");
    oauthParams.insert(OAuth::oauthTokenString, servicesInstance->getServices(SinaServices::MyAccessToken));
    
    QString key = servicesInstance->getServices("Secret")+"&"+servicesInstance->getServices(SinaServices::MyAccessTokenSecret);
    params.unite(oauthParams);
    QByteArray signature = oauth.createSignature( httpMethod, url, key, params);
    oauthParams.insert(OAuth::oauthSignatureString, signature.toPercentEncoding());
    oauth.setOAuthInfo(request, oauthParams);
}

QString SinaWeibo::getPublicStatus(int count){

    Q_ASSERT(count > 0);

    QMap<QString,QString> params;
    QString urlString(servicesInstance->getServices("PublicStatuses"));
    QUrl url;
    url.setUrl(urlString);
    QNetworkRequest request;

    if(count>20)
    {
        url.addQueryItem("count", QString::number(count));
        params.insert("count", QString::number(count));
    }

    request.setUrl(url);
    this->setOAuthAuthorize( request, OAuth::GET, servicesInstance->getServices("PublicStatuses"), params);

    qDebug(request.url().toString().toLocal8Bit());

    Http *http = new Http(this);
    QString content = http->get(request);
    if(http->hasNetworkError())
    {
        return QString::null;
    }
    delete http;
    return content;
}

void SinaWeibo::initPublicStatus()
{
        QString xmlContent = getPublicStatus();
        account->RemoveAll(account->publicStatus, account->publicRetweeted);
        SinaParserXml sinaParserXml;
        sinaParserXml.parserStatusXml(xmlContent, account->publicStatus, account->publicRetweeted);

}

bool SinaWeibo::getSinaAccountByID(Account &account, QString id)
{
    QMap<QString,QString> params;

    QString urlString = servicesInstance->getServices("UserInfo");

    QUrl url;
    url.setUrl(urlString);

    url.addQueryItem("id",id);
    params.insert("id",id);
    url.addQueryItem("source",servicesInstance->getServices("Source"));
    params.insert("source",servicesInstance->getServices("Source"));

    QNetworkRequest request;
    request.setUrl(url);

    this->setOAuthAuthorize( request, OAuth::GET,servicesInstance->getServices("UserInfo"),params );


    Http *http = new Http(this);
    QString content = http->get(request);

    if(http->hasNetworkError())
    {
        switch(http->getNetworkErrorCode())
        {
        case QNetworkReply::HostNotFoundError: /* Host not found error */
            QMessageBox::information(NULL, tr("网络错误"), tr("请确定已连接网络!"),\
                                     QMessageBox::Yes, QMessageBox::Yes);
            break;
        case QNetworkReply::TimeoutError: /* Timeout error */
            QMessageBox::information(NULL, tr("网络错误"), tr("网络超时!"),\
                                     QMessageBox::Yes, QMessageBox::Yes);
            break;
        case QNetworkReply::AuthenticationRequiredError: /* Login error */
            QMessageBox::information(NULL, tr("网络错误"), tr("用户名或密码错误!"),\
                                     QMessageBox::Yes, QMessageBox::Yes);
            break;
        }
        return false;
    }
    delete http;
    SinaParserXml xml;

    return xml.parserSinaAccountInformation(&account, content);


}

void SinaWeibo::getDMSessionStatus(QList<Status> &statusList,QString senderId)
{
    QList<Status> tmpList;
    QList<Status> sendList;
    QList<Status> receiveList;

    SinaParserXml sinaParserXml;

    QString receivedDM = this->getDirectMessage(0,0,
                                                conf->getAccountConf(account->getId(), conf->directMessageCS), 1);
    sinaParserXml.parserDirectMessage(receivedDM, receiveList ,SinaParserXml::Send);
    tmpList.append(receiveList);

    QString sendDM = this->getSendDirectMessage(0,0,
                                            conf->getAccountConf(account->getId(), conf->directMessageCS), 1);
    sinaParserXml.parserDirectMessage(sendDM, sendList,SinaParserXml::Send);
    tmpList.append(sendList);

    QList<Status> sessionStatus;
    Status status;

    for (int i = 0 ; i != tmpList.size() ; ++i )
    {
        status = tmpList.at(i);
        if (senderId == status.getSenderId() || senderId == status.getRecipientId())
        {
            sessionStatus.append(status);
        }
    }

    qSort(sessionStatus.begin(),sessionStatus.end(),compareDateTime);

    statusList = sessionStatus;
}

bool SinaWeibo::compareDateTime(const Status &statusOne,const Status &statusTwo)
{
    return statusOne.getCreatedAt() < statusTwo.getCreatedAt();
}

void SinaWeibo::setCommentsAndRetweetNum(QList<Status> &statusList)
{
    if (statusList.size() == 0)
        return;

    QString jsonContent = getStatusCountsByJson(statusList);
    Status status;

    int commentsNum = 0;
    int retweetedNum = 0;

    for (int i = 0; i != statusList.size(); ++i)
    {
        status = statusList.at(i);
        QRegExp regExp("\"id\":("+status.getId()+"),\"comments\":(\\d+),\"rt\":(\\d+)");
        jsonContent.indexOf(regExp);
        commentsNum = regExp.cap(2).toInt();
        retweetedNum = regExp.cap(3).toInt();
        status.setCommentsNum(commentsNum);
        status.setRetweetedNum(retweetedNum);
        statusList.replace(i,status);
    }
}

void SinaWeibo::setVedioPic(QList<Status> &statusList)
{
    Status status;
    QList<QString> listString;
    QMap<QString,QString> urlMap;

    if (statusList.size() == 0)
        return;

    for (uint i = 0; i != statusList.size(); ++i)
    {
        status = statusList.at(i);
        QRegExp regExp("http://([\\w-]+\\.)+[\\w-]+(/[A-Za-z0-9]*)");
        if (status.getVedioPic().length() != 0)
            continue;
        QString text = status.getText();
        text.indexOf(regExp);
        QString shortUrl = regExp.cap(0);

        if (shortUrl.length() == 0 || !shortUrl.contains("http://t.cn"))
            continue;

        status.setShortVedioUrl(shortUrl);
        listString.append(shortUrl);
        statusList.replace(i,status);
    }
    if (listString.length() == 0)
        return;

    QString xmlContent = this->getLongUrl(listString);
    SinaParserXml parser;
    parser.fillUrlMap(xmlContent,urlMap);
    MyNetworkCookieJar jar;

    for (uint i = 0; i != statusList.size(); ++i)
    {
        status = statusList.at(i);

        if (status.getShortVedioUrl().length() != 0)
        {
            QString longUrl = urlMap.value(status.getShortVedioUrl());
            if(!longUrl.contains("v.youku.com") && !longUrl.contains("www.tudou.com")
                    &&!longUrl.contains("www.yinyuetai.com/vedio") && !longUrl.contains("v.ifeng.com"))
                continue;
            status.setVedioPic(jar.getImageUrl(longUrl));
            status.setLongVedioUrl(longUrl);
            statusList.replace(i,status);
        }
    }
}

void SinaWeibo::setVedioPic(QMap<QString, Status> &retwStatusMap)
{
    QList<Status> list = retwStatusMap.values();
    QStringList strList = retwStatusMap.keys();
    setVedioPic(list);
    for (int i = 0; i !=retwStatusMap.size(); ++i)
    {
        retwStatusMap.insert(strList.at(i),list.at(i));
    }

}

QString SinaWeibo::getFriends(QString userId)
{
    QMap<QString,QString> params;

    QString urlString = servicesInstance->getServices("Friends").arg(userId);

    QUrl url;
    url.setUrl(urlString);

    url.addQueryItem("source",servicesInstance->getServices("Source"));
    params.insert("source",servicesInstance->getServices("Source"));

    QNetworkRequest request;
    request.setUrl(url);
    this->setOAuthAuthorize( request, OAuth::GET,urlString,params );

    Http *http = new Http(this);
    QString content = http->get(request);
    delete http;
    return content;
}

QString SinaWeibo::getFollowers(QString userId)
{
    QMap<QString,QString> params;

    QString urlString = servicesInstance->getServices("Followers").arg(userId);

    QUrl url;
    url.setUrl(urlString);

    url.addQueryItem("source",servicesInstance->getServices("Source"));
    params.insert("source",servicesInstance->getServices("Source"));

    QNetworkRequest request;
    request.setUrl(url);
    this->setOAuthAuthorize( request, OAuth::GET,urlString,params );

    Http *http = new Http(this);
    QString content = http->get(request);
    delete http;
    return content;
}

QString SinaWeibo::FriendsAndFollowersToHtml(QList<Account> &accountList)
{


    QString tmp = everyStatusHtml;

    tmp.prepend("<div style=\"height:93px;overflow:hidden\">");
    tmp.append("</div>");

    QString content;
    Account account;
    Status status;

    for (int i = 0 ; i != accountList.size(); ++i)
    {
        account = accountList.at(i);

        if (account.homePageStatus.size() > 0)
            status = account.homePageStatus.at(0);

        QString tmpStr = tmp;
        tmpStr.replace("userid",account.getId());
        tmpStr.replace("stautsid",status.getId());

        content += tmpStr.arg(account.getProfileImageUrl().replace("/50/","/180/") + " width=90px height=90px")
                                .arg(account.getScreenName());


        if (status.getText().length() >52)
            content = content.arg(statusTextFormat(status.getText().left(46).replace("%","5Zu+5qC35Zu+5qOu56C0").append("......")));
        else
            content = content.arg(statusTextFormat(status.getText().replace("%","5Zu+5qC35Zu+5qOu56C0")));

        content = content.arg("");//Has no retweeted status
        content = content.arg("");//no Right Btn
    }

    return contentFrame.arg(content.replace("5Zu+5qC35Zu+5qOu56C0","%"));


}

QString SinaWeibo::getSingleStatus(QString statusId)
{
    QMap<QString,QString> params;

    QString urlString = servicesInstance->getServices("SingleStatus").arg(statusId);

    QUrl url;
    url.setUrl(urlString);

    url.addQueryItem("source",servicesInstance->getServices("Source"));
    params.insert("source",servicesInstance->getServices("Source"));

    QNetworkRequest request;
    request.setUrl(url);
    this->setOAuthAuthorize( request, OAuth::GET,urlString,params );

    Http *http = new Http(this);
    QString content = http->get(request);
    delete http;
    return content;
}

QString SinaWeibo::getDailyTrends()
{
    QMap<QString,QString> params;
    QString urlString(servicesInstance->getServices("DailyTrends"));
    QUrl url;
    url.setUrl(urlString);
    QNetworkRequest request;

    request.setUrl(url);
    this->setOAuthAuthorize( request, OAuth::GET, servicesInstance->getServices("DailyTrends"), params);

    qDebug(request.url().toString().toLocal8Bit());

    Http *http = new Http(this);
    QString content = http->get(request);
    if(http->hasNetworkError())
    {
        return QString::null;
    }
    delete http;
    return content;
}

QString SinaWeibo::othersCommentsToHtml(QList<Status> & commentList)
{
    QString tmp = everyStatusHtml;


    QString content;
    Status status;

    for (int i = 0; i != commentList.size(); ++i)
    {
        status = commentList.at(i);

        QString tmpStr = tmp;
        tmpStr.replace("userid",status.getUserId());

        content += tmpStr.arg(status.getUserProfileImageUrl())
                                .arg(status.getUserName())
                                .arg(statusTextFormat(status.getText().replace("%","5Zu+5qC35Zu+5qOu56C0")))
                                .arg("")//no retweeted status
                                .arg("");//no button
    }

    return contentFrame.arg(content.replace("5Zu+5qC35Zu+5qOu56C0","%"));
}

QString SinaWeibo::getSquareHtml()
{
    QString tmp = everyStatusHtml;

    QString content;

    content.append((tmp.arg("")//图片
                       .arg("随便看看")//广场类型
                       .arg("到处逛逛 说不定有惊喜在等着你哦~")//广场介绍
                       .arg("")//清除占位符 下同
                       .arg(""))
                   .replace("id=\"statusIdInHtml\"","onclick=showPublicStatus()"));

    content.append((tmp.arg("")//图片
                       .arg("热门话题")//广场类型
                       .arg("来看看今天最关注的事情吧~")//广场介绍
                       .arg("")//清除占位符 下同
                       .arg(""))
                   .replace("id=\"statusIdInHtml\"","onclick=showDailyTrends()"));
    return content;
}

QString SinaWeibo::dailyTrendsToHtml(QStringList &nameList, QStringList &queryList)
{
    QString tmp = everyStatusHtml;



    QString content;

    Q_ASSERT(nameList.size() == queryList.size());

    for(int i = 0 ; i != nameList.size(); ++i)
    {
        content += tmp.arg("")
                      .arg("#" + nameList.at(i) + "#")
                      .arg("#" + queryList.at(i) + "#")
                      .arg("")//清除占位符 下同
                      .arg("");
    }

    return content;
}

QString SinaWeibo::getLongUrl(const QList<QString>& shortUrlList)
{
    QMap<QString,QString> params;
    QString urlString(servicesInstance->getServices("LongUrl"));
    QUrl url;
    url.setUrl(urlString);
    QNetworkRequest request;

    url.addQueryItem("source",servicesInstance->getServices("Source"));
    params.insert("source",servicesInstance->getServices("Source"));

    QString shortUrl;

    for (int i = 0; i != shortUrlList.size(); ++i)
    {
        shortUrl = shortUrlList.at(i);
        url.addQueryItem("url_short",shortUrl.toAscii().toPercentEncoding(":/"));
        params.insert("url_short",shortUrl.toAscii().toPercentEncoding(":/"));
    }

    request.setUrl(url);
/*
    this->setOAuthAuthorize( request, OAuth::GET, servicesInstance->getServices("LongUrl"), params);
    qDebug(request.url().toString().toLocal8Bit());
    */

    Http *http = new Http(this);
    QString content = http->get(request);
    if(http->hasNetworkError())
    {
        return QString::null;
    }
    delete http;
    return content;

}
QString SinaWeibo::getImgAndVedioHtml(const Status &status)
{
    QString imgHtml;
    if (status.getBmiddlePic().length() > 0)
    {
        imgHtml =  "<div><img src="+status.getThumbnailPic()+" "
                "onclick=\"showBigPic('"+status.getThumbnailPic()+"','"+status.getBmiddlePic()+"');event.cancelBubble = true;\""
                    "></img></div>";
    }

    QString vedioHtml;
    if (status.getVedioPic().length() > 0)
    {
        vedioHtml = "<div onclick=\"playVedio('"+status.getLongVedioUrl()+"');event.cancelBubble = true;\" style=\"width:120px;height:90px;background-repeat:no-repeat;background-position:center;background-image: url("+ status.getVedioPic() +"); \">"
                "<img src=\"http://img.t.sinajs.cn/t35/style/images/common/feedvideoplay.gif\" style=\"cursor: pointer; display: block;float:none; margin:auto; padding-top:30px; opacity: 0.5;\" ></img>"
                "</div>";
    }

    return imgHtml + vedioHtml;
}
