#include <vector>
#include <iostream>
#include <algorithm>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
#include "our_gl.h"
using namespace std;

Vec3f camera(1, 1, 4);
Vec3f center(0, 0, 0);
Vec3f up(0, 1, 0);
Vec3f lightDir = Vec3f(2, 1, 0);

Model* model = NULL;
int width = 800;
int height = 800;
int depth = 255;

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
		Vec3f reflectLight = reflect(normal, light).normalize();

		float spec = pow(std::max(reflectLight.z, 0.0f), model->specular(uv)); //�߹�
		float intensity = std::max(0.f, normal * light); //��ǿ�������ƶ�
		TGAColor diffuseColor = model->diffuse(uv);

		color = diffuseColor;
		for (int i = 0; i < 3; i++) color[i] = std::min<float>(5 + diffuseColor[i] * (intensity + .6 * spec), 255); //�õ�������ɫֵ

		return false; //�������������ɫֵ֮����ж���ѡ���Ƿ��޳�����
	}
};

struct TangentShader :IShader
{
	//ע�⣬mat<i,t,T>��vector[t][i]
	mat<3, 3, float> vertexs; //��������������
	mat<2, 3, float> vertex_uv;
	mat<3, 3, float> vertex_nm; //�洢��������ķ���

	mat<4, 4, float> uniform_MIT = (Projection * ModelView).invert_transpose(); //�����
	virtual Vec4f vertex(int iface, int nvert)
	{
		vertex_uv.set_col(nvert, model->uv(iface, nvert));
		vertex_nm.set_col(nvert, proj<3>(uniform_MIT * embed<4>(model->norm(iface, nvert))));
		Vec4f point = Viewport * Projection * ModelView * embed<4>(model->vert(iface, nvert));
		vertexs.set_col(nvert, proj<3>(point / point[3])); //�������Ĺ�һ������ֹ�õ�������w������1
		return  point;
	}

	virtual bool fragment(Vec3f bc, TGAColor& color)
	{
		Vec2f uv = vertex_uv * bc;
		Vec3f bn = (vertex_nm * bc).normalize();

		//ͨ��TBN���󽫷��߱任������ռ���ȥ�������õľ�����ʵ�Ǳʼ����Ǹ����������ʽ�������Ż�һ��
		Vec3f e1 = vertexs.col(1) - vertexs.col(0); //point1-point0
		Vec3f e2 = vertexs.col(2) - vertexs.col(0); //point2-point0
		Vec3f xy1 = Vec3f(vertex_uv[0][1] - vertex_uv[0][0], vertex_uv[0][2] - vertex_uv[0][0], 0); //vec3f(u1-u0,u2-u0,0)
		Vec3f xy2 = Vec3f(vertex_uv[1][1] - vertex_uv[1][0], vertex_uv[1][2] - vertex_uv[1][0], 0); //vec3f(v1-v0,v2-v0,0)

		mat<3, 3, float> A;
		A[0] = e1;
		A[1] = e2;
		A[2] = bn;
		A = A.invert();

		Vec3f t = A * xy1;
		Vec3f b = A * xy2;

		mat<3, 3, float> TBN;
		TBN.set_col(0, t.normalize());
		TBN.set_col(1, b.normalize());
		TBN.set_col(2, bn);

		Vec3f n = (TBN * model->normal(uv)).normalize();

		float diff = max(dot(n, lightDir), 0.f);
		color = model->diffuse(uv) * diff;

		return false;
	}
};

struct DepthShader : IShader
{
	mat<3, 3, float> vertexs;
	virtual Vec4f vertex(int iface, int nvert)
	{
		Vec4f v = Viewport * Projection * ModelView * embed<4>(model->vert(iface, nvert));
		vertexs.set_col(nvert, proj<3>(v / v[3]));

		return v;
	}
	virtual bool fragment(Vec3f bc, TGAColor& color)
	{
		Vec3f p = vertexs * bc;
		color = TGAColor(255, 255, 255) * (p.z / depth);

		return false;
	}
};

int main()
{
	model = new Model("obj/african_head.obj");
	TGAImage image(width, height, TGAImage::RGB);
	TGAImage zbimage(width, height, TGAImage::GRAYSCALE);
	vector<vector<int>> zbuffer(width, vector<int>(height, INT_MIN));

	lookAt(lightDir, center, up);
	viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
	projection(lightDir, center);

	DepthShader shader;
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