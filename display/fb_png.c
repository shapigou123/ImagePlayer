/*
 *
 *本文件用来解析jpg图片，并调用fb.c中的显示接口来显示lcd上
 *
 *
 */

#include <stdio.h>

#include <config.h>

#include <png.h>
#include <pngstruct.h>
#include <pnginfo.h>

/*函数功能: 判断一个图片文件是不是jpg图片
 *函数参数: path是图片文件的pathname
 *返回值:   如果是jpg则返回0，不是则返回1，错误返回-1
 */

int is_png(const char *pathname)
{
	FILE *file = NULL;
	char buf[PNG_BYTES_TO_CHECK] = {0};	  
	if ((file = fopen(pathname, "rb")) == NULL)		
		return -1;	/* Read in some of the signature bytes */	
	
	if (fread(buf, 1, PNG_BYTES_TO_CHECK, file) != PNG_BYTES_TO_CHECK)	 
		return -1;  
	return(png_sig_cmp(buf, (png_size_t)0, PNG_BYTES_TO_CHECK));

}


/*
 *函数功能:解码jpg图片，并将解码出来的数据存储
 *函数参数:pPic,记录源jpg图片的信息，解码出来的图片宽高、图片数据缓冲区等信息
 *返回值  :正确返回0，错误返回-1
 */

static int png_analyze(struct pic_info *pPic)
{  
	
	FILE *fp = NULL;
	png_structp png_ptr;
	png_infop info_ptr;
	int color_type;
	png_bytep* row_pointers;
	unsigned long len = 0;
	int pos = 0;
	int i = 0, j = 0;
	
	if ((fp = fopen(pPic->pathname, "rb")) == NULL)     
		{
			fprintf(stderr, "can't open %s\n", pPic->pathname);  
			return -1;	
		}
	//第一步:相关数据结构初始化
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0); 

	if (png_ptr == NULL)  
	{      
		fclose(fp);      
		return -1;  
	}

	info_ptr = png_create_info_struct(png_ptr);  
	if (info_ptr == NULL)   
	{
		fclose(fp);     
		png_destroy_read_struct(&png_ptr, NULL, NULL);     		
		return -1;   
	}

	//第二步:设置错误处理函数
	if (setjmp(png_jmpbuf(png_ptr)))   
	{     
	   	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);    
		fclose(fp);    
     	return -1;  
	}

	//第三步:将要解码的png图片的文件指针和png解码器png_ptr绑定起来
	png_init_io(png_ptr, fp);

	//第四步:读png图片信息
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND | PNG_TRANSFORM_STRIP_ALPHA, NULL);

	//第五步:读取相关图片信息并打印出来

	color_type = info_ptr->color_type;
	debug("color_type = %d\n", color_type);
	
	pPic->width = info_ptr->width;
	pPic->height = info_ptr->height;
	pPic->bpp = info_ptr->pixel_depth;
	len = info_ptr->width * info_ptr->height * info_ptr->pixel_depth / 8;
	debug("PNG picture: width = %d, height = %d, bpp = %d.\n",pPic->width,pPic->height,pPic->bpp);

	//第六步:读取真正的图像信息

	row_pointers = png_get_rows(png_ptr,info_ptr);

	// 只处理RGB24位真彩色图片，其他格式的图片不管
	// 第7步: 图像数据移动到我们自己的buf中
	if(color_type == PNG_COLOR_TYPE_RGB)
  	{
   		//memcpy(pPic->pData, row_pointers, len);
		
		for(i=0; i<pPic->height; i++)
		{
			for(j=0; j<3*pPic->width; j+=3)
			{
				pPic->pData[pos++] = row_pointers[i][j+0];		
				pPic->pData[pos++] = row_pointers[i][j+1];		
				pPic->pData[pos++] = row_pointers[i][j+2];		
			}
		}
  	}

	// 第8步: 收尾处理
	png_destroy_read_struct(&png_ptr, &info_ptr, 0);
	
	return 0;


	
}


// 封装的一个对外使用的jpg显示函数
// 本函数对外只需要一个jpg图片的pathname即可，那些复杂的数据结构都是jpg显示模块内部处理的
// 正确显示图片返回0，显示过程中出错则返回-1

int display_png(const char *pathname)
{
	//第一步:检测给的图片是不是jpg图片
	int ret = -1;
	struct pic_info picture;
	ret = is_png(pathname);
	if(ret != 0)
	{
		return -1;
	}
	debug("%s is a png picture,\n",pathname);

	picture.pathname = (char *)pathname;
	picture.pData = rgb_buf;
	png_analyze(&picture);

	//第三步:显示该图片
	fb_draw_jpg(&picture);
	return 0;
}

 






