/*
 *fb.c ����framebuffer�Ļ������룬����fb�Ĵ򿪡�ioctl��ȡ��Ϣ
 *     �����Ĳ���fb��ʾ���롣
 *
 ********************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <fb.h>
#include <config.h>

//#include "1024600.h"
//#include "800480.h"

unsigned int *pfb = NULL; //ָ��framebuffer�ڴ��׵�ַ
int fbfd = -1;

 
int fb_open()
{
	struct fb_var_screeninfo vinfo ;
	struct fb_fix_screeninfo finfo ;
	 
	int ret = -1;
	 // ��1�������豸
	fbfd = open(FBDEVICE, O_RDWR);
	if (fbfd< 0)
	{
		perror("open error");
	 	return -1;
	}
	debug("open %s success.\n", FBDEVICE);
	 
	// ��2������ȡ�豸��Ӳ����Ϣ
	ret = ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo);
	if(ret < 0)
	{
		 perror("ioctl");
		 close(fbfd);
		 return -1;
	}
	 debug("smem_start = 0x%lx , smem_len = %u\n\n", finfo.smem_start,finfo.smem_len);
	 
	ret = ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo);	 
	if(ret < 0)
	{
		perror("ioctl");
		close(fbfd);
		return -1;
	}
	debug("�޸Ĺ�֮ǰ�Ĳ�����\n");
	debug("xres = %d , yres = %d\n", vinfo.xres,vinfo.yres);
	debug("xres_virtual = %d , yres_virtual = %d\n", vinfo.xres_virtual,vinfo.yres_virtual);
	debug("bits_per_pixel = %d\n\n", vinfo.bits_per_pixel);
	
	 // ��3��������mmap
	pfb = mmap(NULL, finfo.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
	if(NULL == pfb)
	{
		perror("mmap error");
		return 1;
	}		 
	debug("pfb = %p\n\n", pfb);
	 
	//fb_draw_back(WIDTH, HEIGHT, GREEN);
	//fb_draw_line(RED);

	return 0;
 }

void fb_close()
{
	close(fbfd);
}
 

void fb_draw_back(unsigned int width, unsigned int height, unsigned int color)
{
	unsigned int x, y;
	for(y = 0; y < height; y++)
	{
		for(x = 0; x < width; x++)
		{
			*(pfb + y * WIDTH + x) = color; 
		}
	}
}
 
 
void fb_draw_line(unsigned int color)
{
	unsigned int x;
	for(x = 0; x < WIDTH; x++)
	{
		*(pfb + 250 * WIDTH + x) = color;	 
 
	}
}

#if 0 
//��ʾ1024*600��ͼƬ 
void fb_draw_picture1(void)
{
	const unsigned char *pData = gImage_picture;
	unsigned int *p = pfb;
	unsigned int x,y;
	unsigned int cnt = 0;
	for(y=0; y<HEIGHT; y++)
	{
		for(x=0; x<WIDTH;x++)
		{
			cnt =y*WIDTH + x;   //��ǰ���ص���
			cnt *= 3;       	 //��ǰ���ص��������������±�
			//��ǰ���ص������
			*p++ = ((pData[cnt+2]<<0) | (pData[cnt+1]<<8) | (pData[cnt+0]<<16));

		}
	}

						
}
 

void fb_draw_picture2(void)
{
	const unsigned char *pData = gImage_picture;
	unsigned int x,y;
	unsigned int cnt = 0;
	for(y=0; y<HEIGHT; y++)
	{
		for(x=0; x<WIDTH;x++)
		{
			cnt =y*WIDTH + x;	 //��ǰ���ص���

			*(pfb+cnt) = ((pData[3*cnt+2]<<0) | (pData[3*cnt+1]<<8) | (pData[3*cnt+0]<<16));
			 //��ǰ���ص������
		}
	}
}
						 
void fb_draw_picture3(void)
{
	const unsigned char *pData = gImage_picture;
	unsigned int x,y;
	unsigned int cnt = 0;
	unsigned int a = 0;
	for(y=0; y<HEIGHT; y++)
	{
		for(x=0; x<WIDTH;x++)
		{
			cnt =y*WIDTH + x;	 //��ǰ���ص���

			*(pfb+cnt) = ((pData[a+2]<<0) | (pData[a+1]<<8) | (pData[a+0]<<16));
			 //��ǰ���ص������
			 a += 3;
		}
	}
}

//ͼƬ�ֱ��ʴ�С��800*480						 
void fb_draw_picture4(void)
{
	const unsigned char *pData = gImage_800480;
	unsigned int x,y;
	unsigned int cnt = 0;
	unsigned int a = 0;
	for(y=0; y<480; y++)
	{
		for(x=0; x<800;x++)
		{
			cnt =y*WIDTH + x;	 //��ǰ���ص���

			*(pfb+cnt) = ((pData[a+2]<<0) | (pData[a+1]<<8) | (pData[a+0]<<16));
			 //��ǰ���ص������
			 a += 3;
		}
	}
}

//ͼƬ�ֱ��ʴ�С��800*480_2
void fb_draw_picture5(void)
{
	const unsigned char *pData = gImage_800480;
	unsigned int x,y;
	unsigned int cnt1 = 0;
	unsigned int cnt2 = 0;
	for(y=0; y<480; y++)
	{
		for(x=0; x<800;x++)
		{
			cnt1 =y*WIDTH + x;	 //��ǰ���ص���
			cnt2 =y*500 + x;

			*(pfb+cnt1) = ((pData[3*cnt2+2]<<0) | (pData[3*cnt2+1]<<8) | (pData[3*cnt2+0]<<16));
			 //��ǰ���ص������

		}
	}
}

//��������:��ͼƬ����ʼλ��(x0,y0)��__��x������ʾ������һ�ߣ���y������ʾ
void fb_draw_picture6(unsigned int x0, unsigned int y0)
{
	const unsigned char *pData = gImage_800480;
	unsigned int x,y;
	unsigned int cnt = 0;
	unsigned int a = 0;
	for(y=y0; y<480+y0; y++)
	{
		for(x=x0; x<800+x0;x++)
		{
			cnt =y*WIDTH + x;	 //��ǰ���ص���
			//��ֵ���ǵ�ǰ���ص���fb�ڴ�(��ʾ�ڴ�)�е�ƫ����
			//��ǰ���ص���ͼ�����������е��±�
			*(pfb+cnt) = ((pData[a+2]<<0) | (pData[a+1]<<8) | (pData[a+0]<<16));
			 //��ǰ���ص������
			 a += 3;
		}
	}
}

 
//��������:��ͼƬ����ʼλ��(x0,y0)��___��������x��y������ʾ
void fb_draw_picture7(unsigned int x0, unsigned int y0)
{
	const unsigned char *pData = gImage_800480;
	unsigned int x,y;
	unsigned int cnt = 0;
	unsigned int a = 0;
	for(y=y0; y<480+y0; y++)
	{
		if(y >= HEIGHT)
			break;
		
		for(x=x0; x<800+x0;x++)
		{	
			if(x >= WIDTH)
			{
				a+=3;
				continue;
			}
		
			cnt =y*WIDTH + x;	 //��ǰ���ص���
			//��ֵ���ǵ�ǰ���ص���fb�ڴ�(��ʾ�ڴ�)�е�ƫ����
			//��ǰ���ص���ͼ�����������е��±�
			*(pfb+cnt) = ((pData[a+2]<<0) | (pData[a+1]<<8) | (pData[a+0]<<16));
			 //��ǰ���ص������
			 a += 3;
		}
	}
} 
 
#endif 
 
 void fb_draw_bmp(const struct pic_info *pPic)
 {
	 const char *pData = ( const char *)pPic->pData;
	 unsigned int x,y=0;
	 unsigned int cnt = 0;
	 unsigned  long int a = 0;

	if ((pPic->bpp != 32) && (pPic->bpp != 24))
	{
		fprintf(stderr, "BPP %d is not support.\n", pPic->bpp);
		return;
	}

	debug("image resolution: %d * %d, bpp = %d.\n ",
	pPic->width, pPic->height, pPic->bpp); 

	 for(y=0; y< pPic->height; y++)
	 {
	 	 a=(pPic->height-y-1)*(pPic->width)*3;//ָ�����һ�е�һ�����ص�
		 for(x=0; x<pPic->width; x++)
		 {
			cnt =y*WIDTH + x;	  //��ǰ���ص���
			 //��ֵ���ǵ�ǰ���ص���fb�ڴ�(��ʾ�ڴ�)�е�ƫ����
			 //��ǰ���ص���ͼ�����������е��±�			
			*(pfb+cnt) = ((pData[a+0]<<0) | (pData[a+1]<<8) | (pData[a+2]<<16));
			//��ǰ���ص������
			a += 3;
		 }
	 }

 }

 
 
 void fb_draw_jpg(const struct pic_info *pPic)
 {
	 const char *pData = ( const char *)pPic->pData;
	 unsigned int x,y=0;
	 unsigned int cnt = 0;
	 unsigned  long int a = 0;

	if ((pPic->bpp != 32) && (pPic->bpp != 24))
	{
		fprintf(stderr, "BPP %d is not support.\n", pPic->bpp);
		return;
	}

	debug("image resolution: %d * %d, bpp = %d.\n ",
	pPic->width, pPic->height, pPic->bpp); 

	 for(y=0; y< pPic->height; y++)
	 {
	 	 //a=(pPic->height-y-1)*(pPic->width)*3;//ָ�����һ�е�һ�����ص�
		 for(x=0; x<pPic->width; x++)
		 {
			cnt =y*WIDTH + x;	  //��ǰ���ص���
			 //��ֵ���ǵ�ǰ���ص���fb�ڴ�(��ʾ�ڴ�)�е�ƫ����
			 //��ǰ���ص���ͼ�����������е��±�			
			*(pfb+cnt) = ((pData[a+2]<<0) | (pData[a+1]<<8) | (pData[a+0]<<16));
			//��ǰ���ص������
			a += 3;
		 }
	 }

 }
 
 
 
 
 























