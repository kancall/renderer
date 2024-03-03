#include "renderwindow.h"
#include "ui_renderwindow.h"
#include "mainRender.cpp"

RenderWindow::RenderWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RenderWindow)
{
//    myRender();
//    if(output != NULL)
//    {
//        QPainter painter(this);
//        painter.drawImage(0,0,output);
//    }
    painter = new QPainter(this);
    output = QImage(800, 800, QImage::Format_RGB32);
    output.fill(Qt::white);

    myRender();

}

RenderWindow::~RenderWindow()
{
    delete ui;
}

void RenderWindow::paintEvent(QPaintEvent* event)
{

//    QRgb value = qRgb(255, 0, 0);
//    for(int i=50; i<100; ++i)
//    {
//        for(int j=50; j<100; ++j)
//        {
//            QRgb value = qRgb(255, 0, 0); // 红色
//            output.setPixel(i, j, value);
//        }
//     }
//    painter->drawImage(0,0,output);

//    setPointColor(50,50, value);
}

void RenderWindow::setPointColor(int x,int y,QRgb color)
{
    output.setPixel(x, y, color);

    painter->drawImage(0,0,output);

}
