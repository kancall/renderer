﻿#include "tgaimage.h"
#include<iostream>
#include "model.h"
#include"geometry.h"
#include<vector>
#define _USE_MATH_DEFINES
#include<math.h>

using namespace std;

TGAColor white(255, 255, 255, 255);
TGAColor red(255, 0, 0, 255);
TGAColor green(0, 255, 0, 255);
TGAColor blue(0, 0, 255, 255);
TGAColor yellow = TGAColor(255, 255, 0, 255);

const int width = 800;
const int height = 800;
const int depth = 255;

Vec3f camera(2, 1, 1);
Vec3f center(0, 0, 0);
Vec3f up(0, 1, 0);

/// <summary>
/// 求a、b向量的点乘
/// </summary>
/// <param name="a"></param>
/// <param name="b"></param>
/// <returns></returns>
float dot(Vec3f a, Vec3f b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

/// <summary>
/// 求a、b向量的叉乘
/// </summary>
/// <param name="a"></param>
/// <param name="b"></param>
Vec3f cross(Vec3f a, Vec3f b)
{
	return Vec3f(a.y * b.z - b.y * a.z, -a.x * b.z + b.x * a.z, a.x * b.y - b.x * a.y);
}

//将矩阵（matrix）变换成向量（vertor），和v2m是逆操作
Vec3f m2v(Matrix m) {
	return Vec3f(m[0][0] / m[3][0], m[1][0] / m[3][0], m[2][0] / m[3][0]);
}

//将向量（vertor）变换成矩阵（matrix），这个矩阵是齐次坐标
Matrix v2m(Vec3f v) {
	Matrix m(4, 1);
	m[0][0] = v.x;
	m[1][0] = v.y;
	m[2][0] = v.z;
	m[3][0] = 1.f;
	return m;
}

Matrix viewport(int x, int y, int w, int h) {
	Matrix m = Matrix::identity(4);
	m[0][3] = x + w / 2.f;
	m[1][3] = y + h / 2.f;
	m[2][3] = depth / 2.f;

	m[0][0] = w / 2.f;
	m[1][1] = h / 2.f;
	m[2][2] = depth / 2.f;
	return m;
}

Matrix translation(Vec3f v) {
	Matrix Tr = Matrix::identity(4);
	Tr[0][3] = v.x;
	Tr[1][3] = v.y;
	Tr[2][3] = v.z;
	return Tr;
}

//绕z轴旋转
Matrix rotation_z(float cosangle, float sinangle) {
	Matrix R = Matrix::identity(4);
	R[0][0] = R[1][1] = cosangle;
	R[0][1] = -sinangle;
	R[1][0] = sinangle;
	return R;
}

Matrix zoom(float factor) {
	Matrix Z = Matrix::identity(4);
	Z[0][0] = Z[1][1] = Z[2][2] = factor;
	return Z;
}

void line(Vec3f p0, Vec3f p1, TGAImage& image, TGAColor color) {
	bool steep = false;
	if (std::abs(p0.x - p1.x) < std::abs(p0.y - p1.y)) {
		std::swap(p0.x, p0.y);
		std::swap(p1.x, p1.y);
		steep = true;
	}
	if (p0.x > p1.x) {
		std::swap(p0, p1);
	}

	for (int x = p0.x; x <= p1.x; x++) {
		float t = (x - p0.x) / (float)(p1.x - p0.x);
		int y = p0.y * (1. - t) + p1.y * t + .5;
		if (steep) {
			image.set(y, x, color);
		}
		else {
			image.set(x, y, color);
		}
	}
}

//void line(Vec2i a, Vec2i b, TGAImage& image, TGAColor color)
//{
//	int x0 = a.x, y0 = a.y, x1 = b.x, y1 = b.y;
//	bool steep = false;
//	//把长的那条边放在y轴上
//	if (abs(x0 - x1) > abs(y0 - y1))
//	{
//		swap(x0, y0);
//		swap(x1, y1);
//		steep = true;
//	}
//	if (y0 > y1)
//	{
//		swap(x0, x1);
//		swap(y0, y1);
//	}
//
//	int i = x0, t = y0;
//	int dx = x1 - x0, dy = y1 - y0;
//
//	float rate = abs(dx / float(dy));
//	float pix = 0;
//	//不用float和除法的方法：
//	int rate2 = abs(dx) * 2;
//	int pix2 = 0;
//	while (t <= y1)
//	{
//		if (steep)
//			image.set(t, i, color);
//		else
//			image.set(i, t, color);
//		t++;
//
//		//旧方法：有重复的计算，效率低
//		//t在y轴走的百分比，乘上x轴的长度，就是偏移值
//		//i = (x1 - x0)*(t - y0) / (y1 - y0) + x0;
//		//或者i = (t - y0) / (float)(y1 - y0) * (x1 - x0) + x0;要加float，不然计算出来前面除法一直是0
//
//		//新方法：减少重复计算。判断斜率是否进位，进位就将x轴移动一位
//		/*pix += rate;
//		if (pix > .5)
//		{
//			i += (x1 > x0 ? 1 : -1);
//			pix -= 1;
//		}*/
//
//		//更新的方法：去掉除法，直接用dx、dy进行比较，效率更高。其实相当于上一个方法变量集体乘2倍dy
//		pix2 += rate2;
//		if (pix2 > dx && dx != 0)//后面这个判断用来处理水平、垂直线的斜率情况
//		{
//			i += (x1 > x0 ? 1 : -1);
//			pix2 -= dy * 2;
//		}
//	}
//}

void triangle1(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage& image, TGAColor color)
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
		int xA = t2.x + (t0.x - t2.x) * a;
		int xB = isHalf ? t1.x + (t0.x - t1.x) * b : t2.x + (t1.x - t2.x) * b;

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

/// <summary>
/// 计算该点在三角形中的重心坐标
/// </summary>
/// <param name="points"></param>
/// <param name="p"></param>
/// <returns></returns>
Vec3f barycentric(Vec3i* points, Vec2i p)
{
	//求出来这个点在三角形中对应的u v值，然后判断u v值在不在0 1范围内
	/*Vec3f uv = Vec3f(points[2].x - points[0].x, points[1].x - points[0].x, points[0].x - p.x) ^
		Vec3f(points[2].y - points[0].y, points[1].y - points[0].y, points[0].y - p.y);
	if (abs(uv.z < 1)) return Vec3f(-1, 1, 1);
	return Vec3f(1.f - (uv.x + uv.y) / uv.z, uv.y / uv.z, uv.x / uv.z);*/

	//根据https://zhuanlan.zhihu.com/p/361943207计算的，感觉原版代码有点怪怪的
	float u = (float)((p.y - points[2].y) * (points[0].x - points[2].x) - (p.x - points[2].x) * (points[0].y - points[2].y)) /
		((points[1].y - points[2].y) * (points[0].x - points[2].x) - (points[1].x - points[2].x) * (points[0].y - points[2].y));
	float v = (float)((p.x - points[2].x) * (points[2].y - points[1].y) - (p.y - points[2].y) * (points[2].x - points[1].x)) /
		((points[0].x - points[2].x) * (points[2].y - points[1].y) - (points[0].y - points[2].y) * (points[2].x - points[1].x));
	return Vec3f((1 - u - v), u, v);
}

void triangle(Model* model, Vec3i* points, Vec2i* uv, vector<vector<int>>& zbuffer, TGAImage& image, TGAColor color)
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
			Vec3f bc = barycentric(points, Vec2i(x, y));
			if (bc.x < 0 || bc.y < 0 || bc.z < 0) //得到的重心值不在0 1范围内，所以不在三角形内
				continue;
			Vec2i pixelUV = uv[0] * bc.x + uv[1] * bc.y + uv[2] * bc.z;
			//根据重心坐标三个分量，求点的z值
			float z = points[0].z * bc.x + points[0].z * bc.y + points[0].z * bc.z;
			if (zbuffer[x][y] < z)
			{
				zbuffer[x][y] = z;
				image.set(x, y, model->getTextureColor(pixelUV));
			}
		}
	}
}

Matrix lookAt(Vec3f camera, Vec3f center, Vec3f up)
{
	Vec3f z = (camera - center).normalize();
	Vec3f x = cross(up, z).normalize();
	Vec3f y = cross(z, x).normalize();

	Matrix Mr = Matrix::identity(4);
	Matrix Mt = Matrix::identity(4);
	for (int i = 0; i < 3; i++)
	{
		//平移矩阵
		Mt[i][3] = -center[i];
		//旋转矩阵
		Mr[0][i] = x[i];
		Mr[1][i] = y[i];
		Mr[2][i] = z[i];
	}
	return Mr * Mt;
}

void scene()
{
	Model* model = new Model("obj/cube.obj");

	TGAImage image(width, height, TGAImage::RGB);
	Matrix VP = viewport(width / 4, width / 4, width / 2, height / 2);

	{ // draw the axes
		Vec3f x(1.f, 0.f, 0.f), y(0.f, 1.f, 0.f), o(0.f, 0.f, 0.f);
		o = m2v(VP * v2m(o));
		x = m2v(VP * v2m(x));
		y = m2v(VP * v2m(y));
		line(o, x, image, red);
		line(o, y, image, green);
	}


	for (int i = 0; i < model->nfaces(); i++) {
		std::vector<int> face = model->face(i);
		for (int j = 0; j < (int)face.size(); j++) {
			Vec3f wp0 = model->vert(face[j]);
			Vec3f wp1 = model->vert(face[(j + 1) % face.size()]);

			{ // draw the original model
				Vec3f sp0 = m2v(VP * v2m(wp0));
				Vec3f sp1 = m2v(VP * v2m(wp1));
				line(sp0, sp1, image, white);
			}
			{ // draw the deformed model
				Matrix T = zoom(1.5);
				//                  Matrix T = Matrix::identity(4);
				//                  T[0][1] = 0.333;
				//                Matrix T = translation(Vec3f(.33, .5, 0))*rotation_z(cos(10.*M_PI/180.), sin(10.*M_PI/180.));
				Vec3f sp0 = m2v(VP * T * v2m(wp0));
				Vec3f sp1 = m2v(VP * T * v2m(wp1));
				line(sp0, sp1, image, yellow);
			}
		}
		break;
	}


	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	delete model;
}

int main()
{
	int width = 1024, height = 1024;
	TGAImage image(width, height, TGAImage::RGB);

	Vec3f lightDir(0, 0, -1);
	vector<vector<int>> zbuffer(width, vector<int>(height, INT_MIN)); //坐标是右手系，所以z轴是垂直屏幕向外的
	Model* model = new Model("obj/african_head.obj");

	//透视矩阵
	Matrix Projection = Matrix::identity(4);
	float c = 3; //摄像机距离
	Projection[3][2] = -1.f / c;
	//视角矩阵
	Matrix Viewport = viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
	//模型矩阵
	Matrix ModelView = lookAt(camera, center, up);

	for (int i = 0; i < model->nfaces(); i++)
	{
		vector<int> face = model->face(i);
		Vec3i screen[3];
		Vec3f world[3];
		Vec2i uv[3];
		for (int t = 0; t < 3; t++)
		{
			Vec3f v = model->vert(face[t]);
			Vec3f screenFloat = m2v(Viewport * Projection * ModelView * v2m(v)); //透视投影
			screen[t] = Vec3i(screenFloat[0], screenFloat[1], screenFloat[2]); //geometry.cpp的vec3i和vec3f之间的转换出了点问题，所以先手动转一下
			world[t] = v;
			uv[t] = model->uv(i, t);
		}
		Vec3f n = cross((world[2] - world[0]), (world[1] - world[0])); //叉乘三角形的边的向量求法向量
		n.normalize();
		float intensity = dot(n, lightDir); //用点乘表示两个向量之间的夹角大小，夹角越小越接近1，也说明光线越和平面垂直、平面越亮
		if (intensity >= 0)
		{
			Vec3i points[3] = { screen[0], screen[1], screen[2] };
			triangle(model, points, uv, zbuffer, image, TGAColor(intensity * 255, intensity * 255, intensity * 255, 255));
		}
	}

	image.flip_vertically();
	image.write_tga_file("output.tga");
	return 0;
}
