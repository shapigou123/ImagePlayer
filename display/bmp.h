#ifndef __BMP_H__
#define __BMP_H__
#endif
typedef struct
{
	//unsigned short bfType;	//	2字节.内存对齐
	unsigned long  bfSize;
	unsigned short bfReserved1;
	unsigned short bfReserved2;
	unsigned long  bfOffBits;
}BitMapFileHeader;

typedef struct  
{  
    unsigned long    biSize;  //信息头的长度 
    long  			 biWidth; //图片宽度  
    long   			 biHeight;//图片高度   
    unsigned short   biPlanes;   
    unsigned short   biBitCount;  //图片bbp像素深度
    unsigned long    biCompression;   
    unsigned long    biSizeImage;   
    long   			 biXPelsPerMeter;   
    long   			 biYPelsPerMeter;   
    unsigned long    biClrUsed;   
    unsigned long    biClrImportant;   
}BitMapInfoHeader;





