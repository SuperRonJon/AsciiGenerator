#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize2.h"

#include "imgmanip.h"

double get_pixel_brightness(const struct image_data *image, int x, int y)
{
    const size_t pixel_index = (y * image->width + x) * image->channel_count;

    const uint8_t red = (uint8_t)image->data[pixel_index];
    const uint8_t green = (uint8_t)image->data[pixel_index+1];
    const uint8_t blue = (uint8_t)image->data[pixel_index+2];
    const uint8_t alpha = image->channel_count >= 4 ? (uint8_t)image->data[pixel_index+3] : 255;

    const double pr = 0.299;
    const double pg = 0.587;
    const double pb = 0.114;

    const double brightness = sqrt((pr * red * red) + (pg * green * green) + (pb * blue * blue));
    return brightness * (alpha / 255.0);
}

char* image_to_string(const struct image_data *img, bool invert, const char *characters)
{
    const size_t chars_length = strlen(characters);
    const size_t char_count = (img->width * img->height) + img->height + 1;
    char *result_str = malloc(char_count);
    size_t result_itr = 0;
    if (result_str == NULL) {
        return NULL;
    }
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            const double brightness = get_pixel_brightness(img, x, y);
            const size_t char_index = (size_t)(brightness / (255.1 / chars_length));
            result_str[result_itr] = invert ? characters[chars_length - 1 - char_index] : characters[char_index];
            result_itr++;
        }
        result_str[result_itr] = '\n';
        result_itr++;
    }
    result_str[result_itr] = '\0';
    return result_str;
}

void open_image(struct image_data *img, const char *filename)
{
    int width, height, channel_count;
    unsigned char *data = stbi_load(filename, &width, &height, &channel_count, 0);
    if (!data) {
        const char *reason = stbi_failure_reason();
        fprintf(stderr, "Error loading image: %s", reason);
        if (strcmp(reason, "can't fopen") == 0) {
            fprintf(stderr, " - the file %s may not exist.", filename);
        }
        fputs("\n", stderr);
        exit(1);
    }
    img->data = data;
    img->width = width;
    img->height = height;
    img->channel_count = channel_count;
}

void resize_image(struct image_data *img, int new_width, int new_height)
{
    unsigned char *resized_data = malloc(new_width*new_height*img->channel_count);
    if (!resized_data) {
        fputs("Failed to allocate memory for resized image\n", stderr);
        exit(1);
    }

    stbir_resize(
        img->data, img->width, img->height, 0, 
        resized_data, new_width, new_height, 0, img->channel_count, 
        STBIR_TYPE_UINT8, STBIR_EDGE_CLAMP, STBIR_FILTER_POINT_SAMPLE
    );

    if (!resized_data) {
        fputs("Failed to resize image...\n", stderr);
        exit(1);
    }
    stbi_image_free(img->data);
    img->data = resized_data;
    img->width = new_width;
    img->height = new_height;
}

void scale_image(struct image_data *img, double w_scale, double h_scale)
{
    const int new_width = (int)(img->width * w_scale);
    const int new_height = (int)(img->height * h_scale);
    resize_image(img, new_width, new_height);
}

void free_image(struct image_data *img)
{
    stbi_image_free(img->data);
}