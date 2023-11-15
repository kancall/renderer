#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <fstream>

#pragma pack(push,1) //struct按照1字节对齐，一共18字节的数据
struct TGA_Header { //保存TGA文件的必要信息
	char idlength; //图像信息字段长度
	char colormaptype; //颜色表
	char datatypecode; //图像类型
	short colormaporigin; //颜色表首地址
	short colormaplength; //颜色表长度
	char colormapdepth; //颜色表每个表项占用的位数
	short x_origin; //图像x轴的起始坐标
	short y_origin; //图像y轴的起始坐标
	short width;//图像宽度
	short height; //图像高度
	char  bitsperpixel; //每个像素占用的位数
	char  imagedescriptor; //图像描述符
};
#pragma pack(pop)


//存储图片中每一个像素的颜色值
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

//TGA图片
class TGAImage {
protected:
	unsigned char* data; //存放TGA图片读取到的像素数据
	int width;
	int height;
	int bytespp; //bits per pixel，存储像素深度，8、16、24bits等

	bool   load_rle_data(std::ifstream &in);
	bool unload_rle_data(std::ofstream &out);
public:
	enum Format {
		GRAYSCALE=1, RGB=3, RGBA=4
	};

	TGAImage();
	TGAImage(int w, int h, int bpp);
	TGAImage(const TGAImage &img);
	bool read_tga_file(const char *filename); //读取图片
	bool write_tga_file(const char *filename, bool rle=true);
	bool flip_horizontally();
	bool flip_vertically();
	bool scale(int w, int h);
	TGAColor get(int x, int y); //根据屏幕坐标获取像素颜色
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
