#include <vector>
#include <iostream>
#include <algorithm>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
#include "our_gl.h"
using namespace std;

Vec3f camera(0, 0, 3);
Vec3f center(0, 0, 0);
Vec3f up(0, 1, 0);
Vec3f lightDir = Vec3f(1, 1, 1).normalize();

Model* model = NULL;
int width = 800;
int height = 800;

struct GouraudShader :public IShader
{
	Vec3f intensity;
	virtual Vec4f vertex(int iface, int nvert) //���ر任��Ķ���
	{
		intensity[nvert] = std::max(0.f, dot(model->norm(iface, nvert), lightDir));
		Vec4f v = embed<4>(model->vert(iface, nvert));

		Vec4f vertex = Viewport * Projection * ModelView * v;
		return vertex;
	}
	virtual bool fragment(Vec3f bc, TGAColor& color, Vec2f uv)
	{
		TGAColor diffuseColor = model->diffuse(uv); //�õ�����ͼ����ɫֵ
		float gouraud = intensity * bc; //���ݹ�ǿ�õ�gouraud��ɫ��ֵ
		color = diffuseColor * gouraud;

		return false; //�������������ɫֵ֮����ж���ѡ���Ƿ��޳�����
	}
};

struct Shader :public IShader
{
	Vec3f intensity;
	mat<4, 4, float> uniform_M = Projection * ModelView;; //  Projection*ModelView
	mat<4, 4, float> uniform_MIT = (Projection * ModelView).invert_transpose(); // (Projection*ModelView).invert_transpose()
	virtual Vec4f vertex(int iface, int nvert) //���ر任��Ķ���
	{
		intensity[nvert] = std::max(0.f, dot(model->norm(iface, nvert), lightDir));
		Vec4f v = embed<4>(model->vert(iface, nvert));

		Vec4f vertex = Viewport * Projection * ModelView * v;
		return vertex;
	}
	virtual bool fragment(Vec3f bc, TGAColor& color, Vec2f uv) //��������uv�Ѿ��˹�����������
	{
		Vec3f normal = proj<3>(uniform_MIT * embed<4>(model->normal(uv))).normalize();
		Vec3f light = proj<3>(uniform_M * embed<4>(lightDir)).normalize();
		float intensity = std::max(0.f, dot(normal, light));
		color = model->diffuse(uv) * intensity;

		return false; //�������������ɫֵ֮����ж���ѡ���Ƿ��޳�����
	}
};

int main()
{
	model = new Model("obj/african_head.obj");
	TGAImage image(width, height, TGAImage::RGB);
	TGAImage zbimage(width, height, TGAImage::GRAYSCALE);
	vector<vector<int>> zbuffer(width, vector<int>(height, INT_MIN));

	lookAt(camera, center, up);
	viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
	projection(camera, center);

	//GouraudShader shader;
	Shader shader;
	for (int i = 0; i < model->nfaces(); i++) //��ѭ����������������
	{
		Vec4f screen[3];
		Vec2f uv[3];
		for (int t = 0; t < 3; t++) //��ѭ���������������ж���
		{
			screen[t] = shader.vertex(i, t);
			uv[t] = model->uv(i, t);
		}
		Vec3i points[3];
		for (int i = 0; i < 3; i++)
		{
			for (int t = 0; t < 3; t++) 
				points[t][i] = screen[t][i];
		}
		triangle(points, shader, uv, zbuffer, image);
	}

	image.flip_vertically();
	image.write_tga_file("output.tga");
	//������ֵ
	for (int i = 0; i < width; i++)
	{
		for (int t = 0; t < height; t++)
		{
			zbimage.set(i, t, TGAColor(zbuffer[i][t]));
		}
	}
	zbimage.flip_vertically();
	zbimage.write_tga_file("zbuffer.tga");
}