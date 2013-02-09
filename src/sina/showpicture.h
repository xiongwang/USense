

#ifndef SHOWPICTURE_H
#define SHOWPICTURE_H

#include <QTimer>
#include <QtGui>
#include "http.h"

/*! \class ShowPicture
 * \brief 显示图片
 *
 *
 */
class ShowPicture : public QWidget
{
    Q_OBJECT
public:
    explicit ShowPicture(QString bigUrl,QString midUrl, QWidget *parent = 0);
    void setUrl(QString bigUrl,QString midUrl);
    ~ShowPicture();

signals:

public slots:
    void downloadFile();
    void updateProgressBar(qint64 bytesReceived, qint64 bytesTotal);
    void saveImage();
    void resizeDialog(int imageWidth, int imageHeight);
    void showBigPicture();
    void showAfterDownload();
    void closeEvent(QCloseEvent *event);
    void reject();
private:
    QToolBar *toolBar;
    QAction *saveFile;
    QAction *bigPicture;
    QGraphicsView *view;
    QGraphicsScene *scene;
    QVBoxLayout *layout;
    QLabel *label;
    QLabel *showGif;
    QMovie *movie;
    QBuffer *gifImageFile;
    QTimer *timer;
    QProgressBar *progressBar;
    QString bigUrl;
    QString midUrl;
    Http *http;
    QByteArray imageByteArray;
    QImage img;
    bool isGif;
};

#endif // SHOWPICTURE_H
