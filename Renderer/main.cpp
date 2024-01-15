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
	Vec3f vertex_intensity; //各个顶点的光强
	mat<2, 3, float> vertex_uv; //各个顶点的uv坐标
	virtual Vec4f vertex(int iface, int nvert) //返回变换后的顶点
	{
		vertex_intensity[nvert] = std::max(0.f, dot(model->norm(iface, nvert), lightDir));
		vertex_uv.set_col(nvert, model->uv(iface, nvert));
		Vec4f v = embed<4>(model->vert(iface, nvert));

		return Viewport * Projection * ModelView * v;
	}
	virtual bool fragment(Vec3f bc, TGAColor& color)
	{
		Vec2f uv = vertex_uv * bc;

		float intensity = vertex_intensity * bc; //根据重心坐标和每个顶点的光强得到目标点的光强
		TGAColor diffuseColor = model->diffuse(uv); //得到纹理图的颜色值
		color = diffuseColor * intensity;

		return false; //后续可以添加颜色值之类的判断来选择是否剔除像素
	}
};

struct Shader :public IShader
{
	mat<2, 3, float> vertex_uv;
	mat<4, 4, float> uniform_M = Projection * ModelView;
	mat<4, 4, float> uniform_MIT = (Projection * ModelView).invert_transpose(); //逆矩阵
	virtual Vec4f vertex(int iface, int nvert) //返回变换后的顶点
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
		float intensity = std::max(0.f, dot(normal, light)); //光线在法线方向的分量
		color = diffuseColor * intensity;

		return false; //后续可以添加颜色值之类的判断来选择是否剔除像素
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
	for (int i = 0; i < model->nfaces(); i++) //外循环遍历所有三角形
	{
		Vec4f screen[3];
		for (int t = 0; t < 3; t++) //内循环遍历三角形所有顶点
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
	//输出深度值
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