//
// Created by zeke on 17-8-14.
//

#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <sys/user.h>
#include "framebuffer.h"

#define RGB_PIXEL(r, g, b, vf)  (((unsigned char)r)>>3)<<vf->red.offset|(((unsigned char)g)>>2)<<vf->green.offset|((unsigned char)b)>>3<<vf->blue.offset

void printFixedInfo() {
    printf("Fixed screen info:\n"
                   "\tid: %s\n"
                   "\tsmem_start: 0x%lx\n"
                   "\tsmem_len: %d\n"
                   "\ttype: %d\n"
                   "\ttype_aux: %d\n"
                   "\tvisual: %d\n"
                   "\txpanstep: %d\n"
                   "\typanstep: %d\n"
                   "\tywrapstep: %d\n"
                   "\tline_length: %d\n"
                   "\tmmio_start: 0x%lx\n"
                   "\tmmio_len: %d\n"
                   "\taccel: %d\n"
                   "\n",
           finfo.id, finfo.smem_start, finfo.smem_len, finfo.type,
           finfo.type_aux, finfo.visual, finfo.xpanstep, finfo.ypanstep,
           finfo.ywrapstep, finfo.line_length, finfo.mmio_start,
           finfo.mmio_len, finfo.accel);
}

void
printVariableInfo() {
    printf("Variable screen info:\n"
                   "\txres: %d\n"
                   "\tyres: %d\n"
                   "\txres_virtual: %d\n"
                   "\tyres_virtual: %d\n"
                   "\txoffset: %d\n"
                   "\tyoffset: %d\n"
                   "\tbits_per_pixel: %d\n"
                   "\tgrayscale: %d\n"
                   "\tred: offset: %2d, length: %2d, msb_right: %2d\n"
                   "\tgreen: offset: %2d, length: %2d, msb_right: %2d\n"
                   "\tblue: offset: %2d, length: %2d, msb_right: %2d\n"
                   "\ttransp: offset: %2d, length: %2d, msb_right: %2d\n"
                   "\tnonstd: %d\n"
                   "\tactivate: %d\n"
                   "\theight: %d\n"
                   "\twidth: %d\n"
                   "\taccel_flags: 0x%x\n"
                   "\tpixclock: %d\n"
                   "\tleft_margin: %d\n"
                   "\tright_margin: %d\n"
                   "\tupper_margin: %d\n"
                   "\tlower_margin: %d\n"
                   "\thsync_len: %d\n"
                   "\tvsync_len: %d\n"
                   "\tsync: %d\n"
                   "\tvmode: %d\n"
                   "\n",
           vinfo.xres, vinfo.yres, vinfo.xres_virtual, vinfo.yres_virtual,
           vinfo.xoffset, vinfo.yoffset, vinfo.bits_per_pixel,
           vinfo.grayscale, vinfo.red.offset, vinfo.red.length,
           vinfo.red.msb_right, vinfo.green.offset, vinfo.green.length,
           vinfo.green.msb_right, vinfo.blue.offset, vinfo.blue.length,
           vinfo.blue.msb_right, vinfo.transp.offset, vinfo.transp.length,
           vinfo.transp.msb_right, vinfo.nonstd, vinfo.activate,
           vinfo.height, vinfo.width, vinfo.accel_flags, vinfo.pixclock,
           vinfo.left_margin, vinfo.right_margin, vinfo.upper_margin,
           vinfo.lower_margin, vinfo.hsync_len, vinfo.vsync_len,
           vinfo.sync, vinfo.vmode);
}

int initfb() {

    fb_fd = 0;
    ffb = NULL;
    bfb = NULL;

    fb_fd = open(FB_DEV, O_RDWR);
    if (fb_fd == -1) {
        fprintf(stderr, "open fb dev failed ---》　%s \n", FB_DEV);
        return -1;
    }
    if (ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
        fprintf(stderr, "FBIOGET_VSCREENINFO FAILED\n");
        return -1;
    }


    if (ioctl(fb_fd, FBIOGET_FSCREENINFO, &finfo)) {
        fprintf(stderr, "FBIOGET_FSCREENINFO FAILED \n");
        return -1;
    }

    vinfo.yres_virtual = vinfo.yres * 2;

    if (ioctl(fb_fd, FBIOPUT_VSCREENINFO, &vinfo) == -1) {
        fprintf(stderr, "FBIOPUT_VSCREENINFO FAILED \n");
    } else {
        if (ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
            fprintf(stderr, "FBIOGET_VSCREENINFO FAILED\n");
            return -1;
        }
    }
    printFixedInfo();
    printVariableInfo();

    unsigned long fb_mem_offset = finfo.smem_start & (~PAGE_MASK);

    unsigned long map_len = fb_mem_offset + finfo.smem_len;

    screensize = vinfo.yres * finfo.line_length;
    ffb = mmap(0, map_len, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0);
    memset(ffb, 0x1f, map_len);
    bfb = ffb + screensize;
    fprintf(stderr, "init done ,map_len -->  size %lu screen_size %lu \n", map_len, screensize);
    return 0;
}

inline void swap_buffer() {
    if (vinfo.yoffset == 0)
        vinfo.yoffset = vinfo.yres;
    else
        vinfo.yoffset = 0;

    //"Pan" to the back buffer
    if (ioctl(fb_fd, FBIOPAN_DISPLAY, &vinfo) == -1) {
        printFixedInfo();
        printVariableInfo();
        fprintf(stderr, "error FBIOPAN_DISPALY failed\n");
    }

    //Update the pointer to the back buffer so we don't draw on the front buffer
    long tmp;
    tmp = (long) ffb;
    ffb = bfb;
    bfb = (char *) tmp;
}

void draw_rect(__uint8_t r, __int8_t g, __int8_t b, int x, int y, int w, int h) {
    __u32 width = (__u32) (x + w);
    width = width > vinfo.xres ? vinfo.xres : width;
    __u32 height = (__u32) (y + h);
    height = height > vinfo.yres ? vinfo.yres : height;
    int i, j;
    int location;
    unsigned short *p = (unsigned short *) bfb;
    struct fb_var_screeninfo *vfp = &vinfo;
    unsigned short pixel = (unsigned short) RGB_PIXEL(r, g, b, vfp);
    printf("r %u g %u  b %u w %d h %d pixel %x \n", r, g, b, width, height, pixel);
    for (i = 0; i < height; ++i) {
        for (j = 0; j < width; ++j) {
            location = i * finfo.line_length + ((j + vinfo.xoffset) << 1);
            p[location] = pixel;
        }
    }
    swap_buffer();
}

void destroy() {
    close(fb_fd);
    munmap(ffb, map_len);

}

void draw_image(int w, int h, unsigned char *data) {
    int width = w > vinfo.xres ? vinfo.xres : w;
    int height = h > vinfo.yres ? vinfo.yres : h;
    int i, j;
    unsigned short *dst = (unsigned short *) bfb;
    unsigned short *src = (unsigned short *) data;
    memset(dst, -1, screensize);
    int location_0, location_1;
    printf("w %d h %d \n", width, height);
    for (i = 0; i < height; ++i) {
        for (j = 0; j < width; ++j) {
            location_0 = i * vinfo.xres + j;
            location_1 = i * w + j;
            dst[location_0] = src[location_1];
        }
    }
    swap_buffer();
}
