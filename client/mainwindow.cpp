#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->pushButton_2->setStyleSheet("background-color: rgb(255, 255, 255,150);");

    ui->radioButtonb->setChecked(1);         //默认选中黑棋
    player=BLACK;
    playerlabel=new QLabel;
    playerlabel->setText("执黑棋，先落子 ");
    ui->statusbar->addPermanentWidget(playerlabel);

    for(int i=0;i<442;i++)
    {

        bufferReceive[i]='0';
    }

    netlabel=new QLabel;

    connect(ui->radioButtonw,&QRadioButton::clicked,this,&MainWindow::on_radioButtonw_clicked);
    connect(ui->radioButtonb,&QRadioButton::clicked,this,&MainWindow::on_radioButtonb_clicked);

}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    const int size=20;
    const int width=40;
    const int x=40,y=70;

    QPainter paint(this);
    paint.setPen(QPen(Qt::black,1,Qt::SolidLine));

    //画棋盘
    for(int i=0;i<=size;i++)
    {
        paint.drawLine(x,y+width*i,x+size*width,y+width*i);
    }
    for(int i=0;i<=size;i++)
    {
        paint.drawLine(x+width*i,y,x+width*i,y+size*width);
    }

    //画棋子
    for(int i=0;i<441;i++)
    {
        int t=i/21,s=i%21;
            if(bufferReceive[i]=='1')
            {
                paint.setBrush(QBrush(Qt::black,Qt::SolidPattern));
                paint.drawEllipse(x+width*s-15,y+width*t-15,32,32);
            }
            if(bufferReceive[i]=='2')
            {
                paint.setBrush(QBrush(Qt::white,Qt::SolidPattern));
                paint.drawEllipse(x+width*s-15,y+width*t-15,32,32);
            }
    }
    paint.end();
}


void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    int mousex,mousey;
    mousex=event->x();
    mousey=event->y();

    if(mousex>30&&mousex<850&&mousey>60&&mousey<880)              //下棋
    {

        int i,j;
        j=(mousex-20)/40;
        i=(mousey-50)/40;

        if(bnum==0)
        {
            for(int i=0;i<442;i++)
                bufferReceive[i]='0';
        }


     if(Netflag==-1)
            QMessageBox::warning(NULL,"提示：","请连接服务端！");
        else if(player==BLACK&&bnum>wnum)
            QMessageBox::warning(NULL,"提示：","请等待服务端落子！");
        else if(player==WHITE&&bnum==wnum)
            QMessageBox::warning(NULL,"提示：","请等待服务端落子！");
        else
        {
            if(bufferReceive[i*21+j]!='1'&&bufferReceive[i*21+j]!='2')
            {
                if(player==BLACK)
                {
                    if(bnum!=0)
                    {
                        Nettimer->stop();
                        ui->nettime->setText("20");
                    }

                    bufferReceive[i*21+j]='1';
                    bnum++;
                }
                else
                {
                    Nettimer->stop();
                    ui->nettime->setText("20");

                    bufferReceive[i*21+j]='2';
                    wnum++;
                }

                update();


                bufferReceive[442]=i+'0';         //fferReceive[442]棋子x坐标 [443]棋子y坐标
                bufferReceive[443]=j+'0';

                socket->write(bufferReceive);

            }

        }

    }
}

void MainWindow::on_pushButton_clicked()                     //连接服务端
{
    socket=new QTcpSocket(this);
    socket->connectToHost(ui->lineEdit->text(),ui->lineEdit_2->text().toInt());

    connect(socket,&QTcpSocket::connected,this,[=](){
        netlabel->setText("  connect successfully！ ");
        ui->statusbar->addWidget(netlabel);
        Netflag=6;

        ui->pushButton->hide();
    });

    connect(socket,SIGNAL(readyRead()),this,SLOT(receivemessage()));


}

void MainWindow::receivemessage()                            //接收服务端信息
{
      socket->read(bufferReceive,socket->bytesAvailable());
      update();

      if(bufferReceive[441]=='b')                            //fferReceive[441]=w/b/j/c  白棋赢/黑棋赢/禁手/超时标志
      {
          update();
          QMessageBox::warning(NULL,"胜负已定：","black wins！");
          renew();
      }

      else if(bufferReceive[441]=='c')
      {
          if(player==BLACK)
          {
              QMessageBox::warning(NULL,"超时：","black wins！");
          }
          else
          {
              QMessageBox::warning(NULL,"超时：","white wins！");
          }
          renew();
      }

      else
      {

      if(bufferReceive[441]=='w')
      {
          QMessageBox::warning(NULL,"胜负已定：","white wins！");
          renew();
      }
      else if(bufferReceive[441]=='j')
      {
          QMessageBox::warning(NULL,"出现禁手：","white wins！");
          renew();
      }
      else
      {
          bnum=0;
          wnum=0;
          for(int i=0;i<441;i++)
          {
              if(bufferReceive[i]=='1')
                  bnum++;
              if(bufferReceive[i]=='2')
                  wnum++;
          }
          nettimer();
      }

      }
}
void MainWindow::renew()                         //更新棋盘
{
    TIME=20;
    for(int i=0;i<442;i++)
        bufferReceive[i]='0';
    bnum=0;
    wnum=0;
    update();
}



void MainWindow::on_pushButton_2_clicked()       //重新开始
{
    if(bnum!=0&&wnum!=0)
    {
        if((bnum==wnum&&player==BLACK)||(player==WHITE&&bnum>wnum))
        {
            Nettimer->stop();
            ui->nettime->setText("20");
        }

        char buffersend[445];
        for(int i=0;i<445;i++)
            buffersend[i]='0';
        buffersend[441]='r';
        socket->write(buffersend);
        renew();
    }

}


void MainWindow::nettimer()                        //网络倒计时
{
    TIME=20;
    Nettimer = new QTimer(this);
    connect(Nettimer, SIGNAL(timeout()), this, SLOT(showNettimelimit()));
    Nettimer->setInterval( 1000 );
    Nettimer->start();
}


void MainWindow::showNettimelimit()
{
    if(TIME != 0)
    {
        TIME--;
        QString num = QString::number(TIME);
        ui->nettime->setText(num);
    }
    else
    {
        char buffertime[443];
        for(int i=0;i<441;i++)
            buffertime[i]='0';
        buffertime[441]='c';
        socket->write(buffertime);

        if(player==BLACK)
            QMessageBox::warning(NULL,"超时：","white wins！");
        else
            QMessageBox::warning(NULL,"超时：","black wins！");
        renew();
        Nettimer->stop();
        ui->nettime->setText("20");
        TIME=20;
    }
}


void MainWindow::on_radioButtonw_clicked()              //选择白棋
{
    if(Netflag==6&&bnum==0&&wnum==0)
    {
        player=WHITE;
        char buffer[2];
        buffer[0]='w';
        socket->write(buffer);
        playerlabel->setText("执白棋，后落子 ");
        ui->statusbar->addPermanentWidget(playerlabel);
    }

}

void MainWindow::on_radioButtonb_clicked()               //选择黑棋
{
    if(Netflag==6&&bnum==0&&wnum==0)
    {
        player=BLACK;
        char buffer[2];
        buffer[0]='b';
        socket->write(buffer);
        playerlabel->setText("执黑棋，先落子 ");
        ui->statusbar->addPermanentWidget(playerlabel);
    }
}
