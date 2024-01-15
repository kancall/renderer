#include <vector>
#include <iostream>
#include <algorithm>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
#include "our_gl.h"
using namespace std;

Vec3f camera(-1, 0, 5);
Vec3f center(0, 0, 0);
Vec3f up(0, 1, 0);
Vec3f lightDir = Vec3f(1, 1, 1).normalize();

Model* model = NULL;
int width = 800;
int height = 800;

struct GouraudShader :public IShader
{
	Vec3f vertex_intensity; //��������Ĺ�ǿ
	mat<2, 3, float> vertex_uv; //���������uv����
	virtual Vec4f vertex(int iface, int nvert) //���ر任��Ķ���
	{
		vertex_intensity[nvert] = std::max(0.f, dot(model->norm(iface, nvert), lightDir));
		vertex_uv.set_col(nvert, model->uv(iface, nvert));
		Vec4f v = embed<4>(model->vert(iface, nvert));

		return Viewport * Projection * ModelView * v;
	}
	virtual bool fragment(Vec3f bc, TGAColor& color)
	{
		Vec2f uv = vertex_uv * bc;

		float intensity = vertex_intensity * bc; //�������������ÿ������Ĺ�ǿ�õ�Ŀ���Ĺ�ǿ
		TGAColor diffuseColor = model->diffuse(uv); //�õ�����ͼ����ɫֵ
		color = diffuseColor * intensity;

		return false; //�������������ɫֵ֮����ж���ѡ���Ƿ��޳�����
	}
};

struct Shader :public IShader
{
	mat<2, 3, float> vertex_uv;
	mat<4, 4, float> uniform_M = Projection * ModelView;
	mat<4, 4, float> uniform_MIT = (Projection * ModelView).invert_transpose(); //�����
	virtual Vec4f vertex(int iface, int nvert) //���ر任��Ķ���
	{
		vertex_uv.set_col(nvert, model->uv(iface, nvert));
		Vec4f v = embed<4>(model->vert(iface, nvert));

		return Viewport * Projection * ModelView * v;
	}
	virtual bool fragment(Vec3f bc, TGAColor& color) 
	{
		Vec2f uv = vertex_uv * bc;
		Vec3f normal = proj<3>(uniform_MIT * embed<4>(model->normal(uv))).normalize();
		Vec3f light = proj<3>(uniform_M * embed<4>(lightDir)).normalize();

		TGAColor diffuseColor = model->diffuse(uv);
		float intensity = std::max(0.f, dot(normal, light)); //�����ڷ��߷���ķ���
		color = diffuseColor * intensity;

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

	Shader shader;
	for (int i = 0; i < model->nfaces(); i++) //��ѭ����������������
	{
		Vec4f screen[3];
		for (int t = 0; t < 3; t++) //��ѭ���������������ж���
		{
			screen[t] = shader.vertex(i, t);
		}
		Vec3i points[3];
		for (int i = 0; i < 3; i++)
		{
			for (int t = 0; t < 3; t++) 
				points[t][i] = screen[t][i];
		}
		triangle(points, shader, zbuffer, image);
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