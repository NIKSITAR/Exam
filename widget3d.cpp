#include "widget3d.h"
#include "ui_widget3d.h"
#include <QVBoxLayout>
#include <QtDataVisualization>

Widget3D::Widget3D(QWidget *parent)
    : QWidget(parent),
    graph(nullptr),
    pointROV(nullptr),
    pointDS(nullptr)
{
    setupUi(this);
    graph = new Q3DScatter();
    Q3DTheme *theme = new Q3DTheme();
    theme->setBackgroundColor(QColor(0, 150, 255, 200));
    theme->setWindowColor(QColor(0, 200, 255, 0));
    theme->setLabelTextColor(QColor(0, 0, 0));
    theme->setLabelBackgroundColor(QColor(0, 200, 255));
    graph->setActiveTheme(theme);

    QWidget *container = QWidget::createWindowContainer(graph);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(container);

    pointDS = new QScatter3DSeries();
    pointROV = new QScatter3DSeries();

    pointDS->setBaseColor(Qt::blue);
    pointROV->setBaseColor(Qt::red);

    pointDS->setItemSize(0.1f);
    pointROV->setItemSize(0.1f);

    graph->addSeries(pointDS);
    graph->addSeries(pointROV);

    QScatterDataArray data;
    data << QVector3D(0.0f, 0.0f, 0.0f);

    QScatterDataArray data2;
    data2 << QVector3D(2.0f, 2.0f, 2.0f);

    pointDS->dataProxy()->addItems(data);

    graph->axisX()->setRange(-4.0f, 4.0f);
    graph->axisY()->setRange(-4.0f, 4.0f);
    graph->axisZ()->setRange(-4.0f, 4.0f);
}

Widget3D::~Widget3D() {}
