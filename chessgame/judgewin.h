#ifndef JUDGEWIN_H
#define JUDGEWIN_H
#include "checkerboard.h"

#endif // JUDGEWIN_H
class judgement{

    public:
        bool judgedraw(checkerboard board)                  //判断和棋
        {
            if(board.getbnum()+board.getwnum()==441)
                return true;
            return false;
        }

        int FLAG=0;                                         //是否产生禁手标志
        bool judgewin(checkerboard board,int x,int y)       //判断输赢、禁手
        {
            FLAG=0;
            int directionx[8]={-1,1,0,0,-1,1,-1,1};         //上下，左右，左上右下，右上左下
            int directiony[8]={0,0,1,-1,-1,1,1,-1};
            int num[4]={1,1,1,1};
            int i=0,j=0,n6=0,n5=0,J6[4],J5[4];
            int p=board.showpiece(x,y);

            for(i=0,j=0;i<8;i++)                            //八个方向朴素遍历
            {
                int X=x+directionx[i];
                int Y=y+directiony[i];
                while(true)
                {
                    if(X<0||X>20||Y<0||Y>20)
                    break;
                    if(board.showpiece(X,Y)==p)
                    {
                        num[j]++;
                        X+=directionx[i];
                        Y+=directiony[i];
                    }
                    else
                    break;
                }
                if((i+1)%2==0)
                {
                    if(board.showpiece(x,y)==BLACK&&num[j]==6)       //标记可能产生六六禁手
                    {
                        J6[n6]=j;
                        n6++;
                    }
                    if(board.showpiece(x,y)==BLACK&&num[j]==5)       //标记可能产生五五禁手
                    {
                        J5[n5]=j;
                        n5++;
                    }
                    j++;
                }
            }

            for(int i=0;i<4;i++)           //赢棋
            {
                if(num[i]==7)
                {
                    return true;
                }
            }

            for(int i=0;i<4;i++)           //长连禁手
            {
                if(num[i]>7)
                    FLAG=1;
            }

            //六六禁手，六颗黑子旁边不可能为棋盘边界或白子
            if(n6>1)
            {
                int count=0;                //记录活六个数
                for(int i=0;i<n6;i++)
                {
                    int X=x+directionx[J6[i]*2];
                    int Y=y+directiony[J6[i]*2];
                    int flag=0;
                    while(true)
                    {
                        if(X<0||X>20||Y<0||Y>20)
                        {
                            flag=1;
                            break;
                        }
                        if(board.showpiece(X,Y)==WHITE)
                        {
                            flag=1;
                            break;
                        }
                        if(board.showpiece(X,Y)==-1)
                        {
                            break;
                        }
                        else
                        {
                            X+=directionx[J6[i]*2];
                            Y+=directiony[J6[i]*2];
                        }
                    }
                    if(flag)
                    {
                        continue;
                    }
                    else
                    {
                        int X=x+directionx[J6[i]*2+1];
                        int Y=y+directiony[J6[i]*2+1];
                        int flag=0;
                        while(true)
                        {
                            if(X<0||X>20||Y<0||Y>20)
                            {
                                flag=1;
                                break;
                            }
                            if(board.showpiece(X,Y)==WHITE)
                            {
                                flag=1;
                                break;
                            }
                            if(board.showpiece(X,Y)==-1)
                            {
                                break;
                            }
                            else
                            {
                                X+=directionx[J6[i]*2+1];
                                Y+=directiony[J6[i]*2+1];
                            }
                        }
                        if(flag)
                        {
                            continue;
                        }
                        else
                        {
                            count++;
                        }
                    }
                }
                if(count>1)
                {
                    FLAG=1;
                }
            }

            //五五禁手
            if(n5>1)
            {
                int count2=0;
                for(int i=0;i<n5;i++)
                {
                    int X=x+directionx[J5[i]*2];
                    int Y=y+directiony[J5[i]*2];
                    int flag=0;
                    while(true)
                    {
                        if(X<0||X>20||Y<0||Y>20)
                        {
                            flag=1;
                            break;
                        }
                        if(board.showpiece(X,Y)==WHITE)
                        {
                            flag=1;
                            break;
                        }
                        if(board.showpiece(X,Y)==-1)
                        {
                            break;
                        }
                        else
                        {
                            X+=directionx[J5[i]*2];
                            Y+=directiony[J5[i]*2];
                        }
                    }
                    if(flag)
                        continue;
                    else
                    {
                        int X=x+directionx[J5[i]*2+1];
                        int Y=y+directiony[J5[i]*2+1];
                        int flag=0;
                        while(true)
                        {
                            if(X<0||X>20||Y<0||Y>20)
                            {
                                flag=1;
                                break;
                            }
                            if(board.showpiece(X,Y)==WHITE)
                            {
                                flag=1;
                                break;
                            }
                            if(board.showpiece(X,Y)==-1)
                            {
                                break;
                            }
                            else
                            {
                                X+=directionx[J5[i]*2+1];
                                Y+=directiony[J5[i]*2+1];
                            }
                        }
                        if(flag)
                        {
                            continue;
                        }
                        else
                        {
                            count2++;
                        }
                    }
                }
                if(count2>1)
                {
                    FLAG=1;
                }
            }

            return false;
        }


        void judgevalue(checkerboard *board)            //AI评分函
        {
            int directionx[8]={-1,1,0,0,-1,1,-1,1};     //上下，左右，左上右下，右上左下
            int directiony[8]={0,0,1,-1,-1,1,1,-1};
            int numb,numw,empty1,empty2;

            for(int i=0;i<21;i++)
            {
                for(int j=0;j<21;j++)
                {
                    if(board->showpiece(i,j)==BLACK)      //黑棋评分
                    {
                        for(int k=0;k<8;k++)
                        {
                            int X=i,Y=j,X1,X2,Y1,Y2;     //X1,X2，Y1,Y2为空格坐标
                            if(k%2==0)
                            {   numb=1;
                                empty1=0;
                            }
                            X=i+directionx[k];
                            Y=j+directiony[k];
                            while(true)
                            {
                                if(X<0||X>20||Y<0||Y>20)
                                    break;
                                if(board->showpiece(X,Y)==BLACK)
                                {
                                    numb++;
                                    X+=directionx[k];
                                    Y+=directiony[k];
                                }
                                else if(board->showpiece(X,Y)==-1)
                                {
                                    empty1++;
                                    if(empty1==1)
                                    {
                                        X1=X;                //记录空格坐标
                                        Y1=Y;
                                    }
                                    else
                                    {
                                        X2=X;
                                        Y2=Y;
                                    }
                                    break;
                                }
                                else if(board->showpiece(X,Y)==WHITE)
                                {
                                    break;
                                }
                            }

                            if(k%2)
                            {
                                if(numb==1&&empty1!=0)
                                {
                                    if(empty1==1)
                                        board->value[X1][Y1]+=10;          //单棋
                                    else
                                    {
                                        board->value[X1][Y1]+=10;
                                        board->value[X2][Y2]+=10;
                                    }
                                }

                                if(numb==2&&empty1!=0)
                                {
                                    if(empty1==1)
                                            board->value[X1][Y1]+=10;       //死二
                                    else
                                    {
                                            board->value[X1][Y1]+=20;       //活二
                                            board->value[X2][Y2]+=20;
                                    }
                                }

                                if(numb==3&&empty1!=0)
                                {
                                    if(empty1==1)
                                            board->value[X1][Y1]+=30;        //死三
                                    else
                                    {
                                            board->value[X1][Y1]+=50;        //活三
                                            board->value[X2][Y2]+=50;
                                    }
                                }

                                if(numb==4&&empty1!=0)
                                {
                                    if(empty1==1)
                                            board->value[X1][Y1]+=50;        //死四
                                    else
                                    {
                                            board->value[X1][Y1]+=1000;      //活四
                                            board->value[X2][Y2]+=1000;
                                    }
                                }

                                if(numb==5&&empty1!=0)
                                {
                                    if(empty1==1)
                                            board->value[X1][Y1]+=1000;     //死五，原10000
                                    else
                                    {
                                            board->value[X1][Y1]+=5000;    //活五，原10000
                                            board->value[X2][Y2]+=5000;     //原10000
                                    }
                                }
                                if(numb==6&&empty1!=0)                        //活六
                                {
                                    if(empty1==1)
                                            board->value[X1][Y1]+=10000;
                                    else
                                    {
                                            board->value[X1][Y1]+=10000;
                                            board->value[X2][Y2]+=10000;
                                    }
                                }

                            }
                        }
                    }


                    if(board->showpiece(i,j)==WHITE)               //白棋评分
                    {
                        for(int k=0;k<8;k++)
                        {
                            int X=i,Y=j,X1,X2,Y1,Y2;
                            if(k%2==0)
                            {   numw=1;
                                empty2=0;
                            }
                            X=i+directionx[k];
                            Y=j+directiony[k];
                            while(true)
                            {
                                if(X<0||X>20||Y<0||Y>20)
                                    break;
                                if(board->showpiece(X,Y)==WHITE)
                                {
                                    numw++;
                                    X+=directionx[k];
                                    Y+=directiony[k];
                                }
                                else if(board->showpiece(X,Y)==-1)
                                {
                                    empty2++;
                                    if(empty2==1)
                                    {
                                        X1=X;                    //记录空格坐标
                                        Y1=Y;
                                    }
                                    else
                                    {
                                        X2=X;
                                        Y2=Y;
                                    }
                                    break;
                                }
                                else
                                {
                                    break;
                                }
                            }

                            if(k%2)
                            {
                                if(numw==1&&empty2!=0)
                                {
                                    if(empty2==1)                         //单棋
                                    {
                                        board->value[X1][Y1]+=10;
                                    }
                                    else
                                    {
                                            board->value[X1][Y1]+=10;
                                            board->value[X2][Y2]+=10;
                                    }
                                }

                                if(numw==2&&empty2!=0)
                                {
                                    if(empty2==1)                        //死二
                                    {
                                        board->value[X1][Y1]+=15;
                                    }
                                    else
                                    {
                                            board->value[X1][Y1]+=20;      //活二
                                            board->value[X2][Y2]+=20;
                                    }
                                }

                                if(numw==3&&empty2!=0)
                                {
                                    if(empty2==1)
                                    {
                                        board->value[X1][Y1]+=30;          //死三
                                    }
                                    else
                                    {
                                            board->value[X1][Y1]+=50;       //活三
                                            board->value[X2][Y2]+=50;
                                    }
                                }

                                if(numw==4&&empty2!=0)
                                {
                                    if(empty2==1)
                                    {
                                        board->value[X1][Y1]+=200;         //死四
                                    }
                                    else
                                    {
                                            board->value[X1][Y1]+=2000;     //活四
                                            board->value[X2][Y2]+=2000;
                                    }
                                }

                                if(numw==5&&empty2!=0)
                                {
                                    if(empty2==1)
                                    {
                                        board->value[X1][Y1]+=2000;    //死五
                                    }
                                    else
                                    {
                                            board->value[X1][Y1]+=10000;    //活五
                                            board->value[X2][Y2]+=10000;
                                    }
                                }
                                if(numb==6&&empty1!=0)                        //活六
                                {
                                    if(empty1==1)
                                    {
                                        board->value[X1][Y1]+=20000;
                                    }
                                    else
                                    {
                                            board->value[X1][Y1]+=20000;
                                            board->value[X2][Y2]+=20000;
                                    }
                                }
                            }
                        }
                    }

                }
            }
        }
};

