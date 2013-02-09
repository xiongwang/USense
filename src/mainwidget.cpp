#include "mainwidget.h"
#include "ui_mainwidget.h"
#include "QFileDialog"


MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{

    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    pLoginWidget = NULL;
    pSinaWidget = NULL;
    pRenrenWidget = NULL;
    pRssReader = NULL;
    currentSelected = 0;
    currentClient = None;

    movie.setFileName(":/images/loading.gif");
    /*
    ui->loading_label->setText("哈哈");
    ui->loading_label->show();
    */
    ui->loading_label->hide();
    ui->loading_label->setMovie(&movie);

    sinaPixmap.load(":/images/Sina.png");
    sinaLightedPixmap.load(":/images/SinaLighted.png");

    renrenPixmap.load(":/images/Renren.png");
    renrenLightedPixmap.load(":/images/RenrenLighted.png");

    appPixmap.load(":/images/App.png");
    appLightedPixmap.load(":/images/AppLighted");

    bgBeforeLogin.load(":/images/background.png");
    bgAfterLogin.load(":/images/background2.png");

    setBackGroundPic("BeforeLogin");

    loadClient(Login);

    connect(this,SIGNAL(clicked(Position,int)),this,SLOT(response(Position,int)),Qt::UniqueConnection);
}

MainWidget::~MainWidget()
{
    delete ui;
}


void MainWidget::loadClient(Client client){

    if (client == Login){
        removeCurrentClient();
        if (pLoginWidget == NULL)
        {
            initLoginWidget();
        }
        else
            showLoginWidget();
        if(pLoginWidget->getCurrentClient() == App)
        {
            ui->label_up_3->setPixmap(appLightedPixmap);
        }
    }

    else if (client == Sina && currentClient != Sina){
        removeCurrentClient();
        this->currentSelected = 1;

        if (pSinaWidget == NULL)
            initSinaWidget(pLoginWidget->getSinaAccount());
        else
            showSinaWidget();
        ui->label_up_1->setPixmap(sinaLightedPixmap);
    }

    else if (client == Rss && currentClient != Rss)
    {

        if (pRssReader == NULL)
            pRssReader = new RssReader(this);
        removeCurrentClient();

        QPixmap pixmap(":/images/rss/rsslogo.png");

        if (pixmap.isNull())
            qDebug("NULL!!!");
        ui->layout->addWidget(pRssReader);
        pRssReader->show();
        currentClient = Rss;
    }
    else if (client == RenRen && currentClient != RenRen)
    {

        this->currentSelected = 2;
        removeCurrentClient();
        if (pRenrenWidget == NULL)
            initRenrenWidget(*pLoginWidget->getRenRenAccount());
        else
            showRenrenWidget();
        ui->label_up_2->setPixmap(renrenLightedPixmap);
    }
    else
        return ;
}

void MainWidget::mousePressEvent(QMouseEvent *pEvent){

    int x = pEvent->x();
    int y = pEvent->y();
/*
    int reminderOfX = x % widget_width;
    int reminderOfY;
    if (y > 80)
        reminderOfY = (y - 80) % widget_height;
    else
        reminderOfY = 0;

    if ( (reminderOfX > distanceInX && reminderOfX < widget_widthInX - distanceInX)
            &&(reminderOfY > distanceInY && reminderOfY < widget_heightInY - distanceInY)
            &&( x / widget_widthInX < 1 || (y - 80) / widget_heightInY + 1 < 1)
            &&  y / widget_heightInY < 9
            &&  x / widget_widthInX  < 16
            )*/
    if ( (x < widget_widthInY || y < widget_heightInX)
         && (y - widget_heightInX) / widget_heightInY + 1 < 9
         && (x / widget_widthInX < 12))
    {
        if ( x < widget_widthInY && currentClient != None){
            if (y > widget_heightInX)
            {
                //qDebug("你点击了左边的第"+toQByteArray((y - widget_heightInX) / widget_heightInY + 1)+"个Label");
                emit clicked(Left,(y - widget_heightInX) / widget_heightInY + 1);
            }
        }
        else if ( y < widget_heightInX){
            //qDebug("你点击了上边的第"+toQByteArray(x / widget_widthInX)+"个Label");
                emit clicked(Up,x / widget_widthInX);
        }
    }
    if (y < widget_widthInX && this->width() - widget_widthInX < x )
        emit clicked(Up,12);
}

void MainWidget::response(Position pos, int index){


    if ( pos == Left ){
        if ( index >0 && index < 9){
            if (currentClient == Sina)
                emit sendSignalToSina(index);
            else if (currentClient == RenRen)
                emit sendSignalToRenren(index);
        }
        else {
            qDebug("pos = left ,index value Error!!!");
            return ;
        }
    }
    else if ( pos == Up){
        switch ( index ){
        case 1:
        {
            Client client = Sina;

                    if (pSinaWidget == NULL)
                    {
                        pLoginWidget->changeCurrentClient(Sina);
                        loadClient(Login);
                        return;
                    }
                    else
                    {
                        loadClient(client);
                    }
        }

            break;
        case 2:
        {
            Client client = RenRen;

                    if (pRenrenWidget == NULL)
                    {
                        pLoginWidget->changeCurrentClient(RenRen);
                        loadClient(Login);
                        return;
                    }
                    else
                    {
                        loadClient(client);
                    }
        }
            break;
        case 3:
            //AppWidget
            pLoginWidget->changeCurrentClient(App);
            loadClient(Login);
            break;
        case 4:

            break;
        case 5:

            break;
        case 6:

            break;
        case 7:

            break;
        case 8:

            break;
        case 9:

            break;
        case 10:

            break;
        case 11:

            break;
        case 12:
            exit(0);
            break;
        case 13:

            break;
        case 14:

            break;
        case 15:

            break;
        default:
            break;
        }
    }
    else{
        qDebug("In response(Position pos, int index):Position Error!!!");
        return ;
    }
}

QByteArray MainWidget::toQByteArray(int num){
    return QString::number(num).toLocal8Bit();
}

void MainWidget::removeCurrentClient(){

    setBackGroundPic("BeforeLogin");

    switch (currentClient){

    case None:
        break;
    case Login:
        if (!ui->layout->isEmpty()){
            ui->layout->removeWidget(pLoginWidget);
            pLoginWidget->hide();
            if (ui->label_up_3->pixmap()!= NULL
                    &&(ui->label_up_3->pixmap()->cacheKey() == appLightedPixmap.cacheKey()) )
                ui->label_up_3->setPixmap(appPixmap);
        }

        break;
    case Sina:
        if (!ui->layout->isEmpty()){
            ui->layout->removeWidget(pSinaWidget);
            pSinaWidget->hide();
            ui->label_up_1->setPixmap(sinaPixmap);
        }
    case RenRen:
        if (!ui->layout->isEmpty()){
            ui->layout->removeWidget(pRenrenWidget);
            pRenrenWidget->hide();
            ui->label_up_2->setPixmap(renrenPixmap);
        }
    case Rss:
        break;
    default:
        break;
    }
}

void MainWidget::loginSuccessed(Client client){

    if (client == Sina)
    {
        loadClient(Sina);

        return ;
    }
    if (client == Rss)
    {
        loadClient(Rss);

        return ;
    }
    if(client == RenRen)
    {
        loadClient(RenRen);

        return ;
    }
}

void MainWidget::forwardToOtherClientSlot(QString str)
{
    qDebug("in MainWidget:"+str.toLocal8Bit());

    if (pSinaWidget == NULL || pRenrenWidget == NULL)
    {
        //QMessageBox::information(NULL,tr("转发错误"),tr("不存在其它客户端"),QMessageBox::Yes);
        //showErrorMessage("转发错误:不存在其它客户端");
    }
    else
    {
        emit forward_to_otheClientSignal(str);
    }
}

void MainWidget::closeEvent(QCloseEvent *e)
{
    if (pSinaWidget != NULL)
        pSinaWidget->close();
    exit(0);
}

void MainWidget::initSinaWidget(Account* pAccount)
{
    SinaWeibo *pSinaWeibo = new SinaWeibo(pAccount);
    pSinaWeibo->initHomePageStatus();
    pSinaWeibo->initEmotions();
    delete pSinaWeibo;
    pSinaWidget = new SinaWidget(pAccount,this);

    connect(pSinaWidget->leftWebView,SIGNAL(loadFinished(bool)),
            this,SLOT(loadFinishedSlot(bool)));
    connect(pSinaWidget->leftWebView,SIGNAL(loadStarted()),
            this,SLOT(loadStartedSlot()));
    connect(pSinaWidget->rightWebView,SIGNAL(loadFinished(bool)),
            this,SLOT(loadFinishedSlot(bool)));
    connect(pSinaWidget->rightWebView,SIGNAL(loadStarted()),
            this,SLOT(loadStartedSlot()));

    QPixmap headPhoto = pSinaWidget->getPixmapFromUrl(pAccount->getProfileImageUrl());
    if (headPhoto.isNull())
        qDebug("headPhoto.isNull()");
    sinaPhoto = headPhoto.scaled(60,60);

    showSinaWidget();
}

void MainWidget::initRenrenWidget(RenRenAccount renrenAccount)
{
    pRenrenWidget = new RenrenWidget();
    pRenrenWidget->prenrenAccount = new RenRenAccount;
    pRenrenWidget->prenrenAccount = &renrenAccount;
    pRenrenWidget->pFeedApi->account = renrenAccount;
    pRenrenWidget->pGetFriendsApi->account = renrenAccount;
    pRenrenWidget->pStatusApi->account = renrenAccount;
    pRenrenWidget->pUserApi->account = renrenAccount;
    pRenrenWidget->pBlogApi->account = renrenAccount;
    pRenrenWidget->pPhotoApi->account = renrenAccount;
    pRenrenWidget->pShareApi->account = renrenAccount;
    connect(pRenrenWidget->leftWebView,SIGNAL(loadFinished(bool)),
            this,SLOT(loadFinishedSlot(bool)));
    connect(pRenrenWidget->leftWebView,SIGNAL(loadStarted()),
            this,SLOT(loadStartedSlot()));
    connect(pRenrenWidget->rightWebView,SIGNAL(loadFinished(bool)),
            this,SLOT(loadFinishedSlot(bool)));
    connect(pRenrenWidget->rightWebView,SIGNAL(loadStarted()),
            this,SLOT(loadStartedSlot()));
    pRenrenWidget->getFeed();
    pRenrenWidget->getFriends();
    showRenrenWidget();

}

void MainWidget::initLoginWidget()
{
    pLoginWidget = new LoginWidget(this);
    connect(pLoginWidget,SIGNAL(sendToMainWidget(Client)),
            this,SLOT(loginSuccessed(Client)),
            Qt::UniqueConnection);
    connect(pLoginWidget,SIGNAL(startLoginSignal()),
            this,SLOT(loadStartedSlot()));
    showLoginWidget();
}

void MainWidget::showLoginWidget()
{
    ui->photoLabel->setStyleSheet("");
    ui->photoLabel->setPixmap(QPixmap(""));
    ui->layout->addWidget(pLoginWidget);
    pLoginWidget->show();
    currentClient = Login;
}

void MainWidget::showSinaWidget()
{
    setBackGroundPic("AfterLogin");

    ui->photoLabel->setStyleSheet("QWidget#photoLabel{border:3px solid white;border-radius:5px;}");
    ui->photoLabel->setPixmap(sinaPhoto);
    ui->layout->addWidget(pSinaWidget);
    pSinaWidget->show();

    currentClient = Sina;

    connect(this,SIGNAL(sendSignalToSina(int)),pSinaWidget,
            SLOT(receiveSignalFromMainWidget(int)));

    connect(pSinaWidget,SIGNAL(forwardToOtherClientSignal(QString)),
            this,SLOT(forwardToOtherClientSlot(QString)),Qt::UniqueConnection);

    if (pRenrenWidget != NULL){

        if (false == disconnect(this,SIGNAL(forward_to_otheClientSignal(QString)),
                           pSinaWidget,SLOT(forward_from_otherClient(QString))))
        {
            qDebug("Client = Sina : disconnect failed");
        }
        connect(this,SIGNAL(forward_to_otheClientSignal(QString)),
                pRenrenWidget,SLOT(forward_from_otherClient(QString)),Qt::UniqueConnection);
    }
}

void MainWidget::showRenrenWidget()
{
    setBackGroundPic("AfterLogin");
    QPixmap headPhoto = pRenrenWidget->getPixmap();
    currentClient = RenRen;
    if (renrenPhoto.isNull())
            renrenPhoto = headPhoto.scaled(60,60);
    ui->photoLabel->setStyleSheet("QWidget#photoLabel{border:3px solid white;border-radius:5px;}");
    ui->photoLabel->setPixmap(renrenPhoto);

    ui->layout->addWidget(pRenrenWidget);
    pRenrenWidget->show();
    if (pSinaWidget != NULL)
    {
        if ( false == disconnect(this,SIGNAL(forward_to_otheClientSignal(QString))
                   ,pRenrenWidget,SLOT(forward_from_otherClient(QString))) )
        {
            qDebug("Client = RenRen : disconnect failed");
        }
        connect(this,SIGNAL(forward_to_otheClientSignal(QString)),
                pSinaWidget,SLOT(forward_from_otherClient(QString)),Qt::UniqueConnection);
    }

    connect(pRenrenWidget,SIGNAL(forwardToOtherClientSignal(QString)),
            this,SLOT(forwardToOtherClientSlot(QString)),Qt::UniqueConnection);

    connect(this,SIGNAL(sendSignalToRenren(int)),pRenrenWidget,SLOT(receiveSignalFromMainWidget(int)),Qt::UniqueConnection);
}

void MainWidget::setBackGroundPic(QString type)
{
    QPixmap pixmap;

    if (type == "BeforeLogin")
    {
        pixmap = bgBeforeLogin;
    }
    else if (type == "AfterLogin")
    {
        pixmap = bgAfterLogin;
    }
    else
    {
        return;
    }

    palette.setBrush(this->backgroundRole(), QBrush(pixmap));

    this->setPalette(palette);
}

void MainWidget::loadStartedSlot()
{
    if (currentSelected == 1)
        emit clicked(Up,1);
    else if (currentSelected == 2)
        emit clicked(Up,2);
    movieStart();
}

void MainWidget::loadFinishedSlot(bool isFinished)
{
    if (isFinished)
    {
        movieFinished(isFinished);
    }
}

void MainWidget::movieStart()
{
    ui->loading_label->show();
    movie.start();
}

void MainWidget::movieFinished(bool isFinished)
{
    if (isFinished)
    {
        ui->loading_label->hide();
    }
}

void MainWidget::showErrorMessage(QString errorString)
{
    /*
    ui->error_label->setText(errorString);
    ui->error_label->show();
    //ui->error_label->setScaledContents(true);
    QTimer timer;
    timer.setSingleShot(true);
    connect(&timer,SIGNAL(timeout()),this,SLOT(hideErrorMessage()));
    timer.start(1000);
    */
}

void MainWidget::hideErrorMessage()
{
    //ui->error_label->hide();
}

void MainWidget::hideLostFocusWidget(QWidget *old, QWidget *now)
{
    if (old != NULL)
    {

        old->hide();
    }
    if (old != NULL)
    {

        now->show();
    }
}
