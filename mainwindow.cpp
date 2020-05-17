#include "mainwindow.h"
#include <QString>
#include <sstream>
#include <QDebug>
#include <QFile>
#include <QFont>
#include <QPainter>
#include <QColor>
#include <QAction>
#include <QProgressDialog>
#include <QFileDialog>
#include <QCoreApplication>
#include <QtCore>
#include <QFontDatabase>
#include <QApplication>

using namespace std;

const int BOXSIZE=60;
const int MENUSIZE=23;
const int PANELX=4*BOXSIZE;
const int PANELY=MENUSIZE;
const char title[]="推箱子";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , wallPixmap(":/image/wall.png")
    , aimPixmap(":/image/aim.png")
    , manPixmap(":/image/Player.png")
    , manOnAimPixmap(":/image/Player.png")
    , boxPixmap(":/image/box.png")
    , boxOnAimPixmap(":/image/box.png")
    , goldPixmap(":/image/coin.png")
    , YellowwallPixmap(":/image/yw.png")
    , BombPixmap(":/image/bomb.png")
{
    setWindowIcon(QIcon(":/image/Player.png"));
    cnt=0;
    setFocus();
    wallPixmap=wallPixmap.scaled(BOXSIZE,BOXSIZE);
    aimPixmap=aimPixmap.scaled(BOXSIZE,BOXSIZE);
    manPixmap=manPixmap.scaled(BOXSIZE,BOXSIZE);
    BombPixmap=BombPixmap.scaled(BOXSIZE,BOXSIZE);
    manOnAimPixmap=manOnAimPixmap.scaled(BOXSIZE,BOXSIZE);
    boxPixmap=boxPixmap.scaled(BOXSIZE,BOXSIZE);
    boxOnAimPixmap=boxOnAimPixmap.scaled(BOXSIZE,BOXSIZE);
    goldPixmap=goldPixmap.scaled(BOXSIZE,BOXSIZE);
    YellowwallPixmap=YellowwallPixmap.scaled(BOXSIZE,BOXSIZE);
    this->setWindowTitle(title);
    roundLabel=new QLabel(this);
    roundLabel->setAlignment(Qt::AlignCenter);
    MaxPointLabel=new QLabel(this);
    MaxPointLabel->setAlignment(Qt::AlignCenter);
    PointLabel=new QLabel(this);
    PointLabel->setAlignment(Qt::AlignCenter);
    stepLabel=new QLabel(this);
    nextBtn=new QPushButton(QString("下一關"),this);
    connect(nextBtn,SIGNAL(clicked()),this,SLOT(onNextBtnClicked()));
    restartBtn=new QPushButton(QString("重新開始"),this);
    connect(restartBtn,SIGNAL(clicked()),this,SLOT(onRestartBtnClicked()));
    goBackBtn=new QPushButton(QString("倒退"),this);
    connect(goBackBtn,SIGNAL(clicked()),this,SLOT(onGoBackBtnClicked()));
    preBtn=new QPushButton(QString("上一關"),this);
    connect(preBtn,SIGNAL(clicked()),this,SLOT(onPreBtnClicked()));

    menu=menuBar()->addMenu(tr("主菜單"));
    QAction *action=menu->addAction(tr("新開遊戲"));
    connect(action,SIGNAL(triggered()),this,SLOT(onNewGame()));
    menu->addSeparator();
    QAction *action2=menu->addAction(tr("重新開始"));
    connect(action2,SIGNAL(triggered()),this,SLOT(onRestartBtnClicked()));
    menu->addSeparator();
    QAction *action3=menu->addAction(tr("上一關"));
    connect(action3,SIGNAL(triggered()),this,SLOT(onPreBtnClicked()));
    QAction *action1=menu->addAction(tr("下一關"));
    connect(action1,SIGNAL(triggered()),this,SLOT(onNextBtnClicked()));
    menu->addSeparator();
    QAction *action4=menu->addAction(tr("離開遊戲"));
    connect(action4,SIGNAL(triggered()),this,SLOT(onActionLeaveGame()));

    game=NULL;
    highpoint=0;
    nowpoint=0;
    round=0;
    loadGame();
}

bool MainWindow::loadGame(QString filename,QString qs)
{
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text))return false;
    delete game;
    string str=QString(file.readAll()).toStdString();
    istringstream sin(str);
    string s,ss;
    int n,m;
    sin>>n>>m;getline(sin,ss);
    while(getline(sin,ss))s.append(ss);
    while(n<7)
    {
        n++;
        for(int i=0;i<m;i++)s.append(" ");
    }
//    cout<<n<<' '<<m<<"\n"<<s<<endl;
    game=new Game(n,m,s);
    repaint();
    TouchYellow=false;
    setFixedSize(BOXSIZE*(m+4),MENUSIZE+BOXSIZE*n);
    QFont font;
    font.setPointSize(BOXSIZE/4);
    int x=BOXSIZE/2,y=MENUSIZE+BOXSIZE/3;
    int w=BOXSIZE*3,h=BOXSIZE;

    QString Level;
    Level.setNum(round+1);
    font.setPointSize(font.pointSize()*1);
    roundLabel->setGeometry(x+BOXSIZE*17/2,y,w,h);
    roundLabel->setText("當前關卡: "+Level);
    roundLabel->setFont(font);

    QString HighP;
    HighP.setNum(highpoint);
    MaxPointLabel->setGeometry(x,y,w,h);
    MaxPointLabel->setText("最高分: "+HighP);
    MaxPointLabel->setFont(font);
    QString NowP;
    NowP.setNum(game->NowPoint);//nowpoint
    y+=BOXSIZE;
    PointLabel->setGeometry(x,y,w,h);
    PointLabel->setText("分數: "+NowP);
    PointLabel->setFont(font);

    y+=BOXSIZE+BOXSIZE/4;
    preBtn->setGeometry(x,y,w,h);

    y+=BOXSIZE+BOXSIZE/2;
    restartBtn->setGeometry(x,y,w/2-2,h);
    goBackBtn->setGeometry(x+restartBtn->width()+4,y,w/2-2,h);
    y+=BOXSIZE+BOXSIZE/2;
    nextBtn->setGeometry(x,y,w,h);
    y=(y+BOXSIZE+n*(BOXSIZE))/2-h/2+MENUSIZE/2;
    stepLabel->setFont(font);
    stepLabel->setText("步數: 0");
    stepLabel->setGeometry(x,y,w,h);
    return true;
}

bool MainWindow::loadGame()
{
    QString qs;
    qs.setNum(round);
    TouchYellow=false;
    return loadGame(":/map/map"+qs+".txt",qs);
}

MainWindow::~MainWindow()
{
    delete game;
    delete roundLabel;
    delete PointLabel;
    delete MaxPointLabel;
    delete stepLabel;
    delete nextBtn;
    delete restartBtn;
    delete preBtn;
    delete goBackBtn;
}
void MainWindow::onNewGame(){
    nowpoint=0;
    round=0;
    loadGame();
}

void MainWindow::nextStep()
{
    game->move(*showAnsIt);
    showAnsIt++;
    checkGame();
    if(showAnsIt==showAns.end())
    {
        cnt=0;
    }
}

void MainWindow::preStep()
{
    if(showAnsIt==showAns.begin())return;
    game->goBack();
    showAnsIt--;
    checkGame();
}

#define PAINT(pixmap) painter.drawPixmap(PANELX+j*BOXSIZE,PANELY+i*BOXSIZE,BOXSIZE,BOXSIZE,pixmap)

void MainWindow::paintEvent(QPaintEvent *)
{
    if(cnt==4)
    {
        QPalette pe;
        pe.setColor(QPalette::WindowText,Qt::red);
        stepLabel->setPalette(pe);
    }
    else
    {
        QPalette pe;
        pe.setColor(QPalette::WindowText,Qt::black);
        stepLabel->setPalette(pe);
    }
    QPainter painter(this);
    QColor color(222,222,222);
    painter.setPen(color);
    painter.setBrush(QBrush(color,Qt::SolidPattern));
    painter.drawRect(QRect(PANELX,PANELY,BOXSIZE*game->m,BOXSIZE*game->n));
    for(int i=0;i<game->n;i++)
    {
        for(int j=0;j<game->m;j++)
        {
            if(game->mp[i][j]==Game::WALL)PAINT(wallPixmap);
            else if(game->mp[i][j]==Game::AIM)PAINT(aimPixmap);
            else if(game->mp[i][j]==Game::MAN)PAINT(manPixmap);
            else if(game->mp[i][j]==(Game::MAN|Game::AIM))PAINT(manOnAimPixmap);
            else if(game->mp[i][j]==Game::BOX)PAINT(boxPixmap);
            else if(game->mp[i][j]==(Game::BOX|Game::AIM))PAINT(boxOnAimPixmap);
            else if(game->mp[i][j]==(Game::YWall))PAINT(YellowwallPixmap);
            else if(game->mp[i][j]==(Game::YWall|Game::MAN))PAINT(manPixmap);
            else if(game->mp[i][j]==(Game::GOLD))PAINT(goldPixmap);
            else if(game->mp[i][j]==(Game::GOLD|Game::MAN))PAINT(manPixmap);
            else if(game->mp[i][j]==(Game::Bomb))PAINT(BombPixmap);
            else if(game->mp[i][j]==(Game::Bomb|Game::AIM))PAINT(BombPixmap);
              else if(game->mp[i][j]==(Game::Bomb|Game::AIM|Game::MAN))PAINT(manPixmap);
        }
    }
}


void MainWindow::mousePressEvent(QMouseEvent *event)
{
    /*
    QMutexLocker locker(&mutex);
    if(event->button()==Qt::LeftButton)//&&(event->buttons()&Qt::LeftButton))
    {
        if(cnt==4)nextStep();
        else
        {
            int y=(event->pos().x()-PANELX)/BOXSIZE,x=(event->pos().y()-PANELY)/BOXSIZE;
            if(x<0||x>=game->n||y<0||y>=game->m)return;
            vector<int>v=game->move(x,y);
            //setEnabled(false);
            for(vector<int>::iterator i=v.begin();i!=v.end();i++)
            {
               game->move(*i);
               QElapsedTimer timer;
               timer.start();
               while(timer.elapsed()<50);
                   //QCoreApplication::processEvents();
               checkGame();
            }
            //setEnabled(true);
        }

    }
    else if(event->button()==Qt::RightButton)
    {
        if(cnt==4)preStep();
        else onGoBackBtnClicked();
    }
    */
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    QMutexLocker locker(&mutex);
    if(cnt==4)
    {
        switch(e->key())
        {
        case Qt::Key_Escape:
            cnt=0;
            checkGame();
            break;
        case Qt::Key_Left:
            preStep();
            break;
        case Qt::Key_Right:
            nextStep();
            break;
        }
        return;
    }
    if(cnt==3)
    {
        if(e->key()=='E')
        {
            cnt++;
            game->start();

            int nowV=0,maxV=game->getH();
            int maxRange=6666;
            QProgressDialog process(this);
            QString qs;
            process.setLabelText(tr("搜索路徑中...\n估計步數")+qs.setNum(maxV*10));
            process.setWindowTitle(this->windowTitle());
            process.setWindowModality(Qt::WindowModal);
            process.setRange(0,maxRange);
            process.setCancelButtonText(tr("取消"));
            process.setMinimumDuration(0);
            process.setMinimumWidth(300);
            process.show();
            while(nowV<maxRange)
            {
                nowV=min(nowV+1,(maxV-game->minGH)*maxRange/maxV);
                process.setValue(nowV);
                QApplication::processEvents();
                if(process.wasCanceled())
                {
                    game->minGH=-1;
                    process.setLabelText(tr("正在取消搜索..."));
                    game->wait();
                    cnt=0;
                    return;
                }
            }
            game->wait();
            showAns=game->ans;
            showAnsIt=showAns.begin();
            game->restart();
            checkGame();
            return;
        }
        else if(e->key()=='K')return;
        else cnt=0;
    }
    else if(e->key()=='K')
    {
        cnt++;
    }

    else cnt=0;
    switch (e->key()) {
    case 'W':
    case Qt::Key_Up:
        game->move(Game::UP);
        break;
    case 'S':
    case Qt::Key_Down:
        game->move(Game::DOWN);
        break;
    case 'A':
    case Qt::Key_Left:
        game->move(Game::LEFT);
        break;
    case 'D':
    case Qt::Key_Right:
        game->move(Game::RIGHT);
        break;
    case 'R':
        onRestartBtnClicked();
        break;
    case 'P':
        onGoBackBtnClicked();
    }
    checkGame();
}

void MainWindow::checkGame()
{
    repaint();
    QString s;
    stepLabel->setText("步數: "+s.setNum(game->getStep()));
    PointLabel->setText("分數: "+s.setNum(game->getNowP()));
    if(game->isWin())
    {
        QMessageBox::StandardButton reply = QMessageBox::question(this, tr("恭喜通關"),
                                            tr("使用")+stepLabel->text()+"\n開始下一關?",
                                            QMessageBox::Yes | QMessageBox::No);
        if(reply == QMessageBox::Yes)round++;
        else
        {
            game->restart();
            checkGame();
            return;
        }
        if(!loadGame())
        {
            reply = QMessageBox::question(this, tr("恭喜通關"),
                                                tr("您已通關,是否從第1關開始?"),
                                                QMessageBox::Yes | QMessageBox::No);
            if(reply == QMessageBox::Yes)
            {
               if(highpoint<game->getNowP()){
                 highpoint=game->getNowP();
               }
                round=0;
                loadGame();
            }
            else if(reply == QMessageBox::No)exit(0);
            else round--;
        }
    }
}

void MainWindow::onNextBtnClicked()
{
    if(cnt==4){
        //killTimer(timerId);
        cnt=0;}
    round++;
    if(!loadGame())round--;
    update();
    setFocus();
}

void MainWindow::onRestartBtnClicked()
{
    if(cnt==4){
        //killTimer(timerId);
        cnt=0;}
    game->restart();
    checkGame();
    setFocus();
}

void MainWindow::onPreBtnClicked()
{
    if(cnt==4){
        //killTimer(timerId);
        cnt=0;}
    round--;
    if(!loadGame())round++;
    update();
    setFocus();
}

void MainWindow::onGoBackBtnClicked()
{if(cnt==4)return;
    game->goBack();
    checkGame();
}

void MainWindow::onActionClicked()
{
    if(cnt==4){
        //killTimer(timerId);
        cnt=0;}
    QString filename = QFileDialog::getOpenFileName(this,
                                                    tr("自定圖"),
                                                    tr(""),
                                                    tr(".txt(*.txt)")
                                                    );
    loadGame(filename,tr(""));
}
void MainWindow::onActionLeaveGame()
{
    QApplication* app;
    app->quit();
}
