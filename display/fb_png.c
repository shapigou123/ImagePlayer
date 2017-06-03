/*
 *
 *���ļ���������jpgͼƬ��������fb.c�е���ʾ�ӿ�����ʾlcd��
 *
 *
 */

#include <stdio.h>

#include <config.h>

#include <png.h>
#include <pngstruct.h>
#include <pnginfo.h>

/*��������: �ж�һ��ͼƬ�ļ��ǲ���jpgͼƬ
 *��������: path��ͼƬ�ļ���pathname
 *����ֵ:   �����jpg�򷵻�0�������򷵻�1�����󷵻�-1
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
 *��������:����jpgͼƬ������������������ݴ洢
 *��������:pPic,��¼ԴjpgͼƬ����Ϣ�����������ͼƬ��ߡ�ͼƬ���ݻ���������Ϣ
 *����ֵ  :��ȷ����0�����󷵻�-1
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
	//��һ��:������ݽṹ��ʼ��
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

	//�ڶ���:���ô�������
	if (setjmp(png_jmpbuf(png_ptr)))   
	{     
	   	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);    
		fclose(fp);    
     	return -1;  
	}

	//������:��Ҫ�����pngͼƬ���ļ�ָ���png������png_ptr������
	png_init_io(png_ptr, fp);

	//���Ĳ�:��pngͼƬ��Ϣ
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND | PNG_TRANSFORM_STRIP_ALPHA, NULL);

	//���岽:��ȡ���ͼƬ��Ϣ����ӡ����

	color_type = info_ptr->color_type;
	debug("color_type = %d\n", color_type);
	
	pPic->width = info_ptr->width;
	pPic->height = info_ptr->height;
	pPic->bpp = info_ptr->pixel_depth;
	len = info_ptr->width * info_ptr->height * info_ptr->pixel_depth / 8;
	debug("PNG picture: width = %d, height = %d, bpp = %d.\n",pPic->width,pPic->height,pPic->bpp);

	//������:��ȡ������ͼ����Ϣ

	row_pointers = png_get_rows(png_ptr,info_ptr);

	// ֻ����RGB24λ���ɫͼƬ��������ʽ��ͼƬ����
	// ��7��: ͼ�������ƶ��������Լ���buf��
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

	// ��8��: ��β����
	png_destroy_read_struct(&png_ptr, &info_ptr, 0);
	
	return 0;


	
}


// ��װ��һ������ʹ�õ�jpg��ʾ����
// ����������ֻ��Ҫһ��jpgͼƬ��pathname���ɣ���Щ���ӵ����ݽṹ����jpg��ʾģ���ڲ������
// ��ȷ��ʾͼƬ����0����ʾ�����г����򷵻�-1

int display_png(const char *pathname)
{
	//��һ��:������ͼƬ�ǲ���jpgͼƬ
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

	//������:��ʾ��ͼƬ
	fb_draw_jpg(&picture);
	return 0;
}

 






