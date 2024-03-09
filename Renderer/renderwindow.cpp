#include "renderwindow.h"
#include "ui_renderwindow.h"
#include "mainRender.cpp"
#include "QDebug"
#include "our_gl.h"
#include<QtMath>

RenderWindow::RenderWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RenderWindow)
{
    output = QImage(800, 800, QImage::Format_RGB32);
    output.fill(Qt::black);
//    myRender();
//    if(output != NULL)
//    {
//        QPainter painter(this);
//        painter.drawImage(0,0,output);
//    }

    myRender(this);

}

RenderWindow::~RenderWindow()
{
    delete ui;
}

void RenderWindow::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
//    QRgb value = qRgb(255, 0, 0);
//    for(int i=50; i<100; ++i)
//    {
//        for(int j=50; j<100; ++j)
//        {
//            QRgb value = qRgb(255, 0, 0); // 红色
//            output.setPixel(i, j, value);
//        }
//     }
    painter.drawImage(0,0,output);

}

void RenderWindow::mousePressEvent(QMouseEvent *event)
{
    qDebug()<<"press"<<endl;
    lastPoint = event->pos();
}

void RenderWindow::mouseMoveEvent(QMouseEvent *event)
{
    output.fill(Qt::black);
    //移动有点问题，向左移动都没啥用，要改改。而且移动后，原来图片上的东西还绘制了，没有被清掉
    QPoint move=event->pos() - lastPoint;
    float x=(move.x()+camera.x)/100.f;
    float y=(move.y()+camera.y)/100.f;
    camera.x = 20 * qSin(x);
    camera.y = 20 * qSin(y);
    camera.z = qSqrt(400 - camera.x*camera.x - camera.y*camera.y);
    if(1.5>x>1 || 1.5>y>1)
        camera.z=-1*qAbs(camera.z);
    myRender(this);
    update();
}

void RenderWindow::mouseReleaseEvent(QMouseEvent *event)
{
    qDebug()<<"release"<<endl;
}

void RenderWindow::setPointColor(int x,int y,QRgb color)
{
    //qt的image的(0,0)在左上角，所以要更换一下点的位置
    output.setPixel(x, output.height() - y, color);
}

Vec3f barycentric(Vec3i* points, Vec2i p)
{
    //根据games101 p9 16分12的公式算的
    float u0 = (float)((p.y - points[1].y) * (points[2].x - points[1].x) - (p.x - points[1].x) * (points[2].y - points[1].y));
    float u1= ((points[0].y - points[1].y) * (points[2].x - points[0].x) - (points[0].x - points[1].x) * (points[2].y - points[0].y));
    float v0 = (float)((p.y - points[2].y) * (points[0].x - points[2].x) - (p.x - points[2].x) * (points[0].y - points[2].y));
    float v1= ((points[0].x - points[2].x) * (points[1].y - points[2].y) - (points[0].y - points[2].y) * (points[1].x - points[2].x));

    //要注意除法被除数不能为0，不然会计算出一个-nan(ind)的数字！一定要判断一下是否为0
    if (u1 == 0 || v1 == 0)
        return Vec3f(-1, 1, 1);

    float u = u0 / u1;
    float v = v0 / v1;
    return Vec3f(u, v, (1 - u - v));
}

void RenderWindow::triangle(Vec3i* points, IShader& shader, vector<vector<int>>& zbuffer, TGAImage& image, bool isShow)
{
    Vec2i bboxMax = Vec2i(0, 0), bboxMin = Vec2i(image.get_width() - 1, image.get_height() - 1);

    for (int i = 0; i < 3; i++)
    {
        //外面那层大小比较是看看超出边界没
        bboxMin.x = max(0, min(bboxMin.x, points[i].x));
        bboxMin.y = max(0, min(bboxMin.y, points[i].y));

        bboxMax.x = min(image.get_width() - 1, max(bboxMax.x, points[i].x));
        bboxMax.y = min(image.get_height() - 1, max(bboxMax.y, points[i].y));
    }

    for (int x = bboxMin.x; x <= bboxMax.x; x++)
    {
        for (int y = bboxMin.y; y <= bboxMax.y; y++)
        {
            Vec3f bc = barycentric(points, Vec2i(x, y)); //这里传入重心坐标计算公式的p点是个二维值
            if (bc.x < 0 || bc.y < 0 || bc.z < 0) //得到的重心值不在0 1范围内，所以不在三角形内
                continue;
            //根据重心坐标三个分量，求点的z值
            float z = points[0].z * bc.x + points[1].z * bc.y + points[2].z * bc.z;

            TGAColor color;
            bool clip = shader.fragment(bc, color);
            if (zbuffer[x][y] < z && !clip)
            {
                zbuffer[x][y] = z;
                image.set(x, y, color);

                //qt的图片赋值
                if(!isShow)
                    continue;
                QRgb value=qRgb(int(color[2]),int(color[1]),int(color[0]));
                setPointColor(x, y, value);
            }
        }
    }
}
