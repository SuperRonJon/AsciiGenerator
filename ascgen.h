#pragma once
#include <stdbool.h>

typedef struct image_data {
    unsigned char* data;
    int height;
    int width;
    int channel_count;
} image_data;

image_data open_image(const char* filename);
char* image_to_string(const image_data* img, bool invert);
void resize_image(image_data* img, const int new_width, const int new_height);
void scale_image(image_data* img, const double w_scale, const double h_scale);
double get_pixel_brightness(const image_data* image, const int x, const int y);
image_data pack_image(unsigned char* data, int width, int height, int channel_count);