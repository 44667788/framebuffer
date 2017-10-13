//
// Created by zeke on 17-8-14.
//

#ifndef FRAMEBUFFER_FRAMEBUFFER_H
#define FRAMEBUFFER_FRAMEBUFFER_H

#include <linux/fb.h>

#define FB_DEV "/dev/fb0"
struct fb_fix_screeninfo finfo;
struct fb_var_screeninfo vinfo;
int fb_fd;
long screensize;
//double buffer
char *bfb;
char *ffb;
unsigned long map_len;

/**
 * init fb
 * @return 0 for ok
 */
int initfb();

void swap_buffer();

/**
 * 画出一个矩形
 * @param color
 */
void draw_rect(__uint8_t r, __int8_t g, __int8_t b, int x, int y, int w, int h);

/**
 * draw image
 */
void draw_image(int w, int h, unsigned char *data);

void getOps();

void destroy();

#endif //FRAMEBUFFER_FRAMEBUFFER_H
