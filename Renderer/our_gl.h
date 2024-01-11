#ifndef __OUR_GL_H__
#define __OUR_GL_H__

#include "tgaimage.h"
#include "geometry.h"
#include<vector>
using namespace std;

extern Matrix Projection;
extern Matrix Viewport;
extern Matrix ModelView;

float dot(Vec3f a, Vec3f b);
Vec3f cross(Vec3f a, Vec3f b);

void projection(Vec3f camera, Vec3f center);
void viewport(int x, int y, int w, int h);
void lookAt(Vec3f camera, Vec3f center, Vec3f up);

struct IShader 
{
	virtual ~IShader();
	virtual Vec4f vertex(int iface, int nvert) = 0;
	virtual bool fragment(Vec3f bc, TGAColor& color, Vec2f uv) = 0;
};

void triangle(Vec3i* points, IShader& shader, Vec2f* uv, vector<vector<int>>& zbuffer, TGAImage& image);

#endif // !__OUR_GL_H__
