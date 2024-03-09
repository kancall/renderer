#ifndef RENDERWINDOW_H
#define RENDERWINDOW_H

#include <QWidget>
#include <QPainter>
#include <vector>
#include "our_gl.h"
#include<QMouseEvent>

namespace Ui {
class RenderWindow;
}

class RenderWindow : public QWidget
{
    Q_OBJECT

public:
    explicit RenderWindow(QWidget *parent = nullptr);
    ~RenderWindow();

    QImage output;
    QPoint lastPoint;

    void setPointColor(int x,int y,QRgb color); //设置指定点为指定颜色
    void triangle(Vec3i* points, IShader& shader, vector<vector<int>>& zbuffer, TGAImage& image, bool isShow);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;


private:
    Ui::RenderWindow *ui;
};

#endif // RENDERWINDOW_H
