#include "appwidget.h"
#include "ui_appwidget.h"

AppWidget::AppWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AppWidget)
{
    ui->setupUi(this);
    appNameList.append("365日历");
    appUrlList.append("http://www.365rili.com/BD_calendar2.html?bd_user=0&bd_sig=71838f1a2e4fda30d464e21f51be9458");
    widthList.append(540);
    heightList.append(500);

    appNameList.append("有道词典");
    appUrlList.append("http://dict.youdao.com/app/baidu?bd_user=0&bd_sig=22127358188185ec2955f81a4d255778");
    widthList.append(586);
    heightList.append(695);

    appNameList.append("植物大战僵尸");
    appUrlList.append("http://apps.bdimg.com/tools/popcap/plantsvszombies/pvz_9_15.swf");
    widthList.append(540);
    heightList.append(405);

    appNameList.append("拯救橙子2");
    appUrlList.append("http://apps.bdimg.com/store/static/kvt/16a15a2b14af63325b45ccb849071e54.swf");
    widthList.append(700);
    heightList.append(520);

    appNameList.append("愤怒的小鸟");
    appUrlList.append("http://flash.7k7k.com/fl_9/20110819/xiaoniao.swf");
    widthList.append(800);
    heightList.append(450);

    appNameList.append("豆瓣电台");
    appUrlList.append("http://douban.fm/swf/53032/radioplayer.swf");
    widthList.append(420);
    heightList.append(186);


    this->setAutoFillBackground(true);
    QPalette palette;
    QPixmap pixmap(":/images/app/应用.png");

    if (pixmap.isNull())
        qDebug("NULL!!!");

    this->setBackgroundRole(QPalette::Window);
    palette.setBrush(this->backgroundRole(), QBrush(pixmap));
    this->setPalette(palette);
    this->hide();
}

AppWidget::~AppWidget()
{
    delete ui;
}

void AppWidget::mousePressEvent(QMouseEvent *pEvent){

    int x = pEvent->x();
    int y = pEvent->y();
/*
    qDebug("x = %d,y = %d",x,y);
    return;
*/
    if (upToBorder <= y && y <= upToBorder + label_height)
    {
        int iVal = (x - leftToBorder)/horizontalDistance + 1;

        if ( 1 <= iVal && iVal <= appNumPerLine)
        {
            Q_ASSERT(this->parentWidget() != NULL);
            ApplicationWidget *pWidget = new ApplicationWidget(this->parentWidget()->parentWidget());
            pWidget->setApplicationArguments(appUrlList.at(iVal-1),widthList.at(iVal-1),heightList.at(iVal-1));
            pWidget->setWindowTitle(appNameList.at(iVal-1));
            pWidget->show();
        }
    }
    else if (upToBorder + label_height + verticalDistance < y && y <=upToBorder + 2*label_height + verticalDistance)
    {
        int iVal = (x - leftToBorder)/horizontalDistance + 1;

        if ( 1 <= iVal && iVal <= appNumPerLine - 2)
        {
            Q_ASSERT(this->parentWidget() != NULL);
            ApplicationWidget *pWidget = new ApplicationWidget(this->parentWidget()->parentWidget());
            pWidget->setApplicationArguments(appUrlList.at(iVal + appNumPerLine-1),widthList.at(iVal + appNumPerLine-1),heightList.at(iVal + appNumPerLine-1));
            pWidget->setWindowTitle(appNameList.at(iVal + appNumPerLine-1));
            pWidget->show();
        }
    }

}
