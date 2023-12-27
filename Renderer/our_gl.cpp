#ifndef __OUR_GL_H__
#define __OUR_GL_H__

#include "tgaimage.h"
#include "model.h"
using namespace std;

const int depth = 255;
Model* model = NULL;

float dot(Vec3f a, Vec3f b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vec3f cross(Vec3f a, Vec3f b)
{
	return Vec3f(a.y * b.z - b.y * a.z, -a.x * b.z + b.x * a.z, a.x * b.y - b.x * a.y);
}

//������matrix���任��������vertor������v2m�������
Vec3f m2v(Matrix m) {
	return Vec3f(m[0][0] / m[3][0], m[1][0] / m[3][0], m[2][0] / m[3][0]);
}

//��������vertor���任�ɾ���matrix��������������������
Matrix v2m(Vec3f v) {
	Matrix m(4, 1);
	m[0][0] = v.x;
	m[1][0] = v.y;
	m[2][0] = v.z;
	m[3][0] = 1.f;
	return m;
}

//ViewPort Transformation��������ӡ�-1��1���ķ�Χӳ�䵽��Ļ����ķ�Χ��
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

//��z����ת
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
//	//�ѳ��������߷���y����
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
//	//����float�ͳ����ķ�����
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
//		//�ɷ��������ظ��ļ��㣬Ч�ʵ�
//		//t��y���ߵİٷֱȣ�����x��ĳ��ȣ�����ƫ��ֵ
//		//i = (x1 - x0)*(t - y0) / (y1 - y0) + x0;
//		//����i = (t - y0) / (float)(y1 - y0) * (x1 - x0) + x0;Ҫ��float����Ȼ�������ǰ�����һֱ��0
//
//		//�·����������ظ����㡣�ж�б���Ƿ��λ����λ�ͽ�x���ƶ�һλ
//		/*pix += rate;
//		if (pix > .5)
//		{
//			i += (x1 > x0 ? 1 : -1);
//			pix -= 1;
//		}*/
//
//		//���µķ�����ȥ��������ֱ����dx��dy���бȽϣ�Ч�ʸ��ߡ���ʵ�൱����һ���������������2��dy
//		pix2 += rate2;
//		if (pix2 > dx && dx != 0)//��������ж���������ˮƽ����ֱ�ߵ�б�����
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

/// <summary>
/// ����õ����������е���������
/// </summary>
/// <param name="points"></param>
/// <param name="p"></param>
/// <returns></returns>
Vec3f barycentric(Vec3i* points, Vec2i p)
{
	//�������������������ж�Ӧ��u vֵ��Ȼ���ж�u vֵ�ڲ���0 1��Χ��
	/*Vec3f uv = Vec3f(points[2].x - points[0].x, points[1].x - points[0].x, points[0].x - p.x) ^
		Vec3f(points[2].y - points[0].y, points[1].y - points[0].y, points[0].y - p.y);
	if (abs(uv.z < 1)) return Vec3f(-1, 1, 1);
	return Vec3f(1.f - (uv.x + uv.y) / uv.z, uv.y / uv.z, uv.x / uv.z);*/

	//����games101 p9 16��12�Ĺ�ʽ���
	float u = (float)((p.y - points[1].y) * (points[2].x - points[1].x) - (p.x - points[1].x) * (points[2].y - points[1].y)) /
		((points[0].y - points[1].y) * (points[2].x - points[0].x) - (points[0].x - points[1].x) * (points[2].y - points[0].y));
	float v = (float)((p.y - points[2].y) * (points[0].x - points[2].x) - (p.x - points[2].x) * (points[0].y - points[2].y)) /
		((points[0].x - points[2].x) * (points[1].y - points[2].y) - (points[0].y - points[2].y) * (points[1].x - points[2].x));
	return Vec3f(u, v, (1 - u - v));
}

//�������������ֵ���õ�Ŀ���Ĺ��մ�С
float Gouraud(float* intensity, Vec3f bc)
{
	return intensity[0] * bc.x + intensity[1] * bc.y + intensity[2] * bc.z;
}

void triangle(Vec3i* points, Vec2i* uv, vector<vector<int>>& zbuffer, TGAImage& image, float* intensity)
{
	Vec2i bboxMax = Vec2i(0, 0), bboxMin = Vec2i(image.get_width() - 1, image.get_height() - 1);

	for (int i = 0; i < 3; i++)
	{
		//�����ǲ��С�Ƚ��ǿ��������߽�û
		bboxMin.x = max(0, min(bboxMin.x, points[i].x));
		bboxMin.y = max(0, min(bboxMin.y, points[i].y));

		bboxMax.x = min(image.get_width() - 1, max(bboxMax.x, points[i].x));
		bboxMax.y = min(image.get_height() - 1, max(bboxMax.y, points[i].y));
	}

	for (int x = bboxMin.x; x <= bboxMax.x; x++)
	{
		for (int y = bboxMin.y; y <= bboxMax.y; y++)
		{
			Vec3f bc = barycentric(points, Vec2i(x, y)); //���ﴫ������������㹫ʽ��p���Ǹ���άֵ
			if (bc.x < 0 || bc.y < 0 || bc.z < 0) //�õ�������ֵ����0 1��Χ�ڣ����Բ�����������
				continue;
			//�������������ֵ���õ����Ӧ��uvֵ
			Vec2i pixelUV = uv[0] * bc.x + uv[1] * bc.y + uv[2] * bc.z;
			//��������������������������zֵ
			float z = points[0].z * bc.x + points[1].z * bc.y + points[2].z * bc.z;
			if (zbuffer[x][y] < z)
			{
				zbuffer[x][y] = z;
				TGAColor color = model->getTextureColor(pixelUV);

				float gouraud = Gouraud(intensity, bc);
				image.set(x, y, TGAColor(color.bgra[2], color.bgra[1], color.bgra[0]) * gouraud);
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
		//ƽ�ƾ���
		Mt[i][3] = -center[i];
		//��ת����
		Mr[0][i] = x[i];
		Mr[1][i] = y[i];
		Mr[2][i] = z[i];
	}
	return Mr * Mt;
}

#endif // !_OUT_GL_H
