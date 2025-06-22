#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#include "ascgen.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize2.h"


double get_pixel_brightness(const image_data* image, const int x, const int y) {
    const int pixel_index = (y * image->width + x) * image->channel_count;

    const uint8_t red = (uint8_t)image->data[pixel_index];
    const uint8_t green = (uint8_t)image->data[pixel_index+1];
    const uint8_t blue = (uint8_t)image->data[pixel_index+2];
    const uint8_t alpha = image->channel_count >= 4 ? (uint8_t)image->data[pixel_index+3] : 255;

    const double pr = 0.299;
    const double pg = 0.587;
    const double pb = 0.114;

    double brightness = sqrt((pr * pow(red, 2)) + (pg * pow(green, 2)) + (pb * pow(blue, 2)));
    return brightness * (alpha / 255.0);
}

void resize_image(image_data* img, const int new_width, const int new_height) {
    unsigned char* resized_data = (unsigned char*)malloc(new_width*new_height*img->channel_count);
    if(!resized_data) {
        fputs("Failed to allocate memory for resized image\n", stderr);
        exit(1);
    }
    stbir_resize_uint8_linear(img->data, img->width, img->height, 0, resized_data, new_width, new_height, 0, img->channel_count);
    if(!resized_data) {
        fputs("Failed to resize image...\n", stderr);
        exit(1);
    }
    stbi_image_free(img->data);
    img->data = resized_data;
    img->width = new_width;
    img->height = new_height;
}

void scale_image(image_data* img, const double w_scale, const double h_scale) {
    const int new_width = (int)(img->width * w_scale);
    const int new_height = (int)(img->height * h_scale);
    resize_image(img, new_width, new_height);
}

char* image_to_string(const image_data* img, bool invert) {
    const int CHARACTERS_LENGTH = 10;
    const char characters[10] = {'@', '%', '#', '*', '+', '=', '-', ':', '.', ' '};
    const char characters_inverse[10] = {' ', '.', ':', '-', '=', '+', '*', '#', '%', '@'};
    const size_t char_count = (img->width * img->height) + img->height + 1;
    char* result_str = malloc(char_count);
    int result_itr = 0;
    if(result_str == NULL) {
        return NULL;
    }
    for(int row = 0; row < img->height; row++) {
        for(int col = 0; col < img->width; col++) {
            double brightness = get_pixel_brightness(img, col, row);
            int char_index = (int)(brightness / (255.1 / CHARACTERS_LENGTH));
            result_str[result_itr] = invert ? characters_inverse[char_index] : characters[char_index];
            result_itr++;
        }
        result_str[result_itr] = '\n';
        result_itr++;
    }
    result_str[result_itr] = '\0';
    return result_str;
}

image_data pack_image(unsigned char* data, int width, int height, int channel_count) {
    image_data result;
    result.data = data;
    result.width = width;
    result.height = height;
    result.channel_count = channel_count;
    return result;
}

image_data open_image(const char* filename) {
    int width, height, channel_count;
    unsigned char* data = stbi_load(filename, &width, &height, &channel_count, 0);
    if(!data) {
        const char* reason = stbi_failure_reason();
        fprintf(stderr, "Error loading image: %s", reason);
        if(strcmp(reason, "can't fopen") == 0) {
            fprintf(stderr, " - the file %s may not exist.", filename);
        }
        fputs("\n", stderr);
        exit(1);
    }
    return pack_image(data, width, height, channel_count);
}
