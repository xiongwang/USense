#include "dealrenrenevent.h"

Dealrenrenevent::Dealrenrenevent(RenRenAccount * account, QObject *parent) :
    QObject(parent)
{
    this->account = account;
    this->domReady = false;
}

void Dealrenrenevent::getSameFriends()
{
    emit getSameFriendsSignal();
}

void Dealrenrenevent::getCommentsPage(QString owner_id, QString status_id)
{
    emit getCommentsPageSignal(owner_id,status_id);
}

void Dealrenrenevent::getShareInfo(QString post_id)
{
    emit getShareInfoSignal(post_id);
}

void Dealrenrenevent::readStatus(QString id, QString uid)
{
    emit readStatusSignal(id, uid);
}

void Dealrenrenevent::readBlog(QString id, QString uid)
{
    emit readBlogSignal(id, uid);
}

void Dealrenrenevent::getProfileInfo(QString uid)
{
    emit getProfileInfoSignal(uid);
}

void Dealrenrenevent::getStatusInfo(QString uid)
{
    emit getStatusInfoSignal(uid);
}

void Dealrenrenevent::getAlbumsInfo(QString uid)
{
    emit getAlbumsInfoSignal(uid);
}

void Dealrenrenevent::getBlogsInfo(QString uid)
{
    emit getBlogsInfoSignal(uid);
}

void Dealrenrenevent::getNewPageInfo(int page)
{
    emit getNewPageSignal(page);
}

void Dealrenrenevent::setDomReady(bool b)
{
    if (b == true)
        qDebug("setDomReady(true)");
    this->domReady = b;
}

bool Dealrenrenevent::isDomReady()
{
    return this->domReady;
}

void Dealrenrenevent::addEmotion(QString str)
{
    emit addEmotionSignal(str);
}

void Dealrenrenevent::createStatusWidget(QString arg_1, QString arg_2,QString arg_3, QString arg_4,int widgetType)
{
//    qDebug("current widget type : "+QString::number(widgetType).toLocal8Bit());
    emit createStatusWidgetSignal(arg_1,arg_2,arg_3,arg_4,widgetType);
}

void Dealrenrenevent::forwardToOtherClient(QString str)
{
    qDebug("in Dealrenrenevent forwardToOtherClient");
    emit forwardToOtherClientSignal(str);
}

void Dealrenrenevent::playVideo(QString str)
{
    emit playVideoSignal(str);
}
