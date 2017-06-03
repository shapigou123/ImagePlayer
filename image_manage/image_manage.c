

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>

#include <fb.h>
#include <config.h>

#include "image_manage.h"


// images数组本来是空的，然后程序初始化时会去一个事先约定好的目录(image目录)下去

image_info_t images[MAX_IMAGE_CNT];
unsigned int image_index = 0;			// 数组images中的计数index





// 递归检索所有的文件及子文件夹，并且将所有的图片格式收集并且填充记录到images数组中
// 经过检索后，image数组中就记录了所有的图片，然后显示图片逻辑部分再去这个图片库中
// 拿出相应的图片来显示即可
// path是要去检索的文件夹的pathname
int scan_image(const char *path)

{
	DIR *dir;
	struct dirent *ptr;
	char base[1000];
 
	if ((dir=opendir(path)) == NULL)
	{
		perror("Open dir error...");
		exit(1);
	}
 
	while ((ptr=readdir(dir)) != NULL)
	{
		if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0)	
			continue;

	//	debug("d_name = %s.\n", ptr->d_name);
	//	debug("d_type = %d, DT_REG = %d, DT_DIR = %d, DT_UNKNOWN = %d.\n", 
	//		ptr->d_type, DT_REG, DT_DIR, DT_UNKNOWN);

	// readdir函数每调用一次就会返回opendir打开的basepath目录下的一个文件，直到
	// basepath目录下所有文件都被读完之后，就会返回NULL	
		switch(ptr->d_type)
		{
			case DT_REG:				//普通文件
				printf("d_name:%s/%s\n",path,ptr->d_name);
				break;
			case DT_DIR: 				//文件夹
			   memset(base,'\0',sizeof(base));
			   strcpy(base,path);
			   strcat(base,"/");
		  	   strcat(base,ptr->d_name);
		       scan_image(base);
			   break;
			case DT_UNKNOWN:
				debug("unknow file type\n");		
			default:
				break;				
		}		  
			  
	   }
	   closedir(dir);
	   return 0;
 }

 int scan_image2(const char *path)
 
 {
	 DIR *dir;
	 struct dirent *ptr;
	 char base[1000];  //存储的是全路径名
  	 struct stat sta;
	 if ((dir=opendir(path)) == NULL)
	 {
		 perror("Open dir error...");
		 exit(1);
	 }
  
	 while ((ptr=readdir(dir)) != NULL)
	 {
		 if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0)	 
			 continue; 

		//把文件的pathname给填充完整
		memset(base,'\0',sizeof(base));
		strcpy(base,path);
		strcat(base,"/");
		strcat(base,ptr->d_name);
		
		lstat(base, &sta);

		if (S_ISREG(sta.st_mode))
		{
			printf("regular file.\n");
			printf("d_name:%s/%s\n", path, ptr->d_name);
			// 如果是普通文件，就要在这里进行处理:
			// 处理思路就是 先判定是否属于已知的某种图片格式，如果是则放到images数组中
			// 如果都不属于则不理他

			if (!is_bmp(base))
			{
				strcpy(images[image_index].pathname, base);   
				//pathname 是数组不能直接赋值，用strcpy。因为前面用memset给数组全部赋值为'\0',是字符串格式
				images[image_index].type = IMAGE_TYPE_BMP;
			}
			if (!is_jpg(base))
			{
				strcpy(images[image_index].pathname, base);
				images[image_index].type = IMAGE_TYPE_JPG;
			}
			if (!is_png(base))
			{
				strcpy(images[image_index].pathname, base);
				images[image_index].type = IMAGE_TYPE_PNG;
			}		
			image_index++;
		}
		if (S_ISDIR(sta.st_mode))
		{
			printf("directory.\n");
			printf("d_name:%s/%s\n", path, ptr->d_name);
			scan_image2(base);
		}
	}
	
		closedir(dir);
		return 0;
  }


void print_images_info(void)
{
	int i;

	printf("iamge_index = %d.\n", image_index);
	for (i=0; i<image_index; i++)
	{
		printf("images[i].pathname = %s,	type = %d.\n", images[i].pathname, images[i].type);
	}
}


void show_images(void)
{
	int i;
	for (i=0; i<image_index; i++)
	{
		switch(images[i].type)
		{
			case IMAGE_TYPE_JPG:
				display_jpg(images[i].pathname);	break;
					
			case IMAGE_TYPE_BMP:
				display_bmp(images[i].pathname);	break;
					
			case IMAGE_TYPE_PNG:
				display_png(images[i].pathname);	break;		

			default:								break;
		}
		sleep(2);
	}

}

static void show_image(int index)
{
		debug("index = %d.\n", index);
		switch(images[index].type)
		{
			case IMAGE_TYPE_JPG:
				display_jpg(images[index].pathname);	break;
					
			case IMAGE_TYPE_BMP:
				display_bmp(images[index].pathname);	break;
					
			case IMAGE_TYPE_PNG:
				display_png(images[index].pathname);	break;		

			default:								break;
		}

}


int ts_updown(void)
{
	// 第一步: 触摸屏的触摸操作检测
	int fd = -1, ret = -1;
	struct input_event ev;
	int i = 0;				//用来记录当前显示的是第几个图片
	fd = open(DEVICE_TOUCESCREEN, O_RDONLY);
	if (fd < 0)
	{
		perror("open");
		return -1;
	}
	while (1)
	{
		//debug("image_index = %d.\n", image_index);
		memset(&ev, 0, sizeof(struct input_event));
	
		ret = read(fd, &ev, sizeof(struct input_event));
	
		if(ret != sizeof(struct input_event))
		{
			perror("read");
			close(fd);
			return -1;
		}

	// 第二步: 根据触摸坐标来翻页	
/*	
		printf("------------------------------\n");
		printf("type: %hd\n", ev.type);
		printf("code: %hd\n", ev.code);
		printf("value: %d\n", ev.value);
		printf("\n");
*/
		if((ev.code == ABS_X) && (ev.type == EV_ABS))
		{
			if((ev.value >= 0) && (ev.value < TOUCH_WIDTH))
			{
				//上翻页
				if (i-- <= 1)
				{
					i = image_index;
					
					debug("i=%d.\n", i);
				}
			}

			else if((ev.value >= (WIDTH - TOUCH_WIDTH)) && (ev.value < WIDTH))
			{
				//下翻页
				if (i++ >= image_index)
				{
					i = 1;
					debug("i=%d.\n", i);
				}
			}

			else
			{
				//不翻页
			}
			show_image(i - 1);
		}
	}
	
	close(fd);

	return 0;
	
	
}








