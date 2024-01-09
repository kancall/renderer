#include <vector>
#include <iostream>
#include <algorithm>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
#include "our_gl.h"

Vec3f camera(2, 1, 4);
Vec3f center(0, 0, 0);
Vec3f up(0, 1, 0);
Vec3f lightDir = Vec3f(1, -1, 1).normalize();

Model* model = NULL;
int width = 800;
int height = 800;

struct GouraudShader :public IShader
{
	Vec3f intensity;
	virtual Vec4f vertex(int iface, int nvert) //返回变换后的顶点
	{
		intensity[nvert] = std::max(0.f, dot(model->norm(iface, nvert), lightDir));
		Vec4f v = embed<4>(model->vert(iface, nvert));
		Vec4f vertexs = Viewport * Projection * ModelView * v;

		return vertexs;
	}
	virtual void fragment(Vec3f bc, TGAColor& color, Vec2i uv)
	{
		TGAColor diffuseColor = model->diffuse(uv); //得到纹理图的颜色值
		float gouraud = intensity * bc; //根据光强得到gouraud着色的值
		color = diffuseColor * gouraud;
	}
};

int main()
{
	Vec4f v = Vec4f();
	Vec4f temp = ModelView * v; //很奇怪啊，这乘法第一次乘出来的老是错的，我真搞不懂了

	model = new Model("obj/african_head.obj");
	TGAImage image(width, height, TGAImage::RGB);
	TGAImage zbimage(width, height, TGAImage::GRAYSCALE);
	vector<vector<int>> zbuffer(width, vector<int>(height, INT_MIN));

	lookAt(camera, center, up);
	viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
	projection(camera, center);

	GouraudShader shader;
	for (int i = 0; i < model->nfaces(); i++)
	{
		Vec4f screen[3];
		Vec2i uv[3];
		for (int t = 0; t < 3; t++)
		{
			screen[t] = shader.vertex(i, t);
			uv[t] = model->uv(i, t);
		}
		Vec3i points[3];
		for (int i = 0; i < 3; i++)
		{
			points[0][i] = screen[0][i];
			points[1][i] = screen[1][i];
			points[2][i] = screen[2][i];
		}
		triangle(points, shader, uv, zbuffer, image);
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