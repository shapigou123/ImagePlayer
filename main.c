#include <stdio.h>
#include <unistd.h>
#include <config.h>
#include<fb.h>

int main(void)
{
	int ret = -1; 
	
	printf("image decode player...\n");
	debug("test for debug %d\n",13);
	ret = fb_open();
	if(ret < 0)
	{
		debug("fb_open error.\n");
	}
	//fb_draw_back(1024, 600, RED);
//	fb_draw_picture5();
//	fb_draw_line(GREEN);
//	fb_draw_picture6(600,300);
//	fb_close();


//test bmp ok!!
	//display_bmp("123.bmp");
	//sleep(3);
	//display_jpg("456.jpg");
	//display_png("111.png");
	scan_image2("image");
	//while(1)
	//{
	//	show_images();
		
	//}
	ts_updown();

//test jpg

	return 0;
}
































