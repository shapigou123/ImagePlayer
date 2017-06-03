/*
 *
 *本文件用来解析jpg图片，并调用fb.c中的显示接口来显示lcd上
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




//自己定义的错误处理函数
METHODDEF(void)
my_error_exit (j_common_ptr cinfo)
{  
	//my_error_ptr myerr = (my_error_ptr) cinfo->err;
	//(*cinfo->err->output_message) (cinfo); 
	fprintf(stderr, "my_error_exit\n");
 	//longjmp(myerr->setjmp_buffer, 1);
}

/*函数功能: 判断一个图片文件是不是jpg图片
 *函数参数: path是图片文件的pathname
 *返回值:   如果是jpg则返回0，不是则返回1，错误返回-1
 */
int is_jpg(const char *pathname)
{
	FILE *file = NULL;
	char buf[2]={0};

	//打开文件
	file = fopen(pathname,"rb");
	if(NULL == file)
	{
		fprintf(stderr, "fopen %s error.\n",pathname);
		fclose(file);
		return -1;
	}

	//读出前2个字节
	fread(buf, 1, 2, file); // 2表示读两个元素的数据sizeof(buf)，1表示每个元素占一个字节 sizeof(char)
	//debug("read: 0x%x%x\n", buf[0], buf[1]);
	//判断是不是0xffd8
	if(!((buf[0] == 0xff) && (buf[1] == 0xd8)))
	{
		fclose(file);
		return 1;		//不是jpg图片
	}
	//是0xffd8则就继续读文件最后两个字节
	fseek(file, -2, SEEK_END);
	fread(buf, 1, 2, file);
//	debug("read: 0x%x%x\n", buf[0], buf[1]);

	if(!((buf[0] == 0xff) && (buf[1] == 0xd9)))
	{
	    fclose(file);
		return 1;		//不是jpg图片
	}
	fclose(file);
	return 0;
}

/*
 *函数功能:解码jpg图片，并将解码出来的数据存储
 *函数参数:pPic,记录源jpg图片的信息，解码出来的图片宽高、图片数据缓冲区等信息
 *返回值  :正确返回0，错误返回-1
 */
static int jpg_analyze(struct pic_info *pPic)
{
	struct jpeg_decompress_struct cinfo;		//cinfo贯穿整个解码过程的信息记录和传递的数据结构
	struct my_error_mgr jerr;					// 错误处理的
	FILE *infile;								// 指向fopen打开源jpg图片文件的指针
	//JSAMPARRAY buffer;							// 指向解码行数据的指针
	char *buffer = NULL;
	int row_stride;								// 解码出来的一行图片信息的字节数
   // int i;
	if ((infile = fopen(pPic->pathname, "rb")) == NULL) 
	{    
		fprintf(stderr, "can't open %s\n", pPic->pathname);  
	    return -1;  
	}

	//step1:错误处理函数的绑定，为cinfo分配内存并初始化
	cinfo.err = jpeg_std_error(&jerr.pub); 
	jerr.pub.error_exit = my_error_exit;
	
	// 给解码器做必要的内存分配和数据结构的初始化
	jpeg_create_decompress(&cinfo);

	 //step2:将fopen打开的jpg图片infile和解码器cinfo相关联
	jpeg_stdio_src(&cinfo, infile);

	//step3:读取头信息
	jpeg_read_header(&cinfo, TRUE);

	//step4:配置解码参数

	//step5: 启动解码器
	jpeg_start_decompress(&cinfo);

	
	debug("image resolution: %d * %d, bpp/8 = %d.\n ",
	cinfo.output_width, cinfo.output_height, cinfo.output_components); 
	//解码出来的数据一行的字节数
	row_stride = cinfo.output_width * cinfo.output_components;
	//buffer = (*cinfo.mem->alloc_sarray) ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);
	buffer = (char *)malloc(row_stride);
	if(NULL == buffer)
    {
		fprintf(stderr, "cinfo.mem->alloc_sarray error.\n");
	}

	//step6:逐行解码，并将解码出来的数据丢到事先准备好的缓冲区中
	 while (cinfo.output_scanline < cinfo.output_height) 
	 {          
	 //解码一行的信息，放入buffer中
	 	jpeg_read_scanlines(&cinfo, &buffer, 1);  //原函数中buffer对应的形参应该是二重指针

/*	 for(i=0; i<2000; i++)
	 {
		printf("%x ",buffer[i]);
	 }
*/	 
	 //将buffer中的一行数据移走到别的地方去暂存或使用，总之目的是腾出buffer空间
	 //给将来循环的下一次解码出来的行数据用
		memcpy(pPic->pData + cinfo.output_scanline * row_stride , buffer, row_stride); 
	 }

	//step7:解码完成，清理工作.
	 jpeg_finish_decompress(&cinfo);
	 jpeg_destroy_decompress(&cinfo);
	 fclose(infile);


	//显示图片。
	pPic->height = cinfo.output_height;
	pPic->width  = cinfo.output_width;
	pPic->bpp	 = cinfo.output_components*8;

	return 0;
}


// 封装的一个对外使用的jpg显示函数
// 本函数对外只需要一个jpg图片的pathname即可，那些复杂的数据结构都是jpg显示模块内部处理的
// 正确显示图片返回0，显示过程中出错则返回-1

int display_jpg(const char *pathname)
{
	//第一步:检测给的图片是不是jpg图片
	int ret = -1;
	struct pic_info picture;
	ret = is_jpg(pathname);
	if(ret != 0)
	{
		return -1;
	}
	//第二步:解析该jpg图片
	picture.pathname = (char *)pathname;
	picture.pData = rgb_buf;
	jpg_analyze(&picture);

	//第三步:显示该图片
	fb_draw_jpg(&picture);
	return 0;

}


