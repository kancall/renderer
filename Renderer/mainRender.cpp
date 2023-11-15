#include "tgaimage.h"
#include<iostream>
#include "model.h"
#include"geometry.h"
#include<vector>

using namespace std;



TGAColor white(255, 255, 255, 255);
TGAColor red(255, 0, 0, 255);
TGAColor green(0, 255, 0, 255);
void line(Vec2i a, Vec2i b, TGAImage&image,TGAColor color)
{
	int x0 = a.x, y0 = a.y, x1 = b.x, y1 = b.y;
	bool steep = false;
	//把长的那条边放在y轴上
	if (abs(x0 - x1) > abs(y0 - y1))
	{
		swap(x0, y0);
		swap(x1, y1);
		steep = true;
	}
	if (y0 > y1)
	{
		swap(x0, x1);
		swap(y0, y1);
	}

	int i = x0, t = y0;
	int dx = x1 - x0, dy = y1 - y0;

	float rate = abs(dx / float(dy));
	float pix = 0;
	//不用float和除法的方法：
	int rate2 = abs(dx) * 2;
	int pix2 = 0;
	while (t <= y1)
	{
		if(steep)
			image.set(t, i, color);
		else
			image.set(i, t, color);
		t++;

		//旧方法：有重复的计算，效率低
		//t在y轴走的百分比，乘上x轴的长度，就是偏移值
		//i = (x1 - x0)*(t - y0) / (y1 - y0) + x0;
		//或者i = (t - y0) / (float)(y1 - y0) * (x1 - x0) + x0;要加float，不然计算出来前面除法一直是0
		
		//新方法：减少重复计算。判断斜率是否进位，进位就将x轴移动一位
		/*pix += rate;
		if (pix > .5)
		{
			i += (x1 > x0 ? 1 : -1);
			pix -= 1;
		}*/

		//更新的方法：去掉除法，直接用dx、dy进行比较，效率更高。其实相当于上一个方法变量集体乘2倍dy
		pix2 += rate2;
		if (pix2 > dx && dx != 0)//后面这个判断用来处理水平、垂直线的斜率情况
		{
			i += (x1 > x0 ? 1 : -1);
			pix2 -= dy * 2;
		}
	}
}

void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage&image, TGAColor color)
{
	if (t0.y < t1.y) swap(t0, t1);
	if (t0.y < t2.y) swap(t0, t2);
	if (t1.y < t2.y) swap(t1, t2);

	int totalHeight = t0.y - t2.y;
	for (int y = t2.y; y <= t0.y; y++)
	{
		bool isHalf = y >= t1.y ? true : false;
		int midHeight = isHalf ? t0.y - t1.y + 1 : t1.y - t2.y + 1;
		float a = (float)(y - t2.y) / totalHeight;
		float b = isHalf ? (float)(y - t1.y) / midHeight : (float)(y - t2.y) / midHeight;
		int xA = t2.x + (t0.x - t2.x)*a;
		int xB = isHalf ? t1.x + (t0.x - t1.x)*b : t2.x + (t1.x - t2.x)*b;

		if (xA > xB) swap(xA, xB);
		for (int i = xA; i <= xB; i++)
		{
			image.set(i, y, color);
		}
	}
	//下半部分三角
	/*int midHeight = t1.y - t2.y + 1; //下面要除，避免分母是0
	for (int y = t2.y; y <= t1.y; y++)
	{
		int nowHeight = y - t2.y;
		float a = (float)nowHeight / totalHeight;
		float b = (float)nowHeight / midHeight;
		int xA = t2.x + (t0.x - t2.x)*a;
		int xB = t2.x + (t1.x - t2.x)*b;

		if (xA > xB) swap(xA, xB);
		for (int i = xA; i <= xB; i++)
		{
			image.set(i, y, color);
		}
	}
	//上半部分
	midHeight = t0.y - t1.y + 1;
	for (int y = t1.y; y <= t0.y; y++)
	{
		float a = (float)(y - t2.y) / totalHeight;
		float b = (float)(y - t1.y) / midHeight;
		int xA = t2.x + (t0.x - t2.x)*a;
		int xB = t1.x + (t0.x - t1.x)*b;

		if (xA > xB) swap(xA, xB);
		for (int i = xA; i <= xB; i++)
		{
			image.set(i, y, color);
		}
	}*/
}

int main()
{
	cout << 1;
	int width = 800, height = 800;
	TGAImage image(width, height, TGAImage::RGB);
	/*Model* model = new Model("obj/african_head.obj");
	for (int i = 0; i < model->nfaces(); i++)
	{
		vector<int> face = model->face(i);
		for (int t = 0; t < 3; t++)
		{
			Vec3f v0 = model->vert(face[t]);
			Vec3f v1 = model->vert(face[(t + 1) % 3]);
			int x0 = (v0.x + 1.0)*width / 2.0;
			int y0 = (v0.y + 1.0)*height / 2.0;
			int x1 = (v1.x + 1.0)*width / 2.0;
			int y1 = (v1.y + 1.0)*height / 2.0;
			line(x0, y0, x1, y1, image, white);
		}
	}*/
	triangle(Vec2i(200, 400), Vec2i(400, 450), Vec2i(300, 600), image, white);
	image.flip_vertically();
	image.write_tga_file("output.tga");
	return 0;
}
