#ifndef __BMP_H__
#define __BMP_H__
#endif
typedef struct
{
	//unsigned short bfType;	//	2�ֽ�.�ڴ����
	unsigned long  bfSize;
	unsigned short bfReserved1;
	unsigned short bfReserved2;
	unsigned long  bfOffBits;
}BitMapFileHeader;

typedef struct  
{  
    unsigned long    biSize;  //��Ϣͷ�ĳ��� 
    long  			 biWidth; //ͼƬ���  
    long   			 biHeight;//ͼƬ�߶�   
    unsigned short   biPlanes;   
    unsigned short   biBitCount;  //ͼƬbbp�������
    unsigned long    biCompression;   
    unsigned long    biSizeImage;   
    long   			 biXPelsPerMeter;   
    long   			 biYPelsPerMeter;   
    unsigned long    biClrUsed;   
    unsigned long    biClrImportant;   
}BitMapInfoHeader;





