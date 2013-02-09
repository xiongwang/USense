#ifndef APPLICATIONWIDGET_H
#define APPLICATIONWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QtGui>

namespace Ui {
    class ApplicationWidget;
}

class ApplicationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ApplicationWidget(QWidget *parent = 0);
    ~ApplicationWidget();
    void setApplicationArguments(QString url,int width, int height);

private:
    Ui::ApplicationWidget *ui;
    QLabel *pLabel;

protected:
    void mousePressEvent(QMouseEvent *);
};

#endif // APPLICATIONWIDGET_H
