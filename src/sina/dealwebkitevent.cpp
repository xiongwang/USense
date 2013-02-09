#include "dealwebkitevent.h"
#include "sinaweibo.h"
#include "sinaparserxml.h"
#include "statuswidget.h"
#include <QDesktopServices>
#include <QUrl>
#include <QMessageBox>

DealWebkitEvent::DealWebkitEvent(Account *account, QObject *parent) :
    QObject(parent)
{
    this->account = account;
    this->domReady = false;
    parentWidget = static_cast<QWidget*>(parent);
}

void DealWebkitEvent::retwStatusAction(QString idText, QString appendText)
{
    QString realText = QString::fromAscii(QByteArray::fromBase64(appendText.toAscii()).data());
    StatusWidget *pStatusWidget = new StatusWidget(account, idText, realText,parentWidget);
    pStatusWidget->setAttribute(Qt::WA_DeleteOnClose);
    pStatusWidget->show();
}

void DealWebkitEvent::getNewPageInfo(QString page, int feature)
{
//    qDebug("new page:" + page.toLocal8Bit()+QString::number(feature).toLocal8Bit());
    emit getNewPageInfoSignal(page, feature);
}

void DealWebkitEvent::commentStatus(QString id)
{
    StatusWidget *pStatusWidget = new StatusWidget(account, id,parentWidget);
    pStatusWidget->setAttribute(Qt::WA_DeleteOnClose);
    pStatusWidget->show();
}

void DealWebkitEvent::replyComment(QString cid, QString id)
{
    StatusWidget *pStatusWidget = new StatusWidget(account, cid, id, 3,parentWidget);
    pStatusWidget->setAttribute(Qt::WA_DeleteOnClose);
    pStatusWidget->show();
}

void DealWebkitEvent::openUrl(QString url)
{
    QDesktopServices::openUrl(QUrl(url));
}

void DealWebkitEvent::addEmotionPhase(QString phase)
{
    emit this->insertPhaseText(phase);
}

void DealWebkitEvent::addFavorite(QString id)
{
    SinaWeibo *sina = new SinaWeibo(account);
    int errorCode = sina->addFavorite(id);
    if(errorCode == QNetworkReply::NoError)
        QMessageBox::information(NULL, tr("成功"), tr("收藏添加成功！"), QMessageBox::Ok);
    else
    {
        QMessageBox::information(NULL, tr("错误"), tr("添加收藏失败！"), QMessageBox::Ok);
        qDebug("Error code is %d", errorCode);
    }
    delete sina;
}

void DealWebkitEvent::sendDirectMessage(QString id)
{
    StatusWidget *pStatusWidget = new StatusWidget(account, id, 4,parentWidget);
    pStatusWidget->setAttribute(Qt::WA_DeleteOnClose);
    pStatusWidget->show();
}

void DealWebkitEvent::deleteFavorite(QString id)
{
    SinaWeibo *sina = new SinaWeibo(account);
    int errorCode = sina->deleteFavorite(id);
    if(errorCode == QNetworkReply::NoError)
    {
        QMessageBox::information(NULL, tr("成功"), tr("删除收藏成功!"), QMessageBox::Ok);
        emit this->hideDivById(id);
    }
    else
    {
        QMessageBox::information(NULL, tr("错误"), tr("删除收藏失败!"), QMessageBox::Ok);
        qDebug("Error code is %d", errorCode);
    }
    delete sina;
    
}

void DealWebkitEvent::deleteMyStatus(QString id)
{
    SinaWeibo *sina = new SinaWeibo(account);
    int errorCode = sina->deleteStatus(id);
    if(errorCode == QNetworkReply::NoError)
    {
        QMessageBox::information(NULL, tr("成功"), tr("删除微博成功!"), QMessageBox::Ok);
        emit this->hideDivById(id);
    }
    else
    {
        QMessageBox::information(NULL, tr("错误"), tr("删除微博失败!"), QMessageBox::Ok);
        qDebug("Error code is %d", errorCode);
    }
    delete sina;
}

void DealWebkitEvent::getUserTimelineByScreenName(QString screenName, int page)
{
    emit getUserTimelineByScreenNameSignal(screenName, page);
}

void DealWebkitEvent::getMoreHomePageStatus(int page, int feature)
{
    emit this->getMoreHomePageStatusSignal(page, feature);
}

void DealWebkitEvent::getMoreUserPageStatus(int page, int feature)
{
    emit this->getMoreUserPageStatusSignal(page, feature);
}

void DealWebkitEvent::getMoreAtMePageStatus(int page)
{
    emit this->getMoreAtMePageStatusSignal(page);
}

void DealWebkitEvent::getMoreCommentsPageStatus(int page)
{
    emit this->getMoreCommentsPageStatusSignal(page);
}

void DealWebkitEvent::getMoreFavoritePageStatus(int page)
{
    emit this->getMoreFavoritePageStatusSignal(page);
}

void DealWebkitEvent::getMoreDirectMessagePageStatus(int page)
{
    emit this->getMoreDirectMessagePageStatusSignal(page);
}

void DealWebkitEvent::bigPicture(QString bigUrl, QString midUrl)
{
    emit showBigPicture(bigUrl,midUrl);
}

void DealWebkitEvent::openTrends(QString trendsName)
{
    QDesktopServices::openUrl(QUrl("http://t.sina.com.cn/k/"+trendsName));
}

void DealWebkitEvent::setDomReady( bool b)
{
    this->domReady = b;
}

bool DealWebkitEvent::isDomReady()
{
    return this->domReady;
}

void DealWebkitEvent::sendStatusesCounts(QString jsonContent)
{
    //qDebug(jsonContent.toAscii());
    emit this->parserStatusesCounts(jsonContent);
}

void DealWebkitEvent::getCommentsList(QString id, int page)
{
    SinaWeibo *sina = new SinaWeibo(this->account);
    QString xmlContent = sina->getCommentsList(id,page, 10);//10条,暂时限定为10条
    QList<Status> statusList; QMap<QString, Status> statusMap;
    SinaParserXml sinaParserXml;
    sinaParserXml.parserCommentTimeline(xmlContent, statusList, statusMap);
    /*
    QString pageHtml;
    if(page<=1)
    {
        if(statusList.length()<10)
            pageHtml = "";
        else
            pageHtml="<a href=javascript:void(0) onclick=getMoreCommentsList("+id+",2) >下一页</a><hr>";
    } else {
        if(statusList.length()<10)
            pageHtml="<a href=javascript:void(0) onclick=getMoreCommentsList("+
                     id+","+QString::number(page-1)+") >上一页</a><hr>";
        else
            pageHtml="<a href=javascript:void(0) onclick=getMoreCommentsList("+id+","+QString::number(page-1)+") >上一页</a>"
                     "<a href=javascript:void(0) onclick=getMoreCommentsList("+id+","+QString::number(page+1)+") >下一页</a><hr>";
    }
    */
    QString html = sina->commentsListToHtml(statusList);
    //qDebug(pageHtml.toAscii());
    //html.append(pageHtml);
    delete sina;
    emit this->setCommentsListHtml(html,id);
}

void DealWebkitEvent::forwardToOtherClient(QString str)
{
    emit forwardToOtherClientSignal(str);
}

void DealWebkitEvent::getSessionStatus(QString str)
{
    emit getSessionStatusSignal(str);
}

void DealWebkitEvent::showUserInfoSlot(QString str)
{
    emit showUserInfoSignal(str);
}

void DealWebkitEvent::showFriendsSlot(QString str)
{
    emit showFriendsSignal(str);
}

void DealWebkitEvent::showFollowersSlot(QString str)
{
    emit showFollowersSignal(str);
}

void DealWebkitEvent::showCommentsSlot(QString userId, QString statusId)
{
    emit showCommentsSignal(userId,statusId);
}

void DealWebkitEvent::showPublicStatusSlot()
{
    emit showPublicStatusSignal();
}

void DealWebkitEvent::showDailyTrendsSlot()
{
    emit showDailyTrendsSignal();
}

void DealWebkitEvent::sendStatusWithAtSlot(QString username)
{
    StatusWidget *pStatusWidget = new StatusWidget(this->account,parentWidget);
    pStatusWidget->setAttribute(Qt::WA_DeleteOnClose);
    pStatusWidget->setText("@" + username + " ");
    pStatusWidget->show();
}

void DealWebkitEvent::playVedioSlot(QString vedioUrl)
{
    emit playVedioSignal(vedioUrl);
}
