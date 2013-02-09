#include "renrenstatuswidget.h"
#include "ui_renrenstatuswidget.h"
#include <QtWebKit>
#include <QMessageBox>

RenrenStatusWidget::RenrenStatusWidget(RenRenAccount *account,QWidget* parent) :
    ui(new Ui::RenrenStatusWidget)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->account = account;
    this->widgetType = 0;
    this->dealRenrenEvent = new Dealrenrenevent(account);
    ui->groupBox_Emotions->hide();
    this->setGeometry(80,80,620,235);
    connectSlot();
    init();
}

RenrenStatusWidget::RenrenStatusWidget(RenRenAccount *account, int widgetType,QWidget* parent) :
    ui(new Ui::RenrenStatusWidget)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->account = account;
    this->dealRenrenEvent = new Dealrenrenevent(account);
    this->widgetType = widgetType;
    ui->groupBox_Emotions->hide();
    this->setGeometry(80,80,620,235);
    connectSlot();
    init();
}

RenrenStatusWidget::RenrenStatusWidget(RenRenAccount *account, QString arg_1, QString arg_2, int widgetType,QWidget* parent)
    :ui(new Ui::RenrenStatusWidget)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->account = account;
    this->dealRenrenEvent = new Dealrenrenevent(account);
    ui->groupBox_Emotions->hide();
    this->setGeometry(80,80/2,620,235);
    if(widgetType == 0)
        ui->title_label->setText("发新状态");
    if (widgetType == 1)
    {
        this->forward_id = arg_1;
        this->forward_owner = arg_2;
        ui->title_label->setText("转发状态");
    }
    else if (widgetType == 2)
    {
        this->status_id = arg_1;
        this->owner_id = arg_2;
        ui->title_label->setText("回复");
    }
    else if (widgetType == 3 || widgetType == 4 || widgetType == 5 || widgetType == 6)
        ui->title_label->setText("评论");
    this->widgetType = widgetType;
    connectSlot();
    init();
}

RenrenStatusWidget::~RenrenStatusWidget()
{
    delete ui;
}

void RenrenStatusWidget::sendButtonClicked()
{
    ui->toolButton_Send->setDisabled(true);
    QString content = ui->textEdit_Statuscontent->toPlainText();

    if (widgetType == 0)
    {
        //发新状态(forward_id,forward_owner)为0
        emit sendStatus(content,"","","","",0);
    }

    else if (widgetType == 1)
    {
        //转发状态(forward_id,forward_owner)不为0
        emit sendStatus(content,forward_id,forward_owner,"","",1);
    }
    else if (widgetType == 2)
    {
        //回复状态
        emit sendStatus(content,status_id,owner_id,reply_id,"",2);
    }
    else if (widgetType == 3)
    {
        //评论日志
        emit sendStatus(content,log_id,owner_id,reply_id,"",3);
    }
    else if (widgetType == 4)
    {
        //评论分享
        emit sendStatus(content,share_id,owner_id,toUser_id,"",4);
    }
    else if (widgetType == 5)
    {
        //评论照片
        emit sendStatus(content,album_id,photo_id,owner_id,reply_id,5);
    }
    else if (widgetType == 6)
    {
        //分享
        emit sendStatus(content,share_Type,ugc_id,owner_id,share_Url,6);
    }
    else
    {
        QMessageBox::information(this,"widgetType错误","检查widgetType值",QMessageBox::Yes);
    }

    this->hide();
}

void RenrenStatusWidget::atButtonClicked()
{
    ui->textEdit_Statuscontent->insertPlainText("@");
}

void RenrenStatusWidget::emotionButtonClicked()
{
    if(this->geometry().height() != 480)
    {
        ui->groupBox_Emotions->setVisible(true);
        //ui->listWidget_Emotions->addItem("默认");
        QObject::connect(ui->listWidget_Emotions,SIGNAL(clicked(QModelIndex)), this, SLOT(setHtmlByRow(QModelIndex)));
        this->resize(620,480);
    }
    else
    {
        this->resize(620,235);
    }
}

void RenrenStatusWidget::cancelButtonClicked()
{
    this->hide();
}

void RenrenStatusWidget::connectSlot()
{
    QObject::connect(ui->toolButton_Send,SIGNAL(clicked()),
                     this,SLOT(sendButtonClicked()));
    QObject::connect(ui->toolButton_Cancel,SIGNAL(clicked()),
                     this,SLOT(cancelButtonClicked()));
    QObject::connect(ui->toolButton_Mention,SIGNAL(clicked()),
                     this,SLOT(atButtonClicked()));
    QObject::connect(ui->toolButton_Emotion,SIGNAL(clicked()),
                     this,SLOT(emotionButtonClicked()));
    QObject::connect(ui->webView_Emotions->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()),
                     this, SLOT(addJavaScriptObject()));
    QObject::connect(this->dealRenrenEvent,SIGNAL(addEmotionSignal(QString)),
                     this,SLOT(addEmotionSlot(QString)));


}

void RenrenStatusWidget::setHtml(int )
{
    QFile basicHtmlFile(":/other/res/renren/RenrenEmo.html");
    if(!basicHtmlFile.open(QFile::ReadOnly))
    {
        qDebug(basicHtmlFile.errorString().toAscii());
    }
    QTextStream stream(&basicHtmlFile);
    QString basicHtml = stream.readAll();
    basicHtmlFile.close();

    EmotionApi *pEmotion = new EmotionApi;

    pEmotion->deleteLater();

    QMap<QString,QString> mEmotions = pEmotion->mEmotions;
    QList<QString> listEmotions = mEmotions.values();
    QList<QString> keyEmotions = mEmotions.keys();

    QString frameString = "<table>%1</table>";

    int rowNum = (listEmotions.length()%10)==0 ? listEmotions.length()/10 : listEmotions.length()/10+1;

    QString tmp;
    for( int i=0; i<rowNum; i++)
    {
        tmp.append("<tr>");
        for(int j=0; j<10&&(i*10+j)<listEmotions.length(); j++)
        {
                tmp.append("<td><div class=image onclick=addEmotion(\""
                           +keyEmotions.at(i*10+j)
                           +"\")>"
                           +listEmotions.at(i*10+j)
                           +"</div></td>");
        }
        tmp.append("</tr>");
    }

    QString tmpStr = frameString.arg(tmp);
    /*
    qDebug(tmpStr.toLocal8Bit());
    */
    QString all = basicHtml.arg(tmpStr);

    ui->webView_Emotions->setHtml(all);
}

void RenrenStatusWidget::setHtmlByRow(QModelIndex modelIndex)
{
    QWebSettings::clearIconDatabase();
    QWebSettings::clearMemoryCaches();
    setHtml(modelIndex.row());
}

void RenrenStatusWidget::addJavaScriptObject()
{
    ui->webView_Emotions->page()->mainFrame()
            ->addToJavaScriptWindowObject("dealRenrenEvent",this->dealRenrenEvent);
}

void RenrenStatusWidget::addEmotionSlot(QString str)
{
    ui->textEdit_Statuscontent->insertPlainText(str);
}

void RenrenStatusWidget::setForward_id(QString str)
{
    this->forward_id = str;
}

void RenrenStatusWidget::setForward_owner(QString str)
{
    this->forward_owner = str;
}

void RenrenStatusWidget::setStatus_id(QString str)
{
    this->status_id = str;
}

void RenrenStatusWidget::setOwner_id(QString str)
{
    this->owner_id = str;
}

void RenrenStatusWidget::setReply_id(QString str)
{
    this->reply_id = str;
}

void RenrenStatusWidget::setWidgetType(int iVal)
{
    if (iVal >= 0 && iVal <= 10)
    {
        this->widgetType = iVal;
        if(widgetType == 0)
            ui->title_label->setText("发新状态");
        if (widgetType == 1)
        {
            ui->title_label->setText("转发状态");
        }
        else if (widgetType == 2)
        {
            ui->title_label->setText("回复");
        }
        else if (widgetType == 3 || widgetType == 4 || widgetType == 5 || widgetType == 6)
            ui->title_label->setText("评论");
    }
    else
        qDebug("WidgetType Error!!!");
}

void RenrenStatusWidget::setAlbum_id(QString str)
{
    this->album_id = str;
}

void RenrenStatusWidget::setPhoto_id(QString str)
{
    this->photo_id = str;
}

void RenrenStatusWidget::setShare_id(QString str)
{
    this->share_id = str;
}

void RenrenStatusWidget::setToUser_id(QString str)
{
    this->toUser_id = str;
}

void RenrenStatusWidget::setLog_id(QString str)
{
    this->log_id = str;
}

void RenrenStatusWidget::setUgc_id(QString str)
{
    this->ugc_id = str;
}

void RenrenStatusWidget::setShare_Url(QString str)
{
    this->share_Url = str;
}

void RenrenStatusWidget::setShare_Type(QString str)
{
    this->share_Type = str;
}

void RenrenStatusWidget::clearContent()
{
    ui->textEdit_Statuscontent->setText("");
    ui->toolButton_Send->setEnabled(true);
}

void RenrenStatusWidget::setContent(QString str)
{
    ui->textEdit_Statuscontent->setText(str);
}

void RenrenStatusWidget::init()
{
    this->setAutoFillBackground(true);
    QPalette palette;
    this->setBackgroundRole(QPalette::Window);
    palette.setBrush(this->backgroundRole(), QBrush(QColor(Qt::black)));
    this->setPalette(palette);
}




