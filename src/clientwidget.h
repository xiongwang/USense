#ifndef CLIENTWIDGET_H
#define CLIENTWIDGET_H

#include <QWidget>

namespace Ui {
    class ClientWidget;
}

class ClientWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ClientWidget(QWidget *parent = 0);
    ~ClientWidget();
    void setLoginPic(QString);
public slots:
    void loginButtonClicked();
signals:
    void loginButtonClickedSignal(QString,QString);

private:
    Ui::ClientWidget *ui;
    QPixmap sinaLogin;
    QPixmap renrenLogin;
    QPalette palette;
};

#endif // CLIENTWIDGET_H
