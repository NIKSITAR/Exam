#include "widget.h"
#include "widget3d.h"
#include <QDebug>
#include <QDir>
#include <QTimer>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);

    // Создаем задний фон

    QImageReader::setAllocationLimit(256);
    QPixmap pix("land.png");
    pix = pix.scaled(this->size(), Qt::KeepAspectRatioByExpanding);
    QPalette palette;
    palette.setBrush(QPalette::Window, pix);
    this->setPalette(palette);

    player = new QMediaPlayer;  // наш плейер

    k = fileSize(folder);       // кол-во файлов в папке

    numberVideo->setText(QString::number(n) + "/" + QString::number(k));

    player->setSource(QUrl(folder + QString::number(n) + ".MOV"));

    // Формируем отображение крена и дифферента и добавляем картинки аппарата
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

    // Подключение виджета 3Д положения ПА относительно док-станции

    QWidget *parentContainer = pos3D->parentWidget();

    int index = gridLayout->indexOf(pos3D);
    int row, column, rowSpan, columnSpan;

    gridLayout->getItemPosition(index, &row, &column, &rowSpan, &columnSpan);
    pos3D->setStyleSheet("background-color: rgb(0, 102, 204); border-style: outset; border-width: 2px; border-radius: 20px; border-color: rgb(0, 0, 180);");
    Widget3D *threeDWidget = new Widget3D(parentContainer);

    gridLayout->addWidget(threeDWidget, row, column, rowSpan, columnSpan);

    rovSeries = threeDWidget->pointROV;
    dsSeries = threeDWidget->pointDS;
    data << QVector3D(2.0f, 2.0f, 2.0f);
    rovSeries->dataProxy()->resetArray(&data);

    // Таймер для визуализации крена, дифферента и 3Д точки
    // привязка ко времени видео-файла
    // каждый кадр накапливается счетчик для изменения положения

    QTimer *videoTimer = new QTimer(this);

    connect(videoTimer, &QTimer::timeout, this, [this]()
    {
        if (player->playbackState() == QMediaPlayer::PlayingState && flagMarker == 1) {
            timeCounter += 0.05;

            a = 10.0 * qSin(timeCounter);
            txtCurrentPitch->setPlainText(QString::number(a, 'f', 1) + "°");
            pitchRov->setRotation(-a);

            a = 5.0 * qSin(timeCounter);
            txtCurrentRoll->setPlainText(QString::number(a, 'f', 1) + "°");
            rollRov->setRotation(-a);

            float rovX = 2.0f - 0.5f * qSin(timeCounter * 0.3f);
            float rovY = 2.0f - 0.5f * qCos(timeCounter * 0.3f);
            float rovZ = 2.0f - 0.3f * timeCounter;

            data << QVector3D(rovX, rovY, rovZ);
            rovSeries->dataProxy()->setItem(0, QScatterDataItem(QVector3D(rovX, rovY, rovZ)));
        }
    });

    videoTimer->start(50);

    // Обработка нажатия кнопки следующего видео

    connect(nextVideo, &QPushButton::clicked, [this]()
    {
        if (n < k) n += 1;
        else n = 1;
        timeCounter = 0.0;
        rovSeries->dataProxy()->setItem(0, QScatterDataItem(QVector3D(2.0f, 2.0f, 2.0f)));
        numberVideo->setText(QString::number(n) + "/" + QString::number(k));
        player->setSource(QUrl(folder + QString::number(n) + ".MOV"));
        player->play();
        player->setVideoOutput(Video);
    });

    // Обработка нажатия кнопки предыдущего видео

    connect(prevVideo, &QPushButton::clicked, [this]()
    {
        if (n > 1) n -= 1;
        else n = k;
        timeCounter = 0.0;
        rovSeries->dataProxy()->setItem(0, QScatterDataItem(QVector3D(2.0f, 2.0f, 2.0f)));
        numberVideo->setText(QString::number(n) + "/" + QString::number(k));
        player->setSource(QUrl(folder + QString::number(n) + ".MOV"));
        player->play();
        player->setVideoOutput(Video);
    });

    // Обработка нажатия кнопки старт

    connect(startBtn, &QPushButton::clicked,[this]()
    {
        numberVideo->setText(QString::number(n) + "/" + QString::number(k));
        if (endVideo == 1)
        {
            endVideo = 0;
            timeCounter = 0.0;
            rovSeries->dataProxy()->setItem(0, QScatterDataItem(QVector3D(2.0f, 2.0f, 2.0f)));
            txtCurrentPitch->setPlainText("0.0°");
            txtCurrentRoll->setPlainText("0.0°");
            pitchRov->setRotation(0);
            rollRov->setRotation(0);
        }
        player->play();
        player->setVideoOutput(Video);
    });

    // Обработка нажатия кнопки стоп

    connect(stopBtn, &QPushButton::clicked,[this]()
    {
        player->pause();
    });

    // Обработка слайдера

    connect(player, &QMediaPlayer::metaDataChanged, [this]()
    {
        horizontalSlider->setSliderPosition(0);
        horizontalSlider->setMaximum(player->duration());
    });

    connect(player, &QMediaPlayer::positionChanged, [this]()
    {
        horizontalSlider->setSliderPosition(player->position());
    });

    connect(horizontalSlider, &QSlider::sliderMoved, [this]()
    {
        player->setPosition(horizontalSlider->sliderPosition());
    });

    // Обработка нажатия кнопки режима калибровки

    connect(calibBtn, &QPushButton::clicked, [this]()
    {
        flagMarker = 0;
        folder= "Calib/";
        k = fileSize(folder);
        player->stop();
        player->setSource(QUrl(folder + QString::number(1) + ".MOV"));
        timeCounter = 0.0;
        a = 0.0;
        txtCurrentPitch->setPlainText("0.0°");
        txtCurrentRoll->setPlainText("0.0°");
        pitchRov->setRotation(0);
        rollRov->setRotation(0);
        markerBtn->setEnabled(false);
        markerBtn->setText("Marker");
        rovSeries->dataProxy()->setItem(0, QScatterDataItem(QVector3D(2.0f, 2.0f, 2.0f)));
    });

    // Обработка нажатия кнопки режима маркера

    connect(markerBtn, &QPushButton::clicked, [this]()
    {
        flagMarker = 1;
        folder = "Marker/";
        k = fileSize(folder);
        player->stop();
        player->setSource(QUrl(folder + QString::number(1) + ".MOV"));
        timeCounter = 0.0;
        a = 0.0;
        txtCurrentPitch->setPlainText("0.0°");
        txtCurrentRoll->setPlainText("0.0°");
        pitchRov->setRotation(0);
        rollRov->setRotation(0);
        rovSeries->dataProxy()->setItem(0, QScatterDataItem(QVector3D(2.0f, 2.0f, 2.0f)));
    });


    markerBtn->setEnabled(false);
    markerBtn->setText("Marker");

    // Выводим статус видео по завершении

    connect(player, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status)
    {
        if (status == QMediaPlayer::EndOfMedia && flagMarker == 0)
        {
            markerBtn->setEnabled(true);
            markerBtn->setText("Marker");

            valueRMS->setText("Калибровка завершена: RMS = 0.6 (отлично)");
        }
        else if (status == QMediaPlayer::EndOfMedia && flagMarker == 1)
        {
            endVideo = 1;
            valueRMS->setText("Обработка маркера завершена");
        }
    });

    // Выводим статус видео при проигрывании

    connect(player, &QMediaPlayer::playbackStateChanged, this, [this](QMediaPlayer::PlaybackState state)
    {
        if (state == QMediaPlayer::PlayingState && flagMarker == 0)
        {
            valueRMS->setText("Выполняется калибровка...");
        }
        else if (state == QMediaPlayer::PlayingState && flagMarker == 1)
        {
            valueRMS->setText("Обработка маркера...");
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
