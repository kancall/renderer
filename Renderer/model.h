#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "geometry.h"
#include "tgaimage.h"

class Model {
private:
	std::vector<Vec3f> verts_; //������еĶ�������
	std::vector<std::vector<Vec3i> > faces_; //�洢�����������Ӧ�������� �������� ������ ������
	std::vector<Vec2f> uv_; //�洢ģ�͵���������
	TGAImage textureMap_; //���������ͼ
	void load_texture(std::string filename, const char* suffix, TGAImage& image);
public:
	Model(const char *filename);
	~Model();
	int nverts(); //�����ܸ���
	int nfaces(); //����
	Vec3f vert(int i); //��ȡ����Ϊi�Ķ���
	Vec2i uv(int nface, int nvert); //��ȡ��nface�棬�����nvert����Ӧ������ͼ�ϵ�uvֵ
	std::vector<int> face(int idx); //������������ȡ��������ж����Ӧ��verts����±�
	TGAColor getTextureColor(Vec2i uv);
};

#endif //__MODEL_H__