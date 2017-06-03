/*
 *
 *���ļ���������jpgͼƬ��������fb.c�е���ʾ�ӿ�����ʾlcd��
 *
 *
 */

#include <stdio.h>
#include <jpeglib.h>
#include <config.h>
#include <jerror.h>
#include <string.h>



struct my_error_mgr
{  
	struct jpeg_error_mgr pub;	
	//jmp_buf setjmp_buffer;
};
typedef struct my_error_mgr * my_error_ptr;




//�Լ�����Ĵ�������
METHODDEF(void)
my_error_exit (j_common_ptr cinfo)
{  
	//my_error_ptr myerr = (my_error_ptr) cinfo->err;
	//(*cinfo->err->output_message) (cinfo); 
	fprintf(stderr, "my_error_exit\n");
 	//longjmp(myerr->setjmp_buffer, 1);
}

/*��������: �ж�һ��ͼƬ�ļ��ǲ���jpgͼƬ
 *��������: path��ͼƬ�ļ���pathname
 *����ֵ:   �����jpg�򷵻�0�������򷵻�1�����󷵻�-1
 */
int is_jpg(const char *pathname)
{
	FILE *file = NULL;
	char buf[2]={0};

	//���ļ�
	file = fopen(pathname,"rb");
	if(NULL == file)
	{
		fprintf(stderr, "fopen %s error.\n",pathname);
		fclose(file);
		return -1;
	}

	//����ǰ2���ֽ�
	fread(buf, 1, 2, file); // 2��ʾ������Ԫ�ص�����sizeof(buf)��1��ʾÿ��Ԫ��ռһ���ֽ� sizeof(char)
	//debug("read: 0x%x%x\n", buf[0], buf[1]);
	//�ж��ǲ���0xffd8
	if(!((buf[0] == 0xff) && (buf[1] == 0xd8)))
	{
		fclose(file);
		return 1;		//����jpgͼƬ
	}
	//��0xffd8��ͼ������ļ���������ֽ�
	fseek(file, -2, SEEK_END);
	fread(buf, 1, 2, file);
//	debug("read: 0x%x%x\n", buf[0], buf[1]);

	if(!((buf[0] == 0xff) && (buf[1] == 0xd9)))
	{
	    fclose(file);
		return 1;		//����jpgͼƬ
	}
	fclose(file);
	return 0;
}

/*
 *��������:����jpgͼƬ������������������ݴ洢
 *��������:pPic,��¼ԴjpgͼƬ����Ϣ�����������ͼƬ��ߡ�ͼƬ���ݻ���������Ϣ
 *����ֵ  :��ȷ����0�����󷵻�-1
 */
static int jpg_analyze(struct pic_info *pPic)
{
	struct jpeg_decompress_struct cinfo;		//cinfo�ᴩ����������̵���Ϣ��¼�ʹ��ݵ����ݽṹ
	struct my_error_mgr jerr;					// �������
	FILE *infile;								// ָ��fopen��ԴjpgͼƬ�ļ���ָ��
	//JSAMPARRAY buffer;							// ָ����������ݵ�ָ��
	char *buffer = NULL;
	int row_stride;								// ���������һ��ͼƬ��Ϣ���ֽ���
   // int i;
	if ((infile = fopen(pPic->pathname, "rb")) == NULL) 
	{    
		fprintf(stderr, "can't open %s\n", pPic->pathname);  
	    return -1;  
	}

	//step1:���������İ󶨣�Ϊcinfo�����ڴ沢��ʼ��
	cinfo.err = jpeg_std_error(&jerr.pub); 
	jerr.pub.error_exit = my_error_exit;
	
	// ������������Ҫ���ڴ��������ݽṹ�ĳ�ʼ��
	jpeg_create_decompress(&cinfo);

	 //step2:��fopen�򿪵�jpgͼƬinfile�ͽ�����cinfo�����
	jpeg_stdio_src(&cinfo, infile);

	//step3:��ȡͷ��Ϣ
	jpeg_read_header(&cinfo, TRUE);

	//step4:���ý������

	//step5: ����������
	jpeg_start_decompress(&cinfo);

	
	debug("image resolution: %d * %d, bpp/8 = %d.\n ",
	cinfo.output_width, cinfo.output_height, cinfo.output_components); 
	//�������������һ�е��ֽ���
	row_stride = cinfo.output_width * cinfo.output_components;
	//buffer = (*cinfo.mem->alloc_sarray) ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);
	buffer = (char *)malloc(row_stride);
	if(NULL == buffer)
    {
		fprintf(stderr, "cinfo.mem->alloc_sarray error.\n");
	}

	//step6:���н��룬����������������ݶ�������׼���õĻ�������
	 while (cinfo.output_scanline < cinfo.output_height) 
	 {          
	 //����һ�е���Ϣ������buffer��
	 	jpeg_read_scanlines(&cinfo, &buffer, 1);  //ԭ������buffer��Ӧ���β�Ӧ���Ƕ���ָ��

/*	 for(i=0; i<2000; i++)
	 {
		printf("%x ",buffer[i]);
	 }
*/	 
	 //��buffer�е�һ���������ߵ���ĵط�ȥ�ݴ��ʹ�ã���֮Ŀ�����ڳ�buffer�ռ�
	 //������ѭ������һ�ν����������������
		memcpy(pPic->pData + cinfo.output_scanline * row_stride , buffer, row_stride); 
	 }

	//step7:������ɣ�������.
	 jpeg_finish_decompress(&cinfo);
	 jpeg_destroy_decompress(&cinfo);
	 fclose(infile);


	//��ʾͼƬ��
	pPic->height = cinfo.output_height;
	pPic->width  = cinfo.output_width;
	pPic->bpp	 = cinfo.output_components*8;

	return 0;
}


// ��װ��һ������ʹ�õ�jpg��ʾ����
// ����������ֻ��Ҫһ��jpgͼƬ��pathname���ɣ���Щ���ӵ����ݽṹ����jpg��ʾģ���ڲ������
// ��ȷ��ʾͼƬ����0����ʾ�����г����򷵻�-1

int display_jpg(const char *pathname)
{
	//��һ��:������ͼƬ�ǲ���jpgͼƬ
	int ret = -1;
	struct pic_info picture;
	ret = is_jpg(pathname);
	if(ret != 0)
	{
		return -1;
	}
	//�ڶ���:������jpgͼƬ
	picture.pathname = (char *)pathname;
	picture.pData = rgb_buf;
	jpg_analyze(&picture);

	//������:��ʾ��ͼƬ
	fb_draw_jpg(&picture);
	return 0;

}


