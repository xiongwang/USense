

#ifndef HTTP_H
#define HTTP_H

#include <QObject>
#include <QtNetwork>
#include <QNetworkReply>

/*! \class Http
 *  \brief 提供Http Get/Post方法
 *
 * 默认超时时间为60s
 */
class Http : public QObject
{
    Q_OBJECT
public:
    explicit Http(QObject *parent = 0);
    ~Http();
    QString get(QNetworkRequest& request);
    QPixmap getImageFromUrl(QNetworkRequest& request);
    QByteArray getImageByteArrayFromUrl(QNetworkRequest &request);
    QString post(QNetworkRequest& request, const QByteArray urlencodedAndUtf8FormatData);
    /*! \brief 获取进行网络连接时是否有错误
     */
    bool hasNetworkError(){ return this->m_hasNetworkError; }
    /*! \brief 获取网络连接错误代码
     */
    int getNetworkErrorCode(){ return this->m_networkErrorCode; }
    /*! \brief 网络连接是否完成
     */
    bool isHttpFinish(){ return this->isFinished; }
    void setTimeOutLimit(int time);
    void initPixMap(QUrl);
    QByteArray getImageByteArray();
    

signals:
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void finished();
    void downloadFinishedSignal(QByteArray);
    
public slots:
    void httpFinished(QNetworkReply* reply);
    void handleTimeOut();
    void handlerNetworkError(QNetworkReply::NetworkError error);
    void imageDownloadFinished(QNetworkReply* reply);
    
private:
    QNetworkReply* netReply;
    QNetworkAccessManager *network;
    bool m_hasNetworkError;//是否发生网络错误
    int m_networkErrorCode;//错误代码.如果发生网络错误,该值不为0
    QByteArray contentInByteArray;//接受的内容
    QByteArray imageByteArray;
    QEventLoop* qEventLoop;//接受内容时保持响应
    volatile bool isFinished;
    QTimer* timer;//定时,用于超时检测
    int timeLimit;//用于设置超时

};

#endif // HTTP_H
