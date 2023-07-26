#ifndef PIECE_H
#define PIECE_H

#endif // PIECE_H
class piece{                              //棋子类
    int locationx,locationy,player;      //x坐标，y坐标，棋子颜色

    public:
        piece()
        {
            locationx=0;
            locationy=0;
            player=-1;
        }

        void set(int a,int b,int c)
        {
            locationx=a;
            locationy=b;
            player=c;
        }

        void playerId(int x)
        {
            player=x;
        }

        int getx(){return locationx;}
        int gety(){return locationy;}
        int getplayer(){return player;}

};

