#include "loginwidget.h"
#include "ui_loginwidget.h"

using namespace std;

LoginWidget::LoginWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginWidget)
{
    ui->setupUi(this);
    pClientWidget = new ClientWidget;
    pAppWidget = new AppWidget(this);
    currentClient = Sina;
    setAutoFillBackground(true);

    connect(pClientWidget,SIGNAL(loginButtonClickedSignal(QString,QString)),
            this,SLOT(startLogin(QString,QString)));

    ui->layout->addWidget(pClientWidget);


    this->setAutoFillBackground(true);
    QPalette palette;
    QPixmap pixmap(":/images/background.png");

    if (pixmap.isNull())
        qDebug("NULL!!!");

    this->setBackgroundRole(QPalette::Window);
    palette.setBrush(this->backgroundRole(), QBrush(pixmap));
    this->setPalette(palette);

}

LoginWidget::~LoginWidget()
{
    delete ui;
}

void LoginWidget::startLogin(QString userId,QString password){


    if(currentClient == Sina)
    {
        emit startLoginSignal();
        this->pAccount = new Account;
        pAccount->setUsername(userId);
        pAccount->setPassword(password);
        this->pSinaLogin = new SinaLogin(this->pAccount);
        connect(pSinaLogin,SIGNAL(loginSuccessed()),this,SLOT(sinaLoginSuccessed()));
        pSinaLogin->checkAuthStart();
    }
    else if (currentClient == RenRen)
    {
        emit startLoginSignal();
        this->prenrenAccount = new RenRenAccount;
        prenrenAccount->setUsername(userId);
        prenrenAccount->setPassword(password);
        this->pOAuth = new OAuth2(*(this->prenrenAccount));
        connect(this,SIGNAL(CheckRenrenLogin()),this,SLOT(renrenLoginSuccessed()));
        pOAuth->getAccessToken();
        pOAuth->getSessionKey();
        if(pOAuth->getAccount().getAccessToken()!=NULL)
        {
            this->prenrenAccount = &pOAuth->getAccount();
            emit CheckRenrenLogin();

        }
    }

}

Account* LoginWidget::getSinaAccount(){

    return this->pAccount;
}

void LoginWidget::rssRun()
{
    emit sendToMainWidget(Rss);
}

void LoginWidget::sinaLoginSuccessed(){

    emit sendToMainWidget(Sina);
}
void LoginWidget::renrenLoginSuccessed(){

    emit sendToMainWidget(RenRen);
}
RenRenAccount* LoginWidget::getRenRenAccount()
{
    return this->prenrenAccount;
}

void LoginWidget::changeCurrentClient(Client client)
{
    if (currentClient == client)
        return;

    if (client == App)
    {
        if(!ui->layout->isEmpty())
        {
            ui->layout->removeWidget(pClientWidget);
            pClientWidget->hide();
        }
        ui->layout->addWidget(pAppWidget);
        pAppWidget->show();
    }
    else{
        if (!ui->layout->isEmpty())
        {
            ui->layout->removeWidget(pAppWidget);
            pAppWidget->hide();
        }
        if (client == Sina)
            pClientWidget->setLoginPic("Sina");
        else if (client == RenRen)
            pClientWidget->setLoginPic("Renren");

        ui->layout->addWidget(pClientWidget);
        pClientWidget->show();
    }

    currentClient = client;
}
