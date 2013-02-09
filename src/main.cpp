#include <QtGui/QApplication>
#include <QtCore>
#include <QTextCodec>

#include "sina/account.h"
#include "sina/sinaweibo.h"
#include "mainwidget.h"
#include "rss/rssreader.h"
#include "sina/showpicture.h"

#include "QGraphicsView"
#include "QGraphicsScene"

int main(int argc, char *argv[])
{
    QApplication app(argc,argv);



    QTextCodec *encoding = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForTr(encoding);
    QTextCodec::setCodecForLocale(encoding);
    QTextCodec::setCodecForCStrings(encoding);

    QWaitCondition waitCondition;
    QMutex mutex;

    QSplashScreen *splash = new QSplashScreen;
    splash->setPixmap(QPixmap(":/images/启动动画.png"));
    splash->show();
    MainWidget w;
    mutex.lock();
    waitCondition.wait(&mutex,3000);
    mutex.unlock();
    w.show();
    splash->finish(&w);
    delete splash;



    return app.exec();


}
