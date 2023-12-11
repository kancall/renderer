#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "geometry.h"
#include "tgaimage.h"

class Model {
private:
	std::vector<Vec3f> verts_; //存放所有的顶点坐标
	std::vector<std::vector<Vec3i> > faces_; //存储该面三个点对应顶点坐标 纹理坐标 法向量 的索引
	std::vector<Vec3f> norms_;
	std::vector<Vec2f> uv_; //存储模型的纹理坐标
	TGAImage textureMap_; //存放纹理贴图
	void load_texture(std::string filename, const char* suffix, TGAImage& image);
public:
	Model(const char *filename);
	~Model();
	int nverts(); //顶点总个数
	int nfaces(); //面数
	Vec3f norm(int iface, int nvert);
	Vec3f vert(int i); //获取索引为i的顶点
	Vec2i uv(int nface, int nvert); //获取第nface面，面里第nvert个对应在纹理图上的uv值
	std::vector<int> face(int idx); //根据面索引获取这个面所有顶点对应在verts里的下标
	TGAColor getTextureColor(Vec2i uv);
};

#endif //__MODEL_H__
