//
// Created by zeke on 17-8-16.
//

#include "image.h"
#include "str_utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <png.h>

#define ARGB_TO_RGB565(a, r, g, b) ((((r)&0xF8)>>3)|(((g)&0xFC)<<3)|(((b)&0xF8)<<8))

void load_image(const char *path, my_image *image) {
    if (path == NULL || strlen(path) == 0) {
        printf("path is empty\n");
        return;
    }
    char *postFix = get_post_fix(path);
    if (postFix == NULL) {
        return;
    }
    if (strcasecmp(postFix, "png") == 0) {
        read_png_file(path, image);
    } else if (strcasecmp(postFix, "jpeg") == 0 || strcasecmp(postFix, "jpg") == 0) {
        read_jpg_file(path, image);
    } else {
        printf("unknown file type %s \n", path);
    }
}

void read_png_file(const char *path, my_image *image) {
    printf("start to read png file %s \n", path);
    FILE *file = fopen(path, "rb");
    if (!file) {
        printf("open file failed \n");
        return;
    }
    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png == NULL) {
        printf("allocate mem for png_struct failed");
        return;
    }

    png_infop info = png_create_info_struct(png);
    if (!info) {
        printf("read info failed \n");
        return;
    }
    if (setjmp(png_jmpbuf(png))) {
        printf("setjmp failed");
        return;
    }
    printf("start io --> \n");
    png_init_io(png, file);
    printf("start read info \n");
    png_read_info(png, info);
    printf("try to get width\n");

    image->width = png_get_image_width(png, info);
    printf("try to get height \n");
    image->height = png_get_image_height(png, info);
    printf("height %d  width %d \n", image->height, image->width);
    png_byte color_type = png_get_color_type(png, info);
    png_byte bit_depth = png_get_bit_depth(png, info);
    if (bit_depth == 16) {
        png_set_strip_16(png);
    }
    if (color_type == PNG_COLOR_TYPE_PALETTE) {
        png_set_palette_to_rgb(png);
    }
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) {
        png_set_expand_gray_1_2_4_to_8(png);
    }
    if (png_get_valid(png, info, PNG_INFO_tRNS)) {
        png_set_tRNS_to_alpha(png);
    }
    if (color_type == PNG_COLOR_TYPE_RGB ||
        color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_PALETTE) {
        png_set_filler(png, 0xff, PNG_FILLER_AFTER);
    }

    if (color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png);

    png_read_update_info(png, info);
    png_bytepp bytep = malloc(sizeof(png_bytep) * image->height);
    int y = 0;
    for (; y < image->height; ++y) {
        bytep[y] = malloc(png_get_rowbytes(png, info));
    }
    png_read_image(png, bytep);

    image->data = malloc(image->height * image->width * sizeof(short));
    memset(image->data, 0, sizeof(short) * image->height * image->width);
    int h, w;
    unsigned short *temp = (unsigned short *) image->data;

    int location_0, location_1;
    printf("start to convert \n");
    for (h = 0; h < image->height; ++h) {
        for (w = 0; w < image->width; ++w) {
            location_0 = h * image->width + w;
            location_1 = w << 2;
//            printf("location %d location %d h %d w %d \n", location_0, location_1, h, w);
            temp[location_0] = (unsigned short) ARGB_TO_RGB565(bytep[h][location_1 + 3], bytep[h][location_1],
                                                               bytep[h][location_1 + 1],
                                                               bytep[h][location_1 + 2]);

        }
    }

    for (y = 0; h < image->height; ++y) {
        free(bytep[y]);
    }
    free(bytep);
    png_destroy_read_struct(&png, &info, NULL);
    fclose(file);
    printf("load png done \n");
}

void read_jpg_file(const char *path, my_image *image) {

}

void write_png_file(const char *path, my_image *image) {
    FILE *file = fopen(path, "wb");
    if (!file) {
        printf("open file failed");
        return;
    }

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) {
        printf("create png write struct failed");
        return;
    }

    png_infop info = png_create_info_struct(png);
    if (!info) {
        printf("create png info failed");
        return;
    }

    if (setjmp(png_jmpbuf(png))) {
        printf("setjmp failed");
        return;
    }

    png_init_io(png, file);

    png_set_IHDR(png, info, image->width, image->height, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png, info);
}
