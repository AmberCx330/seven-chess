#ifndef CHECKERBOARD_H
#define CHECKERBOARD_H

#include <QMessageBox>
#include <QDebug>
#include <iostream>
#include "piece.h"
using namespace std;

#define BLACK 1
#define WHITE 2

enum gamemode{ Normal,AI,ai,Net };          //游戏模式


struct Node{                             //每个棋子坐标
    int x=-1,y=-1;
};


class checkerboard{                           //棋盘类
    int size=20;
    piece qz[22][22];                          //多申请空间，避免越界
    Node nodeblack[221],nodewhite[221];       //每下一个棋子便将坐标存入相应的数组
    int blacknum=0,whitenum=0;                            //记录数组中棋子个数

    public:
        int value[22][22];                    //每个空位的评分记录

        checkerboard()
        {
            for(int i=0;i<22;i++)
            {
                for(int j=0;j<22;j++)
                {
                    value[i][j]=0;
                    qz[i][j].set(i,j,-1);          //初始化棋盘，虚拟棋子，物理上不存在，逻辑上存在
                }
            }
        }

        void placepiece(int x,int y,int p)          //下棋函数
        {
            if(qz[x][y].getplayer()!=-1)
            return;
            if(x<0||x>20||y<0||y>20)
            return;

            qz[x][y].playerId(p);
            value[x][y]=0;

            if(p==BLACK)                          //放入棋子路径数组
            {
                blacknum++;
                nodeblack[blacknum].x=x;
                nodeblack[blacknum].y=y;
            }
            if(p==WHITE)
            {
                whitenum++;
                nodewhite[whitenum].x=x;
                nodewhite[whitenum].y=y;
            }
        }

        int showpiece(int x,int y)                 //展示指定点棋子的playerId
        {
            if(x<0||x>20||y<0||y>20)
            return -1;            
            return qz[x][y].getplayer();
        }

        void setundoqz(int x,int y)               //悔棋后设置PlayerId为-1
        {
            qz[x][y].playerId(-1);
        }

        void undo(int p)                          //悔棋函数
        {
            if(p==BLACK)
            {
                setundoqz(nodeblack[blacknum].x,nodeblack[blacknum].y);
                nodeblack[blacknum].x=-1;
                nodeblack[blacknum].y=-1;
                blacknum--;
            }
            if(p==WHITE)
            {
                setundoqz(nodewhite[whitenum].x,nodewhite[whitenum].y);
                nodewhite[whitenum].x=-1;
                nodewhite[whitenum].y=-1;
                whitenum--;
            }
        }

        int getbnum()                       //获取已下棋子个数
        {
            return blacknum;
        }
        int getwnum()
        {
            return whitenum;
        }

};






#endif // CHECKERBOARD_H
