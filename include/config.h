#ifndef __CONFIG_H__
#define __CONFIG_H__


/******** 宏定义 ****************************************************************/

//规定最大支持1920*1080的图片 bpp最多24
#define BMP_MAX_RESOLUTION		(1920*1080)
#define BMP_BUF_SIZE			(BMP_MAX_RESOLUTION*3)

#define PNG_BYTES_TO_CHECK 4


//规定最多支持100张图片
#define MAX_IMAGE_CNT		100


//定义系统中的触摸屏设备的设备名
#define DEVICE_TOUCESCREEN		"/dev/input/event2"

#define TOUCH_WIDTH				200				// 定义触摸翻页区域的宽度

/***********************全局变量****************************************************/
char rgb_buf[BMP_BUF_SIZE];



//#define DEBUG			// 打开调试信息输出的开关

// debug宏的定义
#ifdef DEBUG
#define debug(...)                                                      \
        {                                                               \
            fprintf(stderr, "[debug][%s:%s:%d] ",                     \
                    __FILE__, __FUNCTION__, __LINE__);                  \
            fprintf(stderr, __VA_ARGS__);                               \
        }
#else
#define debug(...)  
#endif



/********结构体定义 ****************************************************************/


//用结构体来封装一个图片信息
typedef struct pic_info
{
	char *pathname;				//图片在文件系统中的路径名+文件名
	unsigned int width;			//图片分辨率之宽
	unsigned int height;		//图片分辨率之高
	unsigned int bpp;			//图片之像素深度
	char *pData;				//指向图片有效数据存储的buf数据
}pic_info;



/******** 函数定义 ****************************************************************/

int is_bmp(const char *pathname);
int is_jpg(const char *pathname);
int is_png(const char *pathname);


int display_jpg(const char *pathname);
int display_bmp(const char *pathname);
int display_png(const char *pathname);

void fb_draw_bmp(const struct pic_info *pPic);
void fb_draw_jpg(const struct pic_info *pPic);

int scan_image2(const char *path);
void print_images_info(void);
void show_images(void);

int ts_updown(void);


#endif










