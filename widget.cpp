#include "widget.h"
#include <QDebug>
#include <QDir>
#include <QTimer>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);

    QImageReader::setAllocationLimit(256);
    QPixmap pix("land.png");
    pix = pix.scaled(this->size(), Qt::KeepAspectRatioByExpanding);
    QPalette palette;
    palette.setBrush(QPalette::Window, pix);
    this->setPalette(palette);

    player = new QMediaPlayer;

    k = fileSize(folder);

    numberVideo->setText(QString::number(n) + "/" + QString::number(k));
    numberVideo->baseSize();

    player->setSource(QUrl(folder + QString::number(n) + ".MOV"));

    // формируем шкалу крена и дифферента и добавляем картинки аппарата
    // PITCH

    scene1 = new QGraphicsScene(pitchView);
    pitchView->setScene(scene1);
    pitchView->setStyleSheet("background: transparent; border: none;");
    pitchView->setRenderHint(QPainter::Antialiasing);

    QGraphicsEllipseItem *circle = new QGraphicsEllipseItem();
    circle->setRect(0, 0, 500, 500); // Размер окружности
    circle->setPos(2.5, 2.5); // Центрируем

    QPen circlePen(Qt::black); // Цвет контура
    circle->setBrush(QBrush(QColor(0, 150, 255, 150))); // Заливка контура
    circlePen.setWidth(2); // Толщина линии
    circle->setPen(circlePen);
    scene1->addItem(circle);

    pitchDial = scene1->addPixmap(QPixmap("anglesPitch.png"));
    pitchRov = scene1->addPixmap(QPixmap("pitchRov.png"));

    pitchDial->setTransform(QTransform::fromScale(0.5, 0.5));
    pitchRov->setTransform(QTransform::fromScale(0.3, 0.3));
    pitchRov->setPos(85, 200);
    txtCurrentPitch = scene1->addText(QString::number(a, 'f', 1) + "°", QFont("Calibri", 24));
    txtCurrentPitch->setDefaultTextColor(QColor(0, 0, 0));

    QTransform t;
    t.translate(pitchDial->pixmap().width()/4-20, pitchDial->pixmap().height()/4-22);
    txtCurrentPitch->setTransform(t);
    pitchRov->setTransformOriginPoint(pitchRov->pixmap().width()/2, pitchRov->pixmap().height()/2);


    //ROLL

    scene2 = new QGraphicsScene(rollView);
    rollView->setScene(scene2);
    rollView->setStyleSheet("background: transparent; border: none;");
    rollView->setRenderHint(QPainter::Antialiasing);

    QGraphicsEllipseItem *circle1 = new QGraphicsEllipseItem();
    circle1->setRect(0, 0, 500, 500);
    circle1->setPos(2.5, 2.5);

    QPen circlePen1(Qt::black);
    circle1->setBrush(QBrush(QColor(0, 150, 255, 150)));
    circlePen1.setWidth(2);
    circle1->setPen(circlePen1);
    scene2->addItem(circle1);

    rollDial = scene2->addPixmap(QPixmap("anglesRoll.png"));
    rollRov = scene2->addPixmap(QPixmap("rollRov.png"));

    rollDial->setTransform(QTransform::fromScale(0.5,0.5));
    rollRov->setTransform(QTransform::fromScale(0.4, 0.4));
    rollRov->setPos(69, 155);
    txtCurrentRoll = scene2->addText(QString::number(a, 'f', 1) + "°", QFont("Times New Roman", 24));
    txtCurrentRoll->setDefaultTextColor(Qt::black);

    QTransform t1;
    t1.translate(rollDial->pixmap().width()/4-20, rollDial->pixmap().height()/4-22);
    txtCurrentRoll->setTransform(t1);
    rollRov->setTransformOriginPoint(rollRov->pixmap().width()/2, rollRov->pixmap().height()/2);

    QTimer *videoTimer = new QTimer(this);

    connect(videoTimer, &QTimer::timeout, this, [this]() {
        if (player->playbackState() == QMediaPlayer::PlayingState && flagMarker == 1) {
            timeCounter += 0.05;
            a = 10.0 * qSin(timeCounter);
            txtCurrentPitch->setPlainText(QString::number(a, 'f', 1) + "°");
            pitchRov->setRotation(-a);
            a = 5.0 * qSin(timeCounter);
            txtCurrentRoll->setPlainText(QString::number(a, 'f', 1) + "°");
            rollRov->setRotation(-a);
        }
    });

    videoTimer->start(50);

    connect(nextVideo, &QPushButton::clicked, [this](){
        if (n < k) n += 1;
        timeCounter = 0.0;
        numberVideo->setText(QString::number(n) + "/" + QString::number(k));
        player->setSource(QUrl(folder + QString::number(n) + ".MOV"));
        player->play();
        player->setVideoOutput(Video);
    });

    connect(prevVideo, &QPushButton::clicked, [this](){
        if (n > 1) n -= 1;
        timeCounter = 0.0;
        numberVideo->setText(QString::number(n) + "/" + QString::number(k));
        player->setSource(QUrl(folder + QString::number(n) + ".MOV"));
        player->play();
        player->setVideoOutput(Video);
    });

    connect(startBtn, &QPushButton::clicked,[this](){
        startFlag = 1;
        numberVideo->setText(QString::number(n) + "/" + QString::number(k));
        player->play();
        player->setVideoOutput(Video);
    });

    connect(stopBtn, &QPushButton::clicked,[this](){
        player->pause();
    });

    connect(player, &QMediaPlayer::metaDataChanged, [this]() {
        horizontalSlider->setSliderPosition(0);
        horizontalSlider->setMaximum(player->duration());
    });

    connect(player, &QMediaPlayer::positionChanged, [this]() {
        horizontalSlider->setSliderPosition(player->position());
    });

    connect(horizontalSlider, &QSlider::sliderMoved, [this]() {
        player->setPosition(horizontalSlider->sliderPosition());
    });

    connect(calibBtn, &QPushButton::clicked, [this](){
        flagMarker = 0;
        folder= "Calib/";
        k = fileSize(folder);
        player->stop();
        player->setSource(QUrl(folder + QString::number(1) + ".MOV"));
    });

    connect(markerBtn, &QPushButton::clicked, [this](){
        flagMarker = 1;
        folder = "Marker/";
        k = fileSize(folder);
        player->stop();
        player->setSource(QUrl(folder + QString::number(1) + ".MOV"));
    });

    connect(player, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::EndOfMedia && flagMarker == 0)
        {
            valueRMS->setText("0,6 Хороший результат");
        }
    });

    connect(player, &QMediaPlayer::playbackStateChanged, this, [this](QMediaPlayer::PlaybackState state) {
        if (state == QMediaPlayer::PlayingState && flagMarker == 0)
        {
            valueRMS->setText("Вычисление ошибки...");
        }
    });

    Video->show();

}

Widget::~Widget() {}

int Widget::fileSize(QString folder)
{
    QDir dir(folder);

    QStringList items = dir.entryList(QDir::NoDotAndDotDot | QDir::AllEntries);

    return items.count();
}
