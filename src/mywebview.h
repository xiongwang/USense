#ifndef MYWEBVIEW_H
#define MYWEBVIEW_H

#include <QWebView>
#include <QtWebKit>

class MyWebView : public QWebView
{
    Q_OBJECT
public:
    explicit MyWebView(QWidget *parent = 0);

signals:

public slots:

protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);

private:
    int lastY;
    bool isPressed;

};

#endif // MYWEBVIEW_H
