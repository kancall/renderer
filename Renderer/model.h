#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "geometry.h"
#include "tgaimage.h"

class Model {
private:
	std::vector<Vec3f> verts_; //������еĶ�������
	std::vector<std::vector<Vec3i> > faces_; //�洢�����������Ӧ�������� �������� ������ ������
	std::vector<Vec3f> norms_; //��¼����ÿ������ķ�����Ϣ
	std::vector<Vec2f> uv_; //�洢ģ�͵���������
	TGAImage diffusemap_; //���������ͼ
	TGAImage normalmap_; //��ŷ�����ͼ
	void load_texture(std::string filename, const char* suffix, TGAImage& image);
public:
	Model(const char *filename);
	~Model();
	int nverts(); //�����ܸ���
	int nfaces(); //����
	Vec3f norm(int iface, int nvert);
	Vec3f vert(int i); //��ȡ����Ϊi�Ķ���
	Vec3f vert(int iface, int nthvert);
	Vec2f uv(int nface, int nvert); //��ȡ��nface�棬�����nvert����Ӧ������ͼ�ϵ�uvֵ
	std::vector<int> face(int idx); //������������ȡ��������ж����Ӧ��verts����±�
	TGAColor diffuse(Vec2f uv);
	Vec3f normal(Vec2f uv); //����uvֵ��ȡ������ͼ����
};

#endif //__MODEL_H__
