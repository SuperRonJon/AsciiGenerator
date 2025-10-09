/*
* asciigen - CLI Ascii Art generator from image files 
* Copyright (c) 2025 Patrick Seute
*/

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize2.h"

#define VERSION "1.4"


typedef struct image_data {
    unsigned char *data;
    int height;
    int width;
    int channel_count;
} image_data;

double get_pixel_brightness(const image_data *image, const int x, const int y) {
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

void resize_image(image_data *img, const int new_width, const int new_height) {
    unsigned char *resized_data = malloc(new_width*new_height*img->channel_count);
    if(!resized_data) {
        fputs("Failed to allocate memory for resized image\n", stderr);
        exit(1);
    }

    stbir_resize(
        img->data, img->width, img->height, 0, 
        resized_data, new_width, new_height, 0, img->channel_count, 
        STBIR_TYPE_UINT8, STBIR_EDGE_CLAMP, STBIR_FILTER_POINT_SAMPLE
    );

    if(!resized_data) {
        fputs("Failed to resize image...\n", stderr);
        exit(1);
    }
    stbi_image_free(img->data);
    img->data = resized_data;
    img->width = new_width;
    img->height = new_height;
}

void scale_image(image_data *img, const double w_scale, const double h_scale) {
    const int new_width = (int)(img->width * w_scale);
    const int new_height = (int)(img->height * h_scale);
    resize_image(img, new_width, new_height);
}

char* image_to_string(const image_data *img, bool invert) {
    const char characters[] = "@%#*+=-:. ";
    const size_t chars_length = strlen(characters);
    const size_t char_count = (img->width * img->height) + img->height + 1;
    char *result_str = malloc(char_count);
    size_t result_itr = 0;
    if(result_str == NULL) {
        return NULL;
    }
    for(int y = 0; y < img->height; y++) {
        for(int x = 0; x < img->width; x++) {
            const double brightness = get_pixel_brightness(img, x, y);
            const size_t char_index = (int)(brightness / (255.1 / chars_length));
            result_str[result_itr] = invert ? characters[chars_length - 1 - char_index] : characters[char_index];
            result_itr++;
        }
        result_str[result_itr] = '\n';
        result_itr++;
    }
    result_str[result_itr] = '\0';
    return result_str;
}

void open_image(image_data *img, const char *filename) {
    int width, height, channel_count;
    unsigned char *data = stbi_load(filename, &width, &height, &channel_count, 0);
    if(!data) {
        const char *reason = stbi_failure_reason();
        fprintf(stderr, "Error loading image: %s", reason);
        if(strcmp(reason, "can't fopen") == 0) {
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

typedef struct config {
    char *filename;
    bool invert;
    double w_scaling;
    double h_scaling;
    double scaling;
} config;

void default_config(config *conf) {
    conf->filename = malloc(1);
    strncpy(conf->filename, "\0", 1);
    conf->invert = false;
    conf->h_scaling = -1.0;
    conf->w_scaling = -1.0;
    conf->scaling = 1.0;
}

char* str_dup(const char *s) {
    if(s == NULL) {
        return NULL;
    }
    size_t size = strlen(s) + 1;
    char *new_str = malloc(size);
    if(new_str == NULL) {
        return NULL;
    }
    strncpy(new_str, s, size);
    return new_str;
}

void print_version(void) {
    printf("asciigen - v%s\n", VERSION);
}

void print_help(void) {
    puts("Usage:\n       asciigen [options] image.png");
    puts("Options:");
    puts("  -i              inverts light and dark colors. Brightest pixels use densest characters");
    puts("  -w scale        Width scaling factor. Output's width will be original_width * scale");
    puts("  -h scale        Height scaling factor. Output's height will be original_height * scale");
    puts("  -s scale        Even scaling factor. Output's dimensions will be original * scale");
    puts("  -v, --version   Prints version");
    puts("  -H, --help      Prints help");
}

void set_config(config *conf, int argc, char **argv) {
    default_config(conf);
    bool get_filename = true;
    int scaling_token_index = -1;
    int h_scaling_token_index = -1;
    int w_scaling_token_index = -1;
    for(int i = 1; i < argc; i++) {
        char *token = argv[i];
        int index_mod = 1;
        if(strcmp(token, "--help") == 0) {
            get_filename = false;
            print_help();
            exit(0);
        }
        else if(strcmp(token, "--version") == 0) {
            get_filename = false;
            print_version();
            exit(0);
        }
        else if(token[0] == '-') {
            for(size_t j = 1; j < strlen(token); j++) {
                char currOpt = token[j];
                switch(currOpt) {
                    case 'i':
                        conf->invert = true;
                        break;
                    case 's':
                        scaling_token_index = i+index_mod;
                        index_mod++;
                        break;
                    case 'w':
                        w_scaling_token_index = i+index_mod;
                        index_mod++;
                        break;
                    case 'h':
                        h_scaling_token_index = i+index_mod;
                        index_mod++;
                        break;
                    case 'H':
                        get_filename = false;
                        print_help();
                        exit(0);
                        break;
                    case 'v':
                    case 'V':
                        get_filename = false;
                        print_version();
                        exit(0);
                        break;
                }
            }
        }
        else if(i == scaling_token_index && i != argc-1) {
            conf->scaling = strtod(argv[i], NULL);
        }
        else if(i == w_scaling_token_index && i != argc-1) {
            conf->w_scaling = strtod(argv[i], NULL);
        }
        else if(i == h_scaling_token_index && i != argc-1) {
            conf->h_scaling = strtod(argv[i], NULL);
        }
    }
    if(get_filename) {
        free(conf->filename);
        conf->filename = str_dup(argv[argc-1]);
        if(!conf->filename) {
            fputs("Error allocating memory for filename...\n", stderr);
            exit(1);
        }
    }
    const bool width_valid = conf->w_scaling > 0.0;
    const bool height_valid = conf->h_scaling > 0.0;
    const bool even_scaling = !width_valid && !height_valid;
    if(!even_scaling && (!width_valid || !height_valid)) {
        fputs("Invalid scaling parameters.\nIf not using equivalent scaling for height and width (-s) both height and width must be supplied and greater than 0.\n", stderr);
        exit(1);
    }
    if(even_scaling) {
        conf->w_scaling = conf->scaling;
        conf->h_scaling = conf->scaling;
    }

}

int main(int argc, char **argv) {
    if(argc < 2) {
        print_help();
        return 0;
    }

    config conf;
    set_config(&conf, argc, argv);
    
    image_data img;
    open_image(&img, conf.filename);
    if(conf.w_scaling != 1.0 || conf.h_scaling != 1.0)
        scale_image(&img, conf.w_scaling, conf.h_scaling);
    char *art = image_to_string(&img, conf.invert);
    stbi_image_free(img.data);
    if(!art) {
        fputs("Error creating art string... Unable to allocate memory\n", stderr);
        return 1;
    }
    puts(art);
    free(art);
    free(conf.filename);
    return 0;
}
