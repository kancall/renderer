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

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    Ui::RenderWindow *ui;
};

#endif // RENDERWINDOW_H
