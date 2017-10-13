/**
 * This project is use to learn about framebuffer
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "framebuffer.h"
#include "image.h"

int main() {
    printf("Hello, World!\n");
    initfb();
    int i;
    int colorCount = 100;
    __uint8_t colors[][3] = {{0xff, 0,    0},
                             {0,    0xff, 0},
                             {0,    0,    0xff}};
    int idx;
//    for (i = 0; i < colorCount; ++i) {
//        idx = i % 3;
//        draw_rect(colors[idx][0], colors[idx][1], colors[idx][2], 0, 0, vinfo.xres, vinfo.yres);
//        printf("draw %d done \n", i);
//        sleep(1);
//    }
    my_image image;
    load_image("test.png", &image);
    draw_image(image.width, image.height, image.data);
    destroy();
    free(image.data);
    return 0;
}