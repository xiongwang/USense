

#include "showpicture.h"

ShowPicture::ShowPicture(QString bigUrl, QString midUrl, QWidget *parent) :
    QWidget(parent)
{
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->bigUrl = bigUrl;
    this->midUrl = midUrl;
    if(midUrl.toUpper().endsWith(".GIF"))
        this->isGif = true;
    else
        this->isGif = false;
    this->setWindowTitle(tr("图片"));
    progressBar = new QProgressBar(this);
    progressBar->setValue(0);
    label = new QLabel(this);
    label->setText("正在下载......");
    this->showGif = new QLabel(this);
    this->showGif->hide();
    this->movie = new QMovie(this);
    this->gifImageFile = new QBuffer(&imageByteArray, this);
    layout = new QVBoxLayout(this);
    layout->addWidget(label);
    layout->addWidget(progressBar);
    
    toolBar = new QToolBar(this);
    saveFile = new QAction(this);
    bigPicture = new QAction(this);
    saveFile->setText("保存图片");
    bigPicture->setText("查看大图");

    QObject::connect(saveFile, SIGNAL(triggered()), this, SLOT(saveImage()));
    QObject::connect(bigPicture, SIGNAL(triggered()), this, SLOT(showBigPicture()));
    toolBar->addAction(saveFile);
    toolBar->addAction(bigPicture);
    toolBar->hide();
    view = new QGraphicsView(this);
    scene = new QGraphicsScene(this);
    view->setScene(scene);
    view->hide();
    
    http = new Http(this);
    QObject::connect(http, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(updateProgressBar(qint64,qint64)));
    //QObject::connect(http, SIGNAL(finished()), this, SLOT(showAfterDownload()));
    timer = new QTimer(this);
    timer->setSingleShot(true);
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(downloadFile()));
    timer->start(500);
}

ShowPicture::~ShowPicture()
{
    qDebug("show picture deleted!");
}

void ShowPicture::downloadFile()
{
    timer->stop();
    QNetworkRequest request(QUrl::fromUserInput(midUrl));
    this->imageByteArray = http->getImageByteArrayFromUrl(request);
    img = QImage::fromData(this->imageByteArray);
    showAfterDownload();
}

void ShowPicture::showAfterDownload()
{
    if(this->isHidden())
    {
        delete this;
        return;
    }
    label->hide();
    progressBar->hide();
    /*
    toolBar->setVisible(true);
    */
    layout->addWidget(toolBar);
    QPixmap image;
    image.loadFromData(imageByteArray);
    if(isGif)
    {
        movie->setDevice(gifImageFile);
        showGif->setMovie(movie);
        showGif->setVisible(true);
        showGif->setAlignment(Qt::AlignCenter);
        layout->addWidget(showGif);
        movie->start();
    } 
    else
    {
        scene->clear();
        scene->addPixmap(image);
        view->setVisible(true);
        layout->addWidget(view);
    }
    //resizeDialog(image.width(), image.height());
    view->resize(img.width(), img.height());
    this->setGeometry(540,0,img.width()+30,img.height()+30);
//    resize(img.width()+30,image.height()+30);
}

void ShowPicture::resizeDialog(int imageWidth, int imageHeight)
{

    int extraWidth = imageWidth - view->width();
    int extraHeight = imageHeight - view->height()+30;
    if(isGif)
    {
        showGif->resize(imageWidth, imageHeight);
    }
    else
    {
        view->resize(imageWidth, imageHeight);
    }
    
    this->resize(this->width()+extraWidth, this->height()+extraHeight);
}

void ShowPicture::updateProgressBar(qint64 bytesReceived, qint64 bytesTotal)
{
    progressBar->setMaximum(bytesTotal);
    progressBar->setValue(bytesReceived);
}

void ShowPicture::saveImage()
{
    QFileInfo fileInfo(midUrl);
    QString fileName = QFileDialog::getSaveFileName(this, tr("保存图片"), QDir::homePath()+"/"+fileInfo.fileName());
    QFile file(fileName);
    if(file.open(QIODevice::WriteOnly))
    {
        file.write(imageByteArray);
        file.flush();
    }
    file.close();
}

void ShowPicture::showBigPicture()
{
    QDesktopServices::openUrl(bigUrl);
}

void ShowPicture::closeEvent(QCloseEvent *event)
{
    if(!http->isHttpFinish())
    {
        this->hide();
        return;
    }
    delete this;
}

void ShowPicture::reject()
{
    close();
}

void ShowPicture::setUrl(QString bigUrl, QString midUrl)
{
    this->bigUrl = bigUrl;
    this->midUrl = midUrl;
}
