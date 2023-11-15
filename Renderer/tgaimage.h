#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <fstream>

#pragma pack(push,1) //struct����1�ֽڶ��룬һ��18�ֽڵ�����
struct TGA_Header { //����TGA�ļ��ı�Ҫ��Ϣ
	char idlength; //ͼ����Ϣ�ֶγ���
	char colormaptype; //��ɫ��
	char datatypecode; //ͼ������
	short colormaporigin; //��ɫ���׵�ַ
	short colormaplength; //��ɫ����
	char colormapdepth; //��ɫ��ÿ������ռ�õ�λ��
	short x_origin; //ͼ��x�����ʼ����
	short y_origin; //ͼ��y�����ʼ����
	short width;//ͼ����
	short height; //ͼ��߶�
	char  bitsperpixel; //ÿ������ռ�õ�λ��
	char  imagedescriptor; //ͼ��������
};
#pragma pack(pop)


//�洢ͼƬ��ÿһ�����ص���ɫֵ
struct TGAColor {
	union {
		struct {
			unsigned char b, g, r, a;
		};
		unsigned char raw[4];
		unsigned int val;
	};
	int bytespp;

	TGAColor() : val(0), bytespp(1) {
	}

	TGAColor(unsigned char R, unsigned char G, unsigned char B, unsigned char A) : b(B), g(G), r(R), a(A), bytespp(4) {
	}

	TGAColor(int v, int bpp) : val(v), bytespp(bpp) {
	}

	TGAColor(const TGAColor &c) : val(c.val), bytespp(c.bytespp) {
	}

	TGAColor(const unsigned char *p, int bpp) : val(0), bytespp(bpp) {
		for (int i=0; i<bpp; i++) {
			raw[i] = p[i];
		}
	}

	TGAColor & operator =(const TGAColor &c) {
		if (this != &c) {
			bytespp = c.bytespp;
			val = c.val;
		}
		return *this;
	}
};

//TGAͼƬ
class TGAImage {
protected:
	unsigned char* data; //���TGAͼƬ��ȡ������������
	int width;
	int height;
	int bytespp; //bits per pixel���洢������ȣ�8��16��24bits��

	bool   load_rle_data(std::ifstream &in);
	bool unload_rle_data(std::ofstream &out);
public:
	enum Format {
		GRAYSCALE=1, RGB=3, RGBA=4
	};

	TGAImage();
	TGAImage(int w, int h, int bpp);
	TGAImage(const TGAImage &img);
	bool read_tga_file(const char *filename); //��ȡͼƬ
	bool write_tga_file(const char *filename, bool rle=true);
	bool flip_horizontally();
	bool flip_vertically();
	bool scale(int w, int h);
	TGAColor get(int x, int y); //������Ļ�����ȡ������ɫ
	bool set(int x, int y, TGAColor c);
	~TGAImage();
	TGAImage & operator =(const TGAImage &img);
	int get_width();
	int get_height();
	int get_bytespp();
	unsigned char *buffer();
	void clear();
};

#endif //__IMAGE_H__
