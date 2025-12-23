#ifndef WIDGET3D_H
#define WIDGET3D_H

#include "ui_widget3d.h"

#include <QtDataVisualization>
#include <QVBoxLayout>

class Widget3D : public QWidget, private Ui::Widget3D
{
    Q_OBJECT

public:
    explicit Widget3D(QWidget *parent = nullptr);
    ~Widget3D();
    Q3DScatter *graph;
    QScatter3DSeries *pointROV;
    QScatter3DSeries *pointDS;
private:

};

#endif // WIDGET3D_H
