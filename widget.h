#ifndef WIDGET_H
#define WIDGET_H

#include "ui_widget.h"

#include <QWidget>
#include <QImageReader>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QGraphicsTextItem>

class Widget : public QWidget, private Ui::Widget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    QString folder = "Calib/";
    QStringList items;
    int fileSize(QString folder);
    int k;
    int flagMarker = 0;
    int startFlag = 1;
    int videoSize = 0;
    int videoTime = 0;
    double timeCounter = 0;
    double a = 0;

private:
    QMediaPlayer *player;
    QGraphicsScene *scene1;
    QGraphicsPixmapItem *pitchRov;
    QGraphicsPixmapItem *pitchDial;
    QGraphicsTextItem *txtCurrentPitch;
    QGraphicsScene *scene2;
    QGraphicsPixmapItem *rollRov;
    QGraphicsPixmapItem *rollDial;
    QGraphicsTextItem *txtCurrentRoll;
    int n = 1;
};
#endif // WIDGET_H
