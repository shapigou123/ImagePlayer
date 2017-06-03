#ifndef __CONFIG_H__
#define __CONFIG_H__


/******** �궨�� ****************************************************************/

//�涨���֧��1920*1080��ͼƬ bpp���24
#define BMP_MAX_RESOLUTION		(1920*1080)
#define BMP_BUF_SIZE			(BMP_MAX_RESOLUTION*3)

#define PNG_BYTES_TO_CHECK 4


//�涨���֧��100��ͼƬ
#define MAX_IMAGE_CNT		100


//����ϵͳ�еĴ������豸���豸��
#define DEVICE_TOUCESCREEN		"/dev/input/event2"

#define TOUCH_WIDTH				200				// ���崥����ҳ����Ŀ��

/***********************ȫ�ֱ���****************************************************/
char rgb_buf[BMP_BUF_SIZE];



//#define DEBUG			// �򿪵�����Ϣ����Ŀ���

// debug��Ķ���
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



/********�ṹ�嶨�� ****************************************************************/


//�ýṹ������װһ��ͼƬ��Ϣ
typedef struct pic_info
{
	char *pathname;				//ͼƬ���ļ�ϵͳ�е�·����+�ļ���
	unsigned int width;			//ͼƬ�ֱ���֮��
	unsigned int height;		//ͼƬ�ֱ���֮��
	unsigned int bpp;			//ͼƬ֮�������
	char *pData;				//ָ��ͼƬ��Ч���ݴ洢��buf����
}pic_info;



/******** �������� ****************************************************************/

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










