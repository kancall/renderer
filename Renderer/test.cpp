#include <vector>
#include <iostream>
#include <algorithm>
#include "geometry.h"
using namespace std;

Matrix Projection;
Matrix Viewport;
Matrix ModelView;
Vec3f camera(2, 1, 4);
Vec3f center(0, 0, 0);
Vec3f up(0, 1, 0);
Vec3f lightDir = Vec3f(1, -1, 1).normalize();
void lookAt(Vec3f camera, Vec3f center, Vec3f up)
{
	Vec3f z = (camera - center).normalize();
	Vec3f x = cross(up, z).normalize();
	Vec3f y = cross(z, x).normalize();

	Matrix Mr = Matrix::identity();
	Matrix Mt = Matrix::identity();
	ModelView = Matrix::identity();

	for (int i = 0; i < 3; i++)
	{
		//Æ½ÒÆ¾ØÕó
		Mt[i][3] = -center[i];
		//Ðý×ª¾ØÕó
		Mr[0][i] = x[i];
		Mr[1][i] = y[i];
		Mr[2][i] = z[i];
	}
	ModelView = Mr * Mt;
}

void projection(Vec3f camera, Vec3f center)
{
	Projection = Matrix::identity();
	Projection[3][2] = -1.f / (camera - center).norm();
}

//ViewPort Transformation£¬½«×ø±ê´Ó¡¾-1£¬1¡¿µÄ·¶Î§Ó³Éäµ½ÆÁÄ»Êä³öµÄ·¶Î§ÖÐ
void viewport(int x, int y, int w, int h)
{
	Viewport = Matrix::identity();
	Viewport[0][3] = x + w / 2.f;
	Viewport[1][3] = y + h / 2.f;
	Viewport[2][3] = 255.f / 2.f;

	Viewport[0][0] = w / 2.f;
	Viewport[1][1] = h / 2.f;
	Viewport[2][2] = 255.f / 2.f;
}

//false
void test1()
{
	Vec4f v = embed<4>(-0.156265, 0.2548, 0.466332);
	mat<4, 4, float> uniform_M = Viewport * Projection * ModelView;
	Vec4f vertex = uniform_M * v;
	cout << vertex << endl;
}

//yes
void test2()
{
	Vec4f v = embed<4>(-0.156265, 0.2548, 0.466332);
	mat<4, 4, float> uniform_M = Viewport * Projection * ModelView;
	v = uniform_M * v;
	cout << vertex << endl;
}

int main()
{
	int width = 800, height = 800;
	lookAt(camera, center, up);
	viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
	projection(camera, center);

	
}