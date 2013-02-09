#include "clientwidget.h"
#include "ui_clientwidget.h"

ClientWidget::ClientWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClientWidget)
{
    ui->setupUi(this);
    connect(ui->pushButton_Login,SIGNAL(clicked()),
            this,SLOT(loginButtonClicked()));

    this->setAutoFillBackground(true);
    this->setBackgroundRole(QPalette::Window);
    sinaLogin.load(":/images/sina/sinalogin.png");
    renrenLogin.load(":/images/renren/renrenlogin.png");
    this->setLoginPic("Sina");

}

ClientWidget::~ClientWidget()
{
    delete ui;
}

void ClientWidget::loginButtonClicked()
{
    emit loginButtonClickedSignal(ui->lineEdit_Username->text(),ui->lineEdit_Password->text());
}

void ClientWidget::setLoginPic(QString clientName)
{
    QPixmap pixmap;

    if (clientName == "Sina")
        pixmap = sinaLogin;
    else if (clientName == "Renren")
        pixmap = renrenLogin;
    palette.setBrush(this->backgroundRole(), QBrush(pixmap));
    this->setPalette(palette);
}
