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
	//�ѳ��������߷���y����
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
	//����float�ͳ����ķ�����
	int rate2 = abs(dx) * 2;
	int pix2 = 0;
	while (t <= y1)
	{
		if(steep)
			image.set(t, i, color);
		else
			image.set(i, t, color);
		t++;

		//�ɷ��������ظ��ļ��㣬Ч�ʵ�
		//t��y���ߵİٷֱȣ�����x��ĳ��ȣ�����ƫ��ֵ
		//i = (x1 - x0)*(t - y0) / (y1 - y0) + x0;
		//����i = (t - y0) / (float)(y1 - y0) * (x1 - x0) + x0;Ҫ��float����Ȼ�������ǰ�����һֱ��0
		
		//�·����������ظ����㡣�ж�б���Ƿ��λ����λ�ͽ�x���ƶ�һλ
		/*pix += rate;
		if (pix > .5)
		{
			i += (x1 > x0 ? 1 : -1);
			pix -= 1;
		}*/

		//���µķ�����ȥ��������ֱ����dx��dy���бȽϣ�Ч�ʸ��ߡ���ʵ�൱����һ���������������2��dy
		pix2 += rate2;
		if (pix2 > dx && dx != 0)//��������ж���������ˮƽ����ֱ�ߵ�б�����
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
	//�°벿������
	/*int midHeight = t1.y - t2.y + 1; //����Ҫ���������ĸ��0
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
	//�ϰ벿��
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
