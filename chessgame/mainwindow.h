#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "checkerboard.h"
#include "judgewin.h"
#include <QMouseEvent>
#include <QMessageBox>
#include <QLabel>
#include <QTcpServer>
#include <QTcpSocket>
#include <QPushButton>




QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QPushButton *back_;
    void renew();                                  //更新棋盘



public slots:
    void showTimelimit1();                         //倒计时槽函数
    void showTimelimit2();
    void showNettimelimit();
    void triggeredNormal();
    void triggeredAI();
    void triggeredai();
    void triggeredNet();

private:
    Ui::MainWindow *ui;

    void paintEvent(QPaintEvent *event);           //绘图事件
    void mouseReleaseEvent(QMouseEvent *event);    //鼠标点击释放事件
    void contextMenuEvent(QContextMenuEvent *event);

    void timer1();                                 //启动倒计时
    void timer2();
    void nettimer();

    QLabel *netlable;
    QLabel *lable;
    QMenu *menu1;



    QTimer *Time1,*Time2,*Nettimer;
    QTimer *timer;
    int TIME=20;

    gamemode mode;                                  //游戏模式
    checkerboard board;
    judgement judger;

    bool piece_color_now;                                 //当前执子颜色

    QTcpServer *_server;                         //服务端、套接字
    QTcpSocket *socket;                          //通信用
    bool NetFlag=false;

    int serverId=WHITE;                               //网络对战,server端棋子颜色，先后手顺序标记


private slots:
    void aiplacepiece();                             //AI下棋
    void slogRecv();                                 //接收客户端信息

    void on_pushButton_ai_clicked();
    void on_stop_clicked();

};
#endif // MAINWINDOW_H
