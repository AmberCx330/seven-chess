#include "gamescreen.h"

#include "mainwindow.h"
#include "ui_gamescreen.h"
#include <QPushButton>

gamescreen::gamescreen(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::gamescreen)
{
    ui->setupUi(this);

    screen = new MainWindow;
    screen->setWindowIcon(QIcon(":/icon1.jpg"));              //设置窗口图标
    screen->autoFillBackground();
    QPalette pal;
    QImage image=QImage(":/background3.jpg");           //设置背景图片

    pal.setBrush(QPalette::Background,QBrush(image));
    screen->setPalette(pal);


    connect(ui->pushButton,&QPushButton::clicked,[=]{this->hide();screen->show();});
    connect(screen->back_,&QPushButton::clicked,[=]{this->show();screen->hide();screen->renew();});

}

gamescreen::~gamescreen()
{
    delete ui;
}

void gamescreen::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawPixmap(0,0,this->width(),this->height(),QPixmap(":/background.png"));
    QPainter painter1(this);
    painter1.drawPixmap(350,380,401,121,QPixmap(":/start.png"));
    QPainter painter2(this);

    painter2.drawPixmap(410,220,361,121,QPixmap(":/title1.png"));


}
