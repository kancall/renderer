#include "model.h"
#include "our_gl.h"
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
		//ƽ�ƾ���
		Mt[i][3] = -center[i];
		//��ת����
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

//ViewPort Transformation��������ӡ�-1��1���ķ�Χӳ�䵽��Ļ����ķ�Χ��
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
/// ����õ����������е���������
/// </summary>
/// <param name="points"></param>
/// <param name="p"></param>
/// <returns></returns>
Vec3f barycentric(Vec3i* points, Vec2i p)
{
	//����games101 p9 16��12�Ĺ�ʽ���
	float u = (float)((p.y - points[1].y) * (points[2].x - points[1].x) - (p.x - points[1].x) * (points[2].y - points[1].y)) /
		((points[0].y - points[1].y) * (points[2].x - points[0].x) - (points[0].x - points[1].x) * (points[2].y - points[0].y));
	float v = (float)((p.y - points[2].y) * (points[0].x - points[2].x) - (p.x - points[2].x) * (points[0].y - points[2].y)) /
		((points[0].x - points[2].x) * (points[1].y - points[2].y) - (points[0].y - points[2].y) * (points[1].x - points[2].x));
	return Vec3f(u, v, (1 - u - v));
}

void triangle(Vec3i* points, IShader& shader, Vec2f* uv, vector<vector<int>>& zbuffer, TGAImage& image)
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
			Vec2f pixelUV = uv[0] * bc.x + uv[1] * bc.y + uv[2] * bc.z;
			//��������������������������zֵ
			float z = points[0].z * bc.x + points[1].z * bc.y + points[2].z * bc.z;

			TGAColor color;
			bool clip = shader.fragment(bc, color, pixelUV);
			if (zbuffer[x][y] < z && !clip)
			{
				zbuffer[x][y] = z;
				image.set(x, y, color);
			}
		}
	}
}