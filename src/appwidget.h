#ifndef APPWIDGET_H
#define APPWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include "app/applicationwidget.h"

namespace Ui {
    class AppWidget;
}

class AppWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AppWidget(QWidget *parent = 0);
    ~AppWidget();
private:
    Ui::AppWidget *ui;
    QMap<QString,QString> appMap;
    QMap<int,int> appSizeMap;
    QStringList appNameList;
    QStringList appUrlList;
    QList<int> widthList;
    QList<int> heightList;
    const static int label_width = 72;
    const static int label_height = 72;
    const static int verticalDistance = 18;
    const static int horizontalDistance = 90;
    const static int leftToBorder = 30;
    const static int upToBorder = 10;
    const static int appNumPerLine = 4;


protected:
    void mousePressEvent(QMouseEvent *event);
};

#endif // APPWIDGET_H
