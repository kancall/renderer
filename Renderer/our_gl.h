#ifndef __OUR_GL_H__
#define __OUR_GL_H__

Matrix Projection;
Matrix Viewport;
Matrix ModelView;

struct IShader 
{
	virtual ~IShader();
	virtual Vec3f vertex();
	virtual void fragment();
};

#endif // !__OUR_GL_H__
