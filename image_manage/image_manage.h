#ifndef __IMAGE_MANAGE_H__
#define __IMAGE_MANAGE_H__


#define PATHNAME_LEN	256

//��ö������ʾͼƬ�ĸ�ʽ����
typedef enum image_type
{
	IMAGE_TYPE_BMP,
	IMAGE_TYPE_JPG,
	IMAGE_TYPE_PNG,
	IMAGE_TYPE_UNKOWN,
}image_type_e;

//�ýṹ���װһ��ͼƬ��Ϣ
typedef struct image_info
{
	char pathname[PATHNAME_LEN];
	image_type_e type;

}image_info_t;



#endif





