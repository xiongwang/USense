
#include "http.h"
#include <QMessageBox>
#include <QApplication>
#include "sinaservices.h"

/*! \brief Http类构造函数
  */
Http::Http(QObject *parent) :
    QObject(parent)
{
    this->isFinished = false;
    this->network = new QNetworkAccessManager(this);
    this->netReply = NULL;
    this->m_hasNetworkError = false;
    this->m_networkErrorCode = QNetworkReply::NoError;
    this->qEventLoop = new QEventLoop(this);
    this->timer = new QTimer(this);
    this->contentInByteArray.clear();
    this->timeLimit = 60 * 1000;//默认超时时间为60s
    QObject::connect( network, SIGNAL(finished(QNetworkReply*)), this, SLOT(httpFinished(QNetworkReply*)));
    QObject::connect( network, SIGNAL(finished(QNetworkReply*)), this, SIGNAL(finished()));
    QObject::connect( timer, SIGNAL(timeout()), this, SLOT(handleTimeOut()));
}

Http::~Http()
{
    this->netReply=NULL;
}

/*! \brief Get Method
  */
QString Http::get(QNetworkRequest &request)
{
    this->netReply = network->get(request);
    QObject::connect(netReply, SIGNAL(error(QNetworkReply::NetworkError)), 
                     this, SLOT(handlerNetworkError(QNetworkReply::NetworkError)));
    timer->start(timeLimit);
    qEventLoop->exec();
    this->netReply->close();
    delete netReply;
    netReply = NULL;
    if(!hasNetworkError())//success
    {
        QTextCodec *codec = QTextCodec::codecForName("UTF-8");
        return codec->toUnicode(contentInByteArray);
    }
    else
    {
        return QString::null;
    }
}

/*! \brief 下载图片
 *
 */
QPixmap Http::getImageFromUrl(QNetworkRequest &request)
{
    this->netReply = network->get(request);
    QObject::connect(netReply, SIGNAL(error(QNetworkReply::NetworkError)),
                     this, SLOT(handlerNetworkError(QNetworkReply::NetworkError)));
    timer->start(timeLimit);
    while(!isFinished)
        qEventLoop->exec();
    netReply->close();
    delete netReply;
    netReply = NULL;
    QPixmap image;
    if(!hasNetworkError())
    {
        image.loadFromData(contentInByteArray);
    }
    return image;
}

/*! \brief 获取图片数据
 *
 */
QByteArray Http::getImageByteArrayFromUrl(QNetworkRequest &request)
{
    this->netReply = network->get(request);
    QObject::connect(netReply, SIGNAL(downloadProgress(qint64,qint64)), this, SIGNAL(downloadProgress(qint64,qint64)));
    QObject::connect(netReply, SIGNAL(error(QNetworkReply::NetworkError)),
                     this, SLOT(handlerNetworkError(QNetworkReply::NetworkError)));

    timer->start(timeLimit);
    qEventLoop->exec();
    netReply->close();
    delete netReply;
    netReply = NULL;
    return contentInByteArray;
}

/*! \brief post method
 */
QString Http::post(QNetworkRequest &request, const QByteArray urlencodedAndUtf8FormatData)
{
    netReply = network->post(request, urlencodedAndUtf8FormatData);
    QObject::connect(netReply, SIGNAL(error(QNetworkReply::NetworkError)),
                     this, SLOT(handlerNetworkError(QNetworkReply::NetworkError)));
    timer->start(timeLimit);
    qEventLoop->exec();
    netReply->close();
    delete netReply;
    netReply = NULL;
    return contentInByteArray;
}
/*! \brief http连接已完成
 */
void Http::httpFinished(QNetworkReply *reply)
{
    this->isFinished = true;
    this->contentInByteArray = reply->readAll();
    this->timer->stop();
    this->qEventLoop->exit();
}

/*! \brief 超时处理
 */
void Http::handleTimeOut()
{
    m_hasNetworkError = true;
    m_networkErrorCode = QNetworkReply::TimeoutError;
    contentInByteArray = netReply->readAll();
    timer->stop();
    qEventLoop->exit();
}

/*! \brief 网络错误处理
 *
 */
void Http::handlerNetworkError(QNetworkReply::NetworkError error)
{
    this->m_hasNetworkError = true;
    this->m_networkErrorCode = error;
    qDebug("error string is "+this->netReply->errorString().toAscii());
}

/*! \brief 设置超时时间，单位为秒
 *
 */
void Http::setTimeOutLimit(int time)
{
    this->timeLimit = time * 1000;
}

void Http::initPixMap(QUrl url)
{
    QNetworkRequest request;
    request.setUrl(url);
    network->get(request);
    connect(network,SIGNAL(finished(QNetworkReply*)),this,
            SLOT(imageDownloadFinished(QNetworkReply*)));
}

void Http::imageDownloadFinished(QNetworkReply *reply)
{
    disconnect(network,SIGNAL(finished(QNetworkReply*)),this,SLOT(imageDownloadFinished(QNetworkReply*)));
    imageByteArray = reply->readAll();
}

QByteArray Http::getImageByteArray()
{
    return this->imageByteArray;
}
