#include <config.h>

#define FBDEVICE "/dev/fb0"
#define WHITE		0xffffffff
#define RED			0xffff0000
#define GREEN		0xff00ff00
#define BLUE		0xff0000ff
 
 
#define WIDTH 		1024
#define HEIGHT		600
 
int  fb_open(void);
void fb_close(void);
void fb_draw_back(unsigned int width, unsigned int height, unsigned int color);
void fb_draw_line(unsigned int color);
/*
void fb_draw_picture1(void);
void fb_draw_picture2(void);
void fb_draw_picture3(void);
void fb_draw_picture4(void);
void fb_draw_picture5(void);
void fb_draw_picture6(unsigned int x0, unsigned int y0);
void fb_draw_picture7(unsigned int x0, unsigned int y0);*/
