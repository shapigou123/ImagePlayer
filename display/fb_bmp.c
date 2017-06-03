//本文件用来解析BMP图片，并且显示到fb中
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>


#include "bmp.h"
#include <fb.h>
#include <config.h>



//判断一个图片是否为合法的bmp文件
//返回值:如果是则返回0,不是则返回-1
int is_bmp(const char *pathname)
{
	int fd = -1;
	ssize_t ret = 0;
	char buf[2] = {0};
	//第一步打开图片
	fd = open(pathname, O_RDONLY);
	if(fd < 0)
	{
		debug("open %s failed\n",pathname);
		return -1;
	}
	
	//第二步读取头信息
	ret = read(fd, buf, 2);
	if(ret != 2)
	{
		debug("read %s failed",pathname);
		ret = -1;
		goto close;
	}
	
	//第三步解析头信息
	if((buf[0] == 'B') && (buf[1] == 'M'))
	{
		//debug("the %s file  is  a real bmp picture\n", pathname);
		ret = 0;
	}		
	else
	{
		//debug("the %s file  is not a bmp picture\n", pathname);
		ret = -1;
		goto close;
	}
		
close:	
	close(fd);
	return ret;
		
	}



//参数列表:path:要解析的bmp图片的pathname
//函数功能:该函数解析path这个bmp图片，并且将解析出来的图片数据丢到bmp_buf中去
//返回值:错误时返回-1；正确返回0
static int bmp_analyze(struct pic_info *pPic)
{
	int fd = -1;
	unsigned long int len = -1;
	int ret = -1;
    BitMapFileHeader fHeader;
	BitMapInfoHeader info;
	unsigned tmp;
	//第一步打开图片
	fd = open(pPic->pathname, O_RDONLY);
	if(fd < 0)
	{
		debug("open %s failed\n", pPic->pathname);
		return -1;
	}

	//第二步读取头信息
	read(fd, &tmp, 2); //文件内部维护了一个文件指针，读了2个字指针后，文件指针往后移动了两个指针。
	read(fd, &fHeader, sizeof(fHeader));	
	debug("bfSize =  %ld. \n",fHeader.bfSize);
	debug("bfOffBits = %ld. \n",fHeader.bfOffBits);

	//第三步读取图片信息
	read(fd, &info, sizeof(info));
	debug("biWidth =  %ld. \n",info.biWidth);
	debug("biHeight =  %ld. \n",info.biHeight);
	debug("picture bpp =  %u. \n",info.biBitCount);
	
	//将数据放入我们自己定义的结构体中
	pPic->width = info.biWidth;
	pPic->height = info.biHeight;
	pPic->bpp = info.biBitCount;
	//第四步读取图片数据
	len = (info.biWidth*info.biHeight*info.biBitCount)/3;
	ret = lseek(fd, fHeader.bfOffBits, SEEK_SET);
	if(ret < 0)
	{
		debug("lseek error\n");
	}
	debug("lseek postion: %d.\n",ret);

	read(fd, rgb_buf, len);
	//显示图片内容
	close(fd);
	return 0;
	
}

int display_bmp(const char *pathname)
{
	//第一步:检测给的图片是不是jpg图片
	int ret = -1;
	struct pic_info picture;
	ret = is_bmp(pathname);
	if(ret != 0)
	{
		return -1;
	}
	
	//第二步:解析该jpg图片
	picture.pathname = (char *)pathname;
	picture.pData = rgb_buf;
	bmp_analyze(&picture);

	//第三步:显示该图片
	fb_draw_bmp(&picture);
	return 0;
}















