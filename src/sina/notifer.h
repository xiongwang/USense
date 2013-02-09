

#ifndef NOTIFER_H
#define NOTIFER_H

#include <QWidget>
#include <QtCore>
#include "http.h"
#include "SinaGlobalNamespace.h"

namespace Ui {
    class Notifer;
}

/*! \class Notifer
 * \brief 消息提醒
 *
 * 英语拼写错了
 */
class Notifer : public QWidget
{
    Q_OBJECT

public:
    explicit Notifer(QWidget *parent = 0);
    ~Notifer();
    void setNotiferContentAndShow(QList<WithU::NotifyObject> messageList );
    void setNotiferContentAndShow(QString msg);
    void showIt();
    bool eventFilter(QObject *target, QEvent *event);
    void fadeOut();
    void setText();
    void setText(QString msg);
    
    
public slots:
    void PreButtonClicked();
    void NextButtonClicked();
    void ExitButtonClicked();
    void timeOutDeal();
    
private:
    void downloadPixmap();
    
private:
    Ui::Notifer *ui;
    QTimer *timer;//定时器，用来实现渐隐
    Http *http;//用来进行网络操作
    double timePassby;
    int currentMessageNumber;//当前信息编号
    QList<WithU::NotifyObject> mList;//提醒消息列表
    QMap<QString, QPixmap> urlPixmapMap;//根据url存储图片
};

#endif // NOTIFER_H
