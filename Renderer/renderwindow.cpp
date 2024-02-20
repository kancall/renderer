#include "renderwindow.h"
#include "ui_renderwindow.h"
#include <QDebug>

RenderWindow::RenderWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RenderWindow)
{
//    ui->setupUi(this);

//    if(output)
//    {
//        QPainter painter(this);
//        painter.drawImage(0,0,output);
//    }
}

RenderWindow::~RenderWindow()
{
    delete ui;
}

void RenderWindow::paintEvent(QPaintEvent* event)
{
    QImage output = QImage(300, 300, QImage::Format_RGB32);
//    output.fill(Qt::red);
    for(int i=50; i<100; ++i)
    {
        for(int j=50; j<100; ++j)
        {
            QRgb value = qRgb(255, 0, 0); // 红色
            output.setPixel(i, j, value);
        }
     }
    QPainter painter(this);
    painter.drawImage(0,0,output);
}
