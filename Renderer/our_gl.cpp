#include "model.h"
#include "our_gl.h"
#include "renderwindow.h"
using namespace std;

const int depth = 255;

Matrix Projection;
Matrix Viewport;
Matrix ModelView;

IShader::~IShader(){ }

float dot(Vec3f a, Vec3f b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vec3f cross(Vec3f a, Vec3f b)
{
	return Vec3f(a.y * b.z - b.y * a.z, -a.x * b.z + b.x * a.z, a.x * b.y - b.x * a.y);
}

Vec3f reflect(Vec3f normal, Vec3f light)
{
	Vec3f res = normal * dot(light, normal) * 2.f - light;
	return res;
}
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
		//平移矩阵
		Mt[i][3] = -center[i];
		//旋转矩阵
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

void projection(float coeff)
{
	Projection = Matrix::identity();
	Projection[3][2] = coeff;
}

//ViewPort Transformation，将坐标从【-1，1】的范围映射到屏幕输出的范围中
void viewport(int x, int y, int w, int h) 
{
	Viewport = Matrix::identity();
	Viewport[0][3] = x + w / 2.f;
	Viewport[1][3] = y + h / 2.f;
	Viewport[2][3] = depth / 2.f;

	Viewport[0][0] = w / 2.f;
	Viewport[1][1] = h / 2.f;
	Viewport[2][2] = depth / 2.f;
}

/// <summary>
/// 计算该点在三角形中的重心坐标
/// </summary>
/// <param name="points"></param>
/// <param name="p"></param>
/// <returns></returns>
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

void setPointColor(int x,int y, TGAColor tgaColor)
{
    RenderWindow rw;
    QRgb color = qRgb(tgaColor[2], tgaColor[1], tgaColor[0]);
    rw.setPointColor(x, y, color);
}

void triangle(Vec3i* points, IShader& shader, vector<vector<int>>& zbuffer, TGAImage& image)
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
                setPointColor(x, y, color);
			}
		}
	}
}
