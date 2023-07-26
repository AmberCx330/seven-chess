#ifndef GAMESCREEN_H
#define GAMESCREEN_H
#include "mainwindow.h"


#include <QWidget>
#include <QPixmap>
#include <QPainter>
#include <QPaintEvent>


namespace Ui {
class gamescreen;
}

class gamescreen : public QWidget
{
    Q_OBJECT


public:
    explicit gamescreen(QWidget *parent = nullptr);
    ~gamescreen();


private:
    Ui::gamescreen *ui;
    MainWindow *screen;
    virtual void paintEvent(QPaintEvent *event);

};


#endif // GAMESCREEN_H
