#ifndef RENDERWINDOW_H
#define RENDERWINDOW_H

#include <QWidget>
#include <QPainter>

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
    QPainter* painter;

    void setPointColor(int x,int y,QRgb color); //设置指定点为指定颜色

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    Ui::RenderWindow *ui;
};

#endif // RENDERWINDOW_H
