#include<iostream>
#include"model.h"
#include "our_gl.h"

Vec3f camera(2, 1, 4);
Vec3f center(0, 0, 0);
Vec3f up(0, 1, 0);
Vec3f lightDir = Vec3f(1, -1, 1).normalize();
Model* model = NULL;

float dot(Vec3f a, Vec3f b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}
struct GouraudShader :public IShader
{
	Vec3f intensity;
	virtual Vec3f vertex(int iface, int nvert)
	{
		intensity[nvert] = dot(model->norm(iface, nvert), lightDir);
		Vec3f v = model->vert(iface, nvert);
		return Viewport * Projection * ModelView * v;
	}
	virtual void fragment()
	{

	}
};