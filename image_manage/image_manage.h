#ifndef __IMAGE_MANAGE_H__
#define __IMAGE_MANAGE_H__


#define PATHNAME_LEN	256

//用枚举来表示图片的格式类型
typedef enum image_type
{
	IMAGE_TYPE_BMP,
	IMAGE_TYPE_JPG,
	IMAGE_TYPE_PNG,
	IMAGE_TYPE_UNKOWN,
}image_type_e;

//用结构体封装一个图片信息
typedef struct image_info
{
	char pathname[PATHNAME_LEN];
	image_type_e type;

}image_info_t;



#endif





