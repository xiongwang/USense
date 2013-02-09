#include "statuswidget.h"
#include "ui_statuswidget.h"
#include "account.h"
#include "sinaweibo.h"
#include "sinaservices.h"
#include <QTextCursor>
#include <QFileDialog>
#include <QtWebKit>
#include <QMessageBox>
StatusWidget::StatusWidget(Account *account,QWidget* parent) :
    ui(new Ui::StatusWidget),QWidget(parent)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    ui->groupBox_Emotions->hide();
    ui->frame_ShowPic->hide();
    this->dealWebkitEvent = new DealWebkitEvent(account,this);
    this->setGeometry(80,80,620,235);
    this->account = account;
    this->dialogType=0;
    this->wordsLimit=140;
    ui->label_Wordscount->setText(QString::number(wordsLimit));
    ui->toolButton_Send->setEnabled(false);
    ui->title_label->setText("发新微博");
    init();
/*

    QObject::connect(ui->toolButton_Emotion, SIGNAL(clicked()), this, SLOT(addEmotionClicked()));
    QObject::connect(ui->toolButton_Send,SIGNAL(clicked()), this, SLOT(sendButtonClicked()));
    QObject::connect(ui->toolButton_Cancel,SIGNAL(clicked()),this,SLOT(close()));
    QObject::connect(ui->toolButton_AddTopic,SIGNAL(clicked()), this, SLOT(trendsButtonClicked()));
    QObject::connect(ui->toolButton_CancelPic, SIGNAL(clicked()), this, SLOT(deletePicture()));
    QObject::connect(ui->textEdit_Statuscontent,SIGNAL(textChanged()),this,SLOT(wordsCount()));
    QObject::connect(ui->webView_Emotions->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()),
                     this, SLOT(addJavaScriptObject()));
    QPalette palette;
    this->setBackgroundRole(QPalette::Window);
    palette.setBrush(this->backgroundRole(),QBrush(QColor(Qt::black)));
    this->setPalette(palette);
    */
    connectSlot();

}

StatusWidget::StatusWidget(Account *account, QString &retwId, QString &appendText,QWidget* parent) :
        ui(new Ui::StatusWidget),QWidget(parent)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    ui->groupBox_Emotions->hide();
    ui->frame_ShowPic->hide();
    this->dealWebkitEvent = new DealWebkitEvent(account,this);
    this->setGeometry(80,80,620,235);
    this->account = account;
    dialogType = 1;
    wordsLimit = 140;
    id = retwId;
    ui->label_Wordscount->setText(QString::number(wordsLimit));
    setWindowTitle(tr("转发微博"));
    ui->toolButton_AddTopic->setEnabled(false);
    ui->toolButton_AddPic->setEnabled(false);// can't upload picture
    ui->title_label->setText("转发微博");
    /*
    QObject::connect(ui->toolButton_Emotion, SIGNAL(clicked()), this, SLOT(addEmotionClicked()));
    QObject::connect(ui->toolButton_Send, SIGNAL(clicked()), this, SLOT(sendButtonClicked()));
    QObject::connect(ui->toolButton_Cancel,SIGNAL(clicked()),this,SLOT(cancelButtonClicked()));
    QObject::connect(ui->toolButton_AddTopic, SIGNAL(clicked()), this, SLOT(trendsButtonClicked()));
    QObject::connect(ui->textEdit_Statuscontent,SIGNAL(textChanged()), this, SLOT(wordsCount()));
    QObject::connect(ui->webView_Emotions->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()),
                     this, SLOT(addJavaScriptObject()));
                     */
    connectSlot();
    init();
    ui->textEdit_Statuscontent->append(appendText);
}

StatusWidget::StatusWidget(Account *account, QString &commentStatusId,QWidget* parent) :
        ui(new Ui::StatusWidget),QWidget(parent)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    ui->groupBox_Emotions->hide();
    ui->frame_ShowPic->hide();
    this->dealWebkitEvent = new DealWebkitEvent(account, this);
    this->setGeometry(80,80,620,235);
    this->account = account;
    dialogType = 2; /* comment */
    wordsLimit = 140;
    id = commentStatusId;
    ui->label_Wordscount->setText(QString::number(wordsLimit));
    setWindowTitle(tr("评论"));
    ui->title_label->setText("评论微博");
    ui->toolButton_AddPic->setEnabled(false);
    /*
    QObject::connect(ui->toolButton_Emotion, SIGNAL(clicked()), this, SLOT(addEmotionClicked()));
    QObject::connect(ui->toolButton_Send, SIGNAL(clicked()), this, SLOT(sendButtonClicked()));
    QObject::connect(ui->toolButton_Cancel,SIGNAL(clicked()),this,SLOT(cancelButtonClicked()));
    QObject::connect(ui->toolButton_AddTopic, SIGNAL(clicked()), this, SLOT(trendsButtonClicked()));
    QObject::connect(ui->textEdit_Statuscontent, SIGNAL(textChanged()), this, SLOT(wordsCount()));
    QObject::connect(ui->webView_Emotions->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()),
                     this, SLOT(addJavaScriptObject()));
                     */
    connectSlot();
    init();

}

StatusWidget::StatusWidget(Account *account, QString &cid, QString &id, int dialogType,QWidget* parent) :
        ui(new Ui::StatusWidget),QWidget(parent)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    ui->groupBox_Emotions->hide();
    ui->frame_ShowPic->hide();
    this->dealWebkitEvent = new DealWebkitEvent(account, this);
    this->setGeometry(80,80,620,235);
    this->account = account;

    if(dialogType!=3)// useless
        this->dialogType = 3;

    this->dialogType = 3;
    wordsLimit = 140;
    this->id = id;
    this->cid = cid;
    ui->label_Wordscount->setText(QString::number(wordsLimit));
    setWindowTitle(tr("评论"));
    ui->toolButton_AddPic->setEnabled(false);
    /*
    QObject::connect(ui->toolButton_Emotion, SIGNAL(clicked()), this, SLOT(addEmotionClicked()));
    QObject::connect(ui->toolButton_Send, SIGNAL(clicked()), this, SLOT(sendButtonClicked()));
    QObject::connect(ui->toolButton_Cancel,SIGNAL(clicked()),this,SLOT(cancelButtonClicked()));
    QObject::connect(ui->toolButton_AddTopic, SIGNAL(clicked()), this, SLOT(trendsButtonClicked()));
    QObject::connect(ui->textEdit_Statuscontent, SIGNAL(textChanged()), this, SLOT(wordsCount()));
    QObject::connect(ui->webView_Emotions->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()),
                     this, SLOT(addJavaScriptObject()));
                     */
    connectSlot();
    init();
}

StatusWidget::StatusWidget(Account *account, QString uid, int dialogType,QWidget* parent) :
        ui(new Ui::StatusWidget),QWidget(parent)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    ui->groupBox_Emotions->hide();
    ui->frame_ShowPic->hide();
    this->dealWebkitEvent = new DealWebkitEvent(account, this);
    this->setGeometry(80,80,620,235);
    this->account = account;
    if(dialogType!=4)// useless
        this->dialogType = 4;
    this->dialogType = 4;
    wordsLimit = 300;
    this->id = uid;
    ui->label_Wordscount->setText(QString::number(wordsLimit));
    setWindowTitle(tr("评论"));
    ui->title_label->setText("回复评论");
    ui->toolButton_AddPic->setEnabled(false);
    /*
    QObject::connect(ui->toolButton_Emotion, SIGNAL(clicked()), this, SLOT(addEmotionClicked()));
    QObject::connect(ui->toolButton_Send, SIGNAL(clicked()), this, SLOT(sendButtonClicked()));
    QObject::connect(ui->toolButton_Cancel,SIGNAL(clicked()),this,SLOT(cancelButtonClicked()));
    QObject::connect(ui->toolButton_AddTopic, SIGNAL(clicked()), this, SLOT(trendsButtonClicked()));
    QObject::connect(ui->textEdit_Statuscontent, SIGNAL(textChanged()), this, SLOT(wordsCount()));
    QObject::connect(ui->webView_Emotions->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()),
                     this, SLOT(addJavaScriptObject()));
                     */
    connectSlot();
    init();
}

StatusWidget::~StatusWidget()
{
    delete ui;
    qDebug(" deleted !");
}

void StatusWidget::sendButtonClicked()
{
    ui->toolButton_Send->setEnabled(false);
    ui->toolButton_Send->setText("发表中...");
    ui->webView_Emotions->settings()->clearMemoryCaches();
    if(dialogType==0)
    {
        QString content = ui->textEdit_Statuscontent->document()->toPlainText().toUtf8();

        if(!this->pictureInByteArray.isEmpty()&&!this->pictureInByteArray.isNull())
            sendStatus(content, this->pictureInByteArray, this->pictureFileName);
        else
            sendStatus(content);
    }
    else if(dialogType==1)
       repost(id, ui->textEdit_Statuscontent->document()->toPlainText());
    else if(dialogType==2)
        commentStatus(id, ui->textEdit_Statuscontent->document()->toPlainText());
    else if(dialogType==3)
        this->replyComment(cid, id, ui->textEdit_Statuscontent->document()->toPlainText());
    else if(dialogType==4)
        this->sendDirectMessage(id, ui->textEdit_Statuscontent->document()->toPlainText());
//    this->accept();
    this->hide();

}

void StatusWidget::cancelButtonClicked()
{
    this->close();
}

void StatusWidget::sendStatus(QString statusText)
{
    SinaWeibo *sina = new SinaWeibo(this->account);
    int errorCode = sina->sendStatusWithoutPicture(statusText);
    if(errorCode == QNetworkReply::NoError)
        QMessageBox::information(NULL, tr("成功"), tr("发表微博成功!"), QMessageBox::Ok);
    else
    {
        QMessageBox::information(NULL, tr("错误"), tr("发表微博失败!"), QMessageBox::Ok);
        qDebug("Error code is %d", errorCode);
    }
    delete sina;
    //this->accept();
        this->hide();

}

void StatusWidget::sendStatus(QString statusText, QByteArray pictureDataInByteArray, QString fileName)
{
    SinaWeibo *sina = new SinaWeibo(this->account);
    int errorCode = sina->sendStatusWithPicture(statusText, pictureDataInByteArray, fileName);
    if(errorCode == QNetworkReply::NoError)
        QMessageBox::information(NULL, tr("成功"), tr("发表微博成功!"), QMessageBox::Ok);
    else
    {
        QMessageBox::information(NULL, tr("错误"), tr("发表微博失败!"), QMessageBox::Ok);
        qDebug("Error code is %d", errorCode);
    }
    delete sina;
    //this->accept();
        this->hide();

}

void StatusWidget::repost(QString id, QString text)
{
    SinaWeibo *sina = new SinaWeibo(this->account);
    int errorCode = sina->repost(id, text);
    if(errorCode == QNetworkReply::NoError)
        QMessageBox::information(NULL, tr("成功"), tr("转发微博成功!"), QMessageBox::Ok);
    else
    {
        QMessageBox::information(NULL, tr("错误"), tr("转发微博失败!"), QMessageBox::Ok);
        qDebug("Error code is %d", errorCode);
    }
    delete sina;
    //this->accept();
        this->hide();

}

void StatusWidget::commentStatus(QString id, QString text)
{
    SinaWeibo *sina = new SinaWeibo(account);
    int errorCode = sina->commentStatus(id, text);
    if(errorCode == QNetworkReply::NoError)
        QMessageBox::information(NULL, tr("成功"), tr("评论发表成功!"), QMessageBox::Ok);
    else
        QMessageBox::information(NULL, tr("错误"), tr("评论发表失败!"), QMessageBox::Ok);
    qDebug("Error code is %d", errorCode);
    delete sina;
    //this->accept();
        this->hide();

}

void StatusWidget::replyComment(QString cid, QString id, QString text)
{
    SinaWeibo *sina = new SinaWeibo(account);
    int errorCode = sina->replyComment(cid, id, text);
    if(errorCode == QNetworkReply::NoError)
        QMessageBox::information(NULL, tr("成功"), tr("回复成功!"), QMessageBox::Ok);
    else
    {
        QMessageBox::information(NULL, tr("错误"), tr("回复失败!"), QMessageBox::Ok);
        qDebug("Error code is %d", errorCode);
    }
    delete sina;
    //this->accept();
        this->hide();

}

void StatusWidget::addPictureButtonClicked()
{
    QString picturePath;
    picturePath = QFileDialog::getOpenFileName(
            this,
            tr("请选择图片"),
            ".",
            tr("Image Files(*.jpg *.png *.gif)")
            );
    if(picturePath.length()!=0)
    {
        ui->textEdit_Statuscontent->insertPlainText("分享图片");
        QFile pictureFile(picturePath);
        if(!pictureFile.open(QFile::ReadOnly))
        {
            //can't open the file
            QMessageBox::information(this, "无法打开图片", "无法打开图片!", QMessageBox::Ok, QMessageBox::Ok);
        }
        else
        {
            if(pictureFile.size()>5000000)//5Mb
            {
                QMessageBox::information(this, "图片太大", "图片不能大于5MB", QMessageBox::Ok, QMessageBox::Ok);
            }
            else
            {
                this->pictureInByteArray = pictureFile.readAll();
                this->pictureFileName = pictureFile.fileName();
                ui->frame_ShowPic->setMinimumHeight(75);
                ui->frame_ShowPic->setVisible(true);
                QPixmap pic;
                pic.loadFromData(this->pictureInByteArray);
                /*
                ui->frame_ShowPic->setPixmap(pic.scaled(75,75));
                */
            }
        }
    }
}

void StatusWidget::deletePicture()
{
    this->pictureInByteArray.clear();
    this->pictureFileName.clear();
    ui->frame_ShowPic->setMinimumHeight(0);
    ui->frame_ShowPic->hide();
    this->adjustSize();
}

void StatusWidget::trendsButtonClicked()
{
    QTextCursor currentTextCursor = ui->textEdit_Statuscontent->textCursor();
    QString topicHelp = tr("在这里插入话题");
    currentTextCursor.insertText("#"+topicHelp+"#");
    currentTextCursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, 1);//unselect right #
    int pos = currentTextCursor.position();
    currentTextCursor.setPosition(pos - topicHelp.length(), QTextCursor::KeepAnchor );
    ui->textEdit_Statuscontent->setTextCursor(currentTextCursor);
}

void StatusWidget::wordsCount()
{
    QString content = ui->textEdit_Statuscontent->document()->toPlainText();
    int length = content.length();
    if(length>0&&length<=wordsLimit)
        ui->toolButton_Send->setEnabled(true);
    else
        ui->toolButton_Send->setEnabled(false);
    ui->label_Wordscount->setText(QString::number(wordsLimit - content.length()));
}

void StatusWidget::addEmotionClicked()
{
    if(this->geometry().height() != 480)
    {
        this->resize(620,480);
        ui->groupBox_Emotions->setVisible(true);
        /*
        ui->listWidget_Emotions->addItem("默认");
        SinaServices *sina = SinaServices::getInstance();
        for(int i=0; i<sina->emotionCategoryList.length(); i++)
        {
            ui->listWidget_Emotions->addItem(sina->emotionCategoryList.at(i));
        }
        */
        QObject::connect(ui->listWidget_Emotions,SIGNAL(clicked(QModelIndex)), this, SLOT(setHtmlByRow(QModelIndex)));
        QObject::connect(dealWebkitEvent, SIGNAL(insertPhaseText(QString)), this, SLOT(insertPhaseText(QString)));
        setHtml(0);
    }
    else
    {
        this->resize(620,235);
    }
}

void StatusWidget::setHtmlByRow(QModelIndex modelIndex)
{
    QWebSettings::clearIconDatabase();
    QWebSettings::clearMemoryCaches();
    setHtml(modelIndex.row());
}

void StatusWidget::addJavaScriptObject()
{
    ui->webView_Emotions->page()->mainFrame()
            ->addToJavaScriptWindowObject("dealWebkitEvent",this->dealWebkitEvent);
}

void StatusWidget::setHtml(int row)
{
    QFile basicHtmlFile(":/other/res/sina/SinaEmo.html");
    if(!basicHtmlFile.open(QFile::ReadOnly))
    {
        qDebug(basicHtmlFile.errorString().toAscii());
    }
    QTextStream stream(&basicHtmlFile);
    QString basicHtml = stream.readAll();
    basicHtmlFile.close();

    SinaServices *sina = SinaServices::getInstance();
    QList<Emotion> list;
    if(row==0)
        list = getEmotionListByCategoryName("");
    else
        list = getEmotionListByCategoryName(sina->emotionCategoryList.at(row-1));
    QString emotionString = emotionListToHtml(list);

    QString all = basicHtml.arg(emotionString);

    ui->webView_Emotions->setHtml(all);
}

QList<Emotion> StatusWidget::getEmotionListByCategoryName(QString categoryName)
{
    SinaServices *sina = SinaServices::getInstance();
    QList<Emotion> hot;
    QList<Emotion> notHot;

    for( int i=0; i<sina->emotionList.length(); i++)
    {
        Emotion emotion = sina->emotionList.at(i);
        if(emotion.getCategory()==categoryName)
        {
            if(emotion.isHot())
                hot.append(emotion);
            else
                notHot.append(emotion);
        }
    }
    hot.append(notHot);
    return hot;
}

QString StatusWidget::emotionListToHtml(QList<Emotion> &list)
{
    QString frameString = "<table>%1</table>";
    QString tmp;
    int row = (list.length()%10)==0 ? list.length()/10 : list.length()/10+1;
    for( int i=0; i<row; i++)
    {
        tmp.append("<tr>");
        for(int j=0; j<10&&(i*10+j)<list.length(); j++)
        {
            Emotion emotion = list.at(i*10+j);
            if(emotion.isHot())
                tmp.append("<td><div class=image><img src="+emotion.getUrl()+" title="+emotion.getPhrase()+
                           " href=javascript:void(0) onclick=addEmotion(\""
                           +emotion.getPhrase()+"\") class=hot></img></div></td>");
            else
                tmp.append("<td><div class=image><img src="+emotion.getUrl()+" title="+emotion.getPhrase()+
                           " href=javascript:void(0) onclick=addEmotion(\""
                           +emotion.getPhrase()+"\")></img></div></td>");
        }
        tmp.append("</tr>");
    }
    QString tmpStr = frameString.arg(tmp);
    //qDebug(tmpStr.toLocal8Bit());
    return tmpStr;
}

void StatusWidget::insertPhaseText(QString phase)
{
    ui->textEdit_Statuscontent->insertPlainText(phase);
}

void StatusWidget::sendDirectMessage(QString uid, QString text)
{
    SinaWeibo *sina = new SinaWeibo(account);
    int errorCode = sina->sendDirectMessage(uid, text);
    if(errorCode == QNetworkReply::NoError)
        QMessageBox::information(NULL, tr("成功"), tr("私信发送成功!"), QMessageBox::Ok);
    else
    {
        QMessageBox::information(NULL, tr("失败"), tr("私信发送失败!"), QMessageBox::Ok);
        qDebug("Error code is %d", errorCode);
    }
    delete sina;
    this->hide();

}

void StatusWidget::setText(QString text)
{
    ui->textEdit_Statuscontent->append(text);
}

void StatusWidget::slotInsertString(QString str)
{
}

void StatusWidget::clearContent()
{
    ui->textEdit_Statuscontent->setText("");
}

void StatusWidget::freeSendButton()
{
    ui->toolButton_Send->setEnabled(true);
}

void StatusWidget::closeEvent(QCloseEvent *e)
{
    this->resize(640,235);
//    ui->toolButton_Emotion->setEnabled(true);
}

void StatusWidget::mentionButtonClicked()
{
    ui->textEdit_Statuscontent->append("@");
}

void StatusWidget::connectSlot()
{
    QObject::connect(ui->toolButton_Mention,SIGNAL(clicked()),
                     this,SLOT(mentionButtonClicked()));
    QObject::connect(ui->toolButton_Emotion, SIGNAL(clicked()), this, SLOT(addEmotionClicked()));
    QObject::connect(ui->toolButton_Send,SIGNAL(clicked()), this, SLOT(sendButtonClicked()));
    QObject::connect(ui->toolButton_Cancel,SIGNAL(clicked()),this,SLOT(close()));
    /*
    QObject::connect(ui->toolButton_AddPic, SIGNAL(clicked()), this, SLOT(addPictureButtonClicked()));
    */
    QObject::connect(ui->toolButton_AddTopic,SIGNAL(clicked()), this, SLOT(trendsButtonClicked()));
    QObject::connect(ui->toolButton_CancelPic, SIGNAL(clicked()), this, SLOT(deletePicture()));
    QObject::connect(ui->textEdit_Statuscontent,SIGNAL(textChanged()),this,SLOT(wordsCount()));
    QObject::connect(ui->webView_Emotions->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()),
                     this, SLOT(addJavaScriptObject()));

}

void StatusWidget::setTitle(QString title)
{
    ui->title_label->setText(title);
}

void StatusWidget::init()
{
    this->setAutoFillBackground(true);
    QPalette palette;
    this->setBackgroundRole(QPalette::Window);
    palette.setBrush(this->backgroundRole(), QBrush(QColor(Qt::black)));
    this->setPalette(palette);
}
