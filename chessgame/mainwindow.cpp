
#include "mainwindow.h"
#include "ui_mainwindow.h"


#include <QPainter>
#include <QTimer>
#include <stdlib.h>
#include <QTextEdit>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    socket = NULL;
    ui->setupUi(this);

    setContextMenuPolicy(Qt::DefaultContextMenu);                                    //设置右键菜单



    back_=ui->back;
    ui->back->setStyleSheet("background-color: rgb(255, 255, 255,150);");
    ui->pushButton->setStyleSheet("background-color: rgb(255, 255, 255,150);");     //设置按钮颜色
    ui->pushButton_2->setStyleSheet("background-color: rgb(255, 255, 255,150);");
    ui->pushButton_ai->setStyleSheet("background-color: rgb(255, 255, 255,150);");
    ui->stop->setStyleSheet("background-color: rgb(255, 255, 255,150);");


    ui->pushButton_ai->hide();
    ui->stop->hide();
    ui->label_nettime->hide();                                     //“倒计时：”隐藏
    ui->nettime->hide();                                            //倒计时时间隐藏
    ui->piece_remind->hide();
    ui->label_5->hide();                                            //“秒”

    piece_color_now=true;                                              //设置默认值
    mode=Normal;

    menu1 = new QMenu(this);
    QAction *normalmode=menu1->addAction("双人对战模式");         //添加菜单选项，并命名
    QAction *AImode=menu1->addAction("人机对战模式");
    QAction *aimode=menu1->addAction("机机对战模式");
    QAction *netmode=menu1->addAction("网络对战模式");

    connect(normalmode,&QAction::triggered,this,&MainWindow::triggeredNormal);      //双人对战模式
    connect(AImode,&QAction::triggered,this,&MainWindow::triggeredAI);              //人机对战模式
    connect(aimode,&QAction::triggered,this,&MainWindow::triggeredai);              //机机对战模式
    connect(netmode,&QAction::triggered,this,&MainWindow::triggeredNet);        //网络对战模式

    netlable= new QLabel(this);
    lable= new QLabel("  当前游戏模式：双人对战  ");
    ui->statusbar->setFixedHeight(40);
    ui->statusbar->addWidget(lable);

    connect(ui->pushButton,&QPushButton::clicked,this,[=](){         //重新开始
        if(board.getbnum()!=0||board.getwnum()!=0)                     //黑子与白子个数
        {
            if(piece_color_now)
            {
                Time2->stop();
                ui->label2->setText("20");
            }
            else
            {
                Time1->stop();
                ui->label->setText("20");
            }
            renew();                                                   //棋盘初始化
        }

    });

    connect(ui->pushButton_2,&QPushButton::clicked,this,[=](){        //悔棋
        if(mode==Normal)
        {
            if(piece_color_now)                                      //轮到黑子
            {
                if(board.getwnum()!=0)
                {
                    board.undo(WHITE);
                    piece_color_now=false;
                    Time2->stop();
                    ui->label2->setText("20");
                    timer1();
                }
                else
                    QMessageBox::warning(NULL,"提示：","无棋可悔");
            }
            else
            {
                if(board.getbnum()!=0)
                {
                    board.undo(BLACK);
                    piece_color_now=true;
                    Time1->stop();
                    ui->label->setText("20");
                    timer2();
                }
                else
                    QMessageBox::warning(NULL,"提示：","无棋可悔");
            }
        }
        update();
    });


}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::slogRecv()                         //网络对战接收客户端棋盘信息
{
    char bufferReceive[445];
    for(int i=0;i<445;i++)
    {
        bufferReceive[i]='0';
    }


    socket->read(bufferReceive,socket->bytesAvailable());

    if(bufferReceive[0]=='w')                              //bufferReceive[0] 接收客户端选择的先后手顺序
    {
        ui->piece_remind->setText("执黑棋，先落子");
        serverId=BLACK;
        bufferReceive[0]='0';
    }
    else if(bufferReceive[0]=='b')
    {
        ui->piece_remind->setText("执白棋，后落子");
        serverId=WHITE;
        bufferReceive[0]='0';
    }

    else
    {
        if(bufferReceive[441]=='r')                          //客户端要求重新开始
        {
            if((serverId==BLACK&&board.getbnum()==board.getwnum())||(serverId==WHITE&&board.getbnum()>board.getwnum()))           //server端下棋时重置倒计时
            {
                Nettimer->stop();
                ui->nettime->setText("20");
            }
            renew();
        }

        else if(bufferReceive[441]=='c')                     //客户端落子超时
        {
            Nettimer->stop();
            ui->nettime->setText("20");
            if(serverId==WHITE)
                QMessageBox::warning(NULL,"超时：","white wins！");
            else
                QMessageBox::warning(NULL,"超时：","black wins！");
            renew();
        }

        else
        {
            for(int i=0;i<441;i++)
            {
                if(bufferReceive[i]=='1')
                {
                    board.placepiece(i/21,i%21,BLACK);
                }
                if(bufferReceive[i]=='2')
                {
                    board.placepiece(i/21,i%21,WHITE);
                }
            }
            update();

            if(judger.judgewin(board,bufferReceive[442]-'0',bufferReceive[443]-'0'))           //判断客户端下完棋后棋面信息
            {
                if(serverId==WHITE)
                {
                    bufferReceive[441]='b';                             //发送获胜信息
                    socket->write(bufferReceive);
                    QMessageBox::warning(NULL,"胜负已定：","black wins！");
                }
                else
                {
                    bufferReceive[441]='w';
                    socket->write(bufferReceive);
                    QMessageBox::warning(NULL,"胜负已定：","white wins！");
                }
                renew();
            }

            else if(serverId==WHITE&&judger.FLAG)
            {
                bufferReceive[441]='j';                                  //发送出现禁手信息
                socket->write(bufferReceive);                            //数据发送
                QMessageBox::warning(NULL,"出现禁手：","white wins！");
                renew();
            }
            else
            nettimer();
        }
    }


}




void MainWindow::paintEvent(QPaintEvent *event){
    const int size=20;
    const int width=40;         //每格的宽度
    const int x=40,y=70;        //棋盘上(0,0)点坐标为(40,70)


    //画棋盘
    QPainter paint(this);
    paint.setPen(QPen(Qt::black,1,Qt::SolidLine));                                      //钢笔工具：颜色，线号，实线

    for(int i=0;i<=size;i++)                                                            //画SIZE+1条横线
    {
        paint.drawLine(x,y+width*i,x+size*width,y+width*i);                             //画线函数：x1,x2,x2,y2;画从(x1,y1)到（x2,y2)的线
    }
    for(int i=0;i<=size;i++)                                                            //画SIZE+1条竖线
    {
        paint.drawLine(x+width*i,y,x+width*i,y+size*width);                             //画线函数：x1,x2,x2,y2;画从(x1,y1)到（x2,y2)的线
    }


    //画棋子
    for(int i=0;i<=size;i++)
    {
        for(int j=0;j<=size;j++)
        {
            if(board.showpiece(i,j)==BLACK)
            {
                paint.setBrush(QBrush(Qt::black,Qt::SolidPattern));
                paint.drawEllipse(x+width*j-15,y+width*i-15,32,32);                     //画椭圆函数（矩形左上角坐标，宽度，高度）
            }
            if(board.showpiece(i,j)==WHITE)
            {
                paint.setBrush(QBrush(Qt::white,Qt::SolidPattern));
                paint.drawEllipse(x+width*j-15,y+width*i-15,32,32);
            }
        }
    }
    paint.end();
}


void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    int mousex,mousey;
    mousex=event->x();                                         //获取鼠标事件发生时鼠标相对于窗口的位置
    mousey=event->y();

    if(mousex>30&&mousex<850&&mousey>60&&mousey<880)          //棋盘内部判断
    {
        int i,j,p;
        j=(mousex-20)/40;
        i=(mousey-50)/40;


        //双人对战
        if(mode==Normal)
        {
            if(board.showpiece(i,j)==-1)
            {
                TIME=20;

                if(piece_color_now)
                {
                    p=BLACK;
                    piece_color_now=false;
                }
                else
                {
                    p=WHITE;
                    piece_color_now=true;
                }
                board.placepiece(i,j,p);
                if(p==BLACK)
                {
                    if(board.getwnum()!=0)
                    {
                        Time2->stop();
                        ui->label2->setText("20");
                    }
                    timer1();
                }
                else
                {
                    Time1->stop();
                    ui->label->setText("20");
                    timer2();
                }

                if(judger.judgewin(board,i,j))
                {
                    if(p==BLACK)                                              //判断时白棋倒计时已经开始
                    {
                        Time1->stop();
                        QMessageBox::warning(NULL,"game over：","black wins！");
                        renew();
                    }
                    if(p==WHITE)
                    {
                        Time2->stop();
                        QMessageBox::warning(NULL,"game over：","white wins！");
                        renew();
                    }
                }
                update();                                            //调用绘图事件，使画面更新
            }

            if(judger.FLAG)
            {
                Time1->stop();
                QMessageBox::warning(NULL,"出现禁手：","white wins！");
                renew();
            }

        }


        //人机对战
        if(mode==AI)
        {
            if(piece_color_now&&board.showpiece(i,j)==-1)
            {
                board.placepiece(i,j,BLACK);
                if(board.getwnum()!=0)
                {
                    Time2->stop();
                    ui->label2->setText("20");
                }
                if(judger.judgewin(board,i,j))
                {
                    QMessageBox::warning(NULL,"胜负已定：","black wins！");
                    renew();
                }
                else if(judger.FLAG)
                {
                    QMessageBox::warning(NULL,"出现禁手：","white wins！");
                    renew();
                }
                else
                {
                    piece_color_now=false;
                    QTimer::singleShot(1000,this,SLOT(aiplacepiece()));      //AI 停顿1s后下棋
                    TIME=20;
                    timer2();             
                }
                update();
            }
        }



        //网络对战
        if(mode==Net)
        {
            if(!NetFlag)
            {
                QMessageBox::warning(NULL,"提示：","请等待客户端连接！");
            }
            else
            {
                if(serverId==WHITE&&board.getbnum()<=board.getwnum())
                {

                    QMessageBox::warning(NULL,"提示：","请等待客户端落子！");
                }
                else if(serverId==BLACK&&board.getbnum()>board.getwnum())
                {
                    QMessageBox::warning(NULL,"提示：","请等待客户端落子！");
                }
                else if(board.showpiece(i,j)==-1)
                {
                    if(serverId==BLACK&&board.getbnum()!=0)
                    {
                        Nettimer->stop();
                        ui->nettime->setText("20");
                    }
                    else if(serverId==WHITE)
                    {
                        Nettimer->stop();
                        ui->nettime->setText("20");
                    }

                    char buffer[443];              //buffer[441]=w/b/j/c  白棋赢/黑棋赢/禁手/超时标志
                    int k=0;

                    for(int i=0;i<21;i++)
                    {
                        for(int j=0;j<21;j++)
                        {
                            buffer[k]='0';
                            k++;
                        }
                    }

                    board.placepiece(i,j,serverId);
                    update();

                    for(int i=0;i<21;i++)
                    {
                        for(int j=0;j<21;j++)
                        {
                            buffer[i*21+j]=board.showpiece(i,j)+'0';
                        }
                     }

                    if(judger.judgewin(board,i,j))
                    {
                        if(serverId==WHITE)
                            buffer[441]='w';
                        else
                            buffer[441]='b';                    

                        socket->write(buffer);

                        if(serverId==WHITE)
                            QMessageBox::warning(NULL,"胜负已定：","white wins！");
                        else
                            QMessageBox::warning(NULL,"胜负已定：","black wins！");
                        renew();
                    }
                    else if(serverId==BLACK&&judger.FLAG)
                    {
                        buffer[441]='j';
                        socket->write(buffer);
                        QMessageBox::warning(NULL,"出现禁手：","white wins！");
                        renew();
                    }

                    else
                    {
                        buffer[441]='0';
                        socket->write(buffer);
                    }

                }
            }

        }

        //和棋
        if(judger.judgedraw(board))
        {            
            Time2->stop();
            Time1->stop();
            QMessageBox::warning(NULL,"game over：","和棋");
            renew();
        }
    }
}


void MainWindow::on_pushButton_ai_clicked()
{
    //机机对战
    if(mode==ai&&board.getbnum()==0&&board.getwnum()==0)
    {
        ui->stop->show();
        if(board.getbnum()==0)
        {
            board.placepiece(10,10,BLACK);
            piece_color_now=false;
            update();
        }

        timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(aiplacepiece()));
        timer->start(700);
     }


}


void MainWindow::on_stop_clicked()
{
    timer->stop();
    renew();
    ui->stop->hide();
}


void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    menu1->move(cursor().pos());                               //让菜单显示的位置在鼠标的坐标上
    if(board.getbnum()==0&&board.getwnum()==0)
    {
        menu1->show();
    }
}



void MainWindow::aiplacepiece()                                    //AI下棋
{
        judger.judgevalue(&board);
        int max=0,k=0;
        Node maxvalue[421];

        for(int i=0;i<21;i++)
        {
            for(int j=0;j<21;j++)
            {
                if(board.value[i][j]>max)
                    max=board.value[i][j];
            }
        }

        for(int i=0;i<21;i++)
        {
            for(int j=0;j<21;j++)
            {
                if(board.value[i][j]==max)
                {
                    maxvalue[k].x=i;
                    maxvalue[k].y=j;
                    k++;
                }
            }
        }

        int p;
        if(piece_color_now)
            p=BLACK;
        else
            p=WHITE;

        srand(time(0));                                            //设置变量种子
        int n=(rand()%(k));                                       //随机选择分数最大的一点落子
        board.placepiece(maxvalue[n].x,maxvalue[n].y,p);

        if(p==BLACK)
            piece_color_now=false;
        else
            piece_color_now=true;

        if(judger.judgewin(board,maxvalue[n].x,maxvalue[n].y))
        {
            if(mode!=ai)
            {
                Time2->stop();
                QMessageBox::warning(NULL,"胜负已定：","white wins！");

            }
            else
            {
                timer->stop();
                if(piece_color_now)
                    QMessageBox::warning(NULL,"胜负已定：","white wins！");
                else
                    QMessageBox::warning(NULL,"胜负已定：","black wins！");
                ui->stop->hide();
            }
            renew();
        }
        else if(judger.judgedraw(board))                                           //判断和棋
        {
            if(mode!=ai)
            {
                Time2->stop();
                Time1->stop();
                QMessageBox::warning(NULL,"比赛结果：","和棋");
            }
            else
            {
                timer->stop();
                QMessageBox::warning(NULL,"比赛结果：","和棋");
                ui->stop->hide();
            }

            renew();
        }
        update();
}

void MainWindow::renew()                               //更新棋盘，全部初始化
{
    TIME=20;

    if(board.getbnum()!=0)
    {
        while(board.getbnum())
        {
            board.undo(BLACK);
        }
    }

    if(board.getwnum()!=0)
    {
        while(board.getwnum())
        {
            board.undo(WHITE);
        }
    }

    piece_color_now=true;
    for(int i=0;i<21;i++)
    {
        for(int j=0;j<21;j++)
        {
            board.value[i][j]=0;
        }
     }
     update();                                               //重绘窗口
}






void MainWindow::timer1()                                         //白棋倒计时
{
    TIME=20;
    Time1 = new QTimer(this);
    connect(Time1, SIGNAL(timeout()), this, SLOT(showTimelimit1()));
    Time1->setInterval( 1000 );                                   //
    Time1->start();                                               //间隔1s发送timeout信号
}


void MainWindow::showTimelimit1()
{
    if(TIME != 0)
    {
        TIME--;
        QString num = QString::number(TIME);
        ui->label->setText(num);
    }
    else
    {
        QMessageBox::warning(NULL,"超时：","black wins！");
        renew();
        Time1->stop();
        ui->label->setText("20");
        TIME=20;
    }
}


void MainWindow::timer2()                                          //黑棋倒计时
{
    TIME=20;
    Time2 = new QTimer(this);
    connect(Time2, SIGNAL(timeout()), this, SLOT(showTimelimit2()));
    Time2->setInterval( 1000 );
    Time2->start();
}


void MainWindow::showTimelimit2()
{
    if(TIME != 0)
    {
        TIME--;
        QString num = QString::number(TIME);
        ui->label2->setText(num);
    }
    else
    {
        QMessageBox::warning(NULL,"超时：","white wins！");
        renew();
        Time2->stop();
        ui->label2->setText("20");
        TIME=20;
    }
}


void MainWindow::nettimer()                                             //网络倒计时
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
        {
            buffertime[i]='0';
        }

        buffertime[441]='c';
        socket->write(buffertime);
        if(serverId==BLACK)
            QMessageBox::warning(NULL,"超时：","black wins！");
        else
            QMessageBox::warning(NULL,"超时：","white wins！");
        renew();
        Nettimer->stop();
        ui->nettime->setText("20");
        TIME=20;
    }
}



void MainWindow::triggeredNormal()
{
    mode=Normal;
    ui->pushButton_2->show();
    ui->pushButton->show();
    ui->pushButton_ai->hide();
    ui->stop->hide();
    ui->label->show();
    ui->label_3->show();
    ui->label2->show();
    ui->label_2->show();
    ui->label_4->show();
    ui->label_6->show();
    ui->label_5->hide();
    ui->label_nettime->hide();
    ui->nettime->hide();
    ui->piece_remind->hide();

    lable->setText("  当前游戏模式：双人对战  ");
    ui->statusbar->addWidget(lable);

}


void MainWindow::triggeredAI()
{
    mode=AI;
    ui->pushButton_2->hide();
    ui->pushButton->show();
    ui->pushButton_ai->hide();
    ui->stop->hide();
    ui->label->show();
    ui->label_3->show();
    ui->label2->show();
    ui->label_2->show();
    ui->label_4->show();
    ui->label_6->show();
    ui->label_5->hide();
    ui->label_nettime->hide();
    ui->nettime->hide();
    ui->piece_remind->hide();

    lable->setText("  当前游戏模式：人机对战  ");
    ui->statusbar->addWidget(lable);

}


void MainWindow::triggeredai()
{
    mode=ai;
    ui->pushButton_2->hide();
    ui->pushButton->hide();
    ui->pushButton_ai->show();
    ui->label->hide();
    ui->label_3->hide();
    ui->label2->hide();
    ui->label_2->hide();
    ui->label_4->hide();
    ui->label_6->hide();
    ui->label_5->hide();
    ui->label_nettime->hide();
    ui->nettime->hide();
    ui->piece_remind->hide();

    lable->setText("  当前游戏模式：机机对战  ");
    ui->statusbar->addWidget(lable);

}


void MainWindow::triggeredNet()
{
    mode=Net;
    ui->pushButton_2->hide();
    ui->pushButton_ai->hide();
    ui->stop->hide();
    ui->pushButton->hide();
    ui->label->hide();
    ui->label_3->hide();
    ui->label2->hide();
    ui->label_2->hide();
    ui->label_4->hide();
    ui->label_6->hide();
    ui->label_5->show();
    ui->label_nettime->show();
    ui->nettime->show();

    lable->setText("  当前游戏模式：网络对战  ");
    ui->statusbar->addWidget(lable);

    _server=new QTcpServer;

    _server->listen(QHostAddress::Any,9999);                   //设置端口9999
    if(socket)                                                 //防止第三者连接，连接成功后，socket已经不为空
    {
        return;
    }
    connect(_server,&QTcpServer::newConnection,this,[=](){     //当有client连接时，提示客户端已连接，准备读取信息
            netlable->setText("connect successfully！");
            ui->statusbar->addPermanentWidget(netlable);
            ui->piece_remind->show();
            NetFlag=true;

            socket=_server->nextPendingConnection();           //返回QTcpsocket，给客户端发送数据
            connect(socket,SIGNAL(readyRead()),this,SLOT(slogRecv()));           //socket缓存区有新的数据触发readyRead信号

    });

}








