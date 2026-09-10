#ifndef ASCIIIMGMANIP_H
#define ASCIIIMGMANIP_H

#include <stdbool.h>

struct image_data {
    unsigned char *data;
    int height;
    int width;
    int channel_count;
};

void open_image(struct image_data *img, const char *filename);
char* image_to_string(const struct image_data *img, bool invert, const char *characters);
void resize_image(struct image_data *img, int new_width, int new_height);
void scale_image(struct image_data *img, double w_scale, double h_scale);
double get_pixel_brightness(const struct image_data *image, int x, int y);
void free_image(struct image_data *img);

#endif