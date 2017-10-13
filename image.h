//
// Created by zeke on 17-8-16.
//

#ifndef FRAMEBUFFER_IMAGE_H
#define FRAMEBUFFER_IMAGE_H
typedef struct my_image {
    int width;
    int height;
    unsigned char *data;
} my_image;

void load_image(const char *path, my_image *image);

void read_png_file(const char *path, my_image *image);

void read_jpg_file(const char *path, my_image *image);

void write_png_file(const char *path, my_image *image);
#endif //FRAMEBUFFER_IMAGE_H
