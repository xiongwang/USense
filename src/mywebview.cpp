#include "mywebview.h"


MyWebView::MyWebView(QWidget *parent) :
    QWebView(parent)
{
    lastY = 0;
    isPressed = false;
}

void MyWebView::mousePressEvent(QMouseEvent *e)
{
    isPressed = true;
    lastY = e->pos().y();
    QWebView::mousePressEvent(e);
}

void MyWebView::mouseReleaseEvent(QMouseEvent *e)
{
    isPressed = false;
    QWebView::mouseReleaseEvent(e);
}

void MyWebView::mouseMoveEvent(QMouseEvent *e)
{


    bool ok = isPressed;
    int offset = e->pos().y() - lastY;

    if (ok)
    {
        ok = (offset > 20 || offset < -20);
    }
    else
        return;
    //qDebug("x=%d,y=%d,lastY=%d,offset=%d",e->pos().x(),e->pos().y(),lastY,offset);
    if (ok)
        ok = offset > 0;
    else
        return;
    if (ok)
    {
        lastY = e->pos().y();
        this->page()->mainFrame()->evaluateJavaScript("down("+QString::number(15)+")");
    }
    else
    {
        lastY = e->pos().y();
        this->page()->mainFrame()->evaluateJavaScript("up("+QString::number(15)+")");
    }

}

