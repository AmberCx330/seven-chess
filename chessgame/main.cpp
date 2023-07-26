#include "mainwindow.h"
#include "gamescreen.h"
#include <QApplication>
#include <QPalette>
#include <QColor>
#include <QPainter>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);



    gamescreen w;


    w.setWindowIcon(QIcon(":/icon1.jpg"));
    w.autoFillBackground();

    w.show();



    return a.exec();

}
