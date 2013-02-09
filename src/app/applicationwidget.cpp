#include "applicationwidget.h"
#include "ui_applicationwidget.h"

ApplicationWidget::ApplicationWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ApplicationWidget)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_DeleteOnClose);
    ui->webview->settings()->setAttribute(QWebSettings::PluginsEnabled, true);
    pLabel = new QLabel(this);
    QPixmap pixmap(":/images/关闭小.png");
    pixmap = pixmap.scaled(30,30);
    pLabel->setPixmap(pixmap);

}

ApplicationWidget::~ApplicationWidget()
{
    delete ui;
}

void ApplicationWidget::setApplicationArguments(QString url, int width, int height)
{
    this->setGeometry(80,0,width+30,height);
    ui->webview->setGeometry(0,0,width,height);
    ui->webview->load(url);
    pLabel->setGeometry(width,0,30,30);
}

void ApplicationWidget::mousePressEvent(QMouseEvent *pEvent)
{
    int x = pEvent->x();
    int y = pEvent->y();

    QRect webviewRect = ui->webview->geometry();

    QPoint topRightPoint = webviewRect.topRight();

    if (topRightPoint.x()<= x
            && x <= topRightPoint.x() + 30
            && 0 <= y
            && y <= 30)
    {
        this->close();
    }
}


