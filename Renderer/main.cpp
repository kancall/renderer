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
		Vec3f reflectLight = reflect(normal, light).normalize();

		float spec = pow(std::max(reflectLight.z, 0.0f), model->specular(uv)); //高光
		float intensity = std::max(0.f, normal * light); //光强，看相似度
		TGAColor diffuseColor = model->diffuse(uv);

		color = diffuseColor;
		for (int i = 0; i < 3; i++) color[i] = std::min<float>(5 + diffuseColor[i] * (intensity + .6 * spec), 255); //得到最终颜色值

		return false; //后续可以添加颜色值之类的判断来选择是否剔除像素
	}
};

struct TangentShader :IShader
{
	//注意，mat<i,t,T>是vector[t][i]
	mat<3, 3, float> vertexs; //三角形三个顶点
	mat<2, 3, float> vertex_uv;
	mat<3, 3, float> vertex_nm; //存储三个顶点的法线

	mat<4, 4, float> uniform_MIT = (Projection * ModelView).invert_transpose(); //逆矩阵
	virtual Vec4f vertex(int iface, int nvert)
	{
		vertex_uv.set_col(nvert, model->uv(iface, nvert));
		vertex_nm.set_col(nvert, proj<3>(uniform_MIT * embed<4>(model->norm(iface, nvert))));
		Vec4f point = Viewport * Projection * ModelView * embed<4>(model->vert(iface, nvert));
		vertexs.set_col(nvert, proj<3>(point / point[3])); //齐次坐标的归一化，防止得到的坐标w不等于1
		return  point;
	}

	virtual bool fragment(Vec3f bc, TGAColor& color)
	{
		Vec2f uv = vertex_uv * bc;
		Vec3f bn = (vertex_nm * bc).normalize();

		//通过TBN矩阵将法线变换到世界空间中去，这里用的矩阵其实是笔记中那个有问题的算式，可以优化一下
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