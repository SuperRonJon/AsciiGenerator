/*
* asciigen - CLI Ascii Art generator from image files 
* Copyright (c) 2025 Patrick Seute
*/

#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize2.h"

#define VERSION "1.2"


typedef struct image_data {
    unsigned char* data;
    int height;
    int width;
    int channel_count;
} image_data;

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

typedef struct config {
    char* filename;
    bool invert;
    double w_scaling;
    double h_scaling;
    double scaling;
} config;

config default_config() {
    config conf;
    conf.filename = malloc(1);
    strcpy(conf.filename, "");
    conf.invert = false;
    conf.h_scaling = -1.0;
    conf.w_scaling = -1.0;
    conf.scaling = 1.0;
    return conf;
}

void print_version() {
    printf("asciigen - v%s\n", VERSION);
}

void print_help() {
    puts("Usage:\n       asciigen [options] image.png");
    puts("Options:");
    puts("    -i              inverts light and dark colors. Brightest pixels use densest characters");
    puts("    -w scale        Width scaling factor. Output's width will be original_width * scale");
    puts("    -h scale        Height scaling factor. Output's height will be original_height * scale");
    puts("    -s scale        Even scaling factor. Output's dimensions will be original * scale");
    puts("    -v, --version   Prints version");
    puts("    -H, --help      Prints help");
}

void set_config(config* conf, int argc, char** argv) {
    bool get_filename = true;
    int scaling_token_index = -1;
    int h_scaling_token_index = -1;
    int w_scaling_token_index = -1;
    for(int i = 1; i < argc; i++) {
        char* token = argv[i];
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
            for(int j = 1; j < strlen(token); j++) {
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
        conf->filename = strdup(argv[argc-1]);
        if(!conf->filename) {
            fputs("Error allocating memory for filename...\n", stderr);
            exit(1);
        }
    }
    bool width_valid = conf->w_scaling > 0.0;
    bool height_valid = conf->h_scaling > 0.0;
    bool even_scaling = !width_valid && !height_valid;
    if(!even_scaling && (!width_valid || !height_valid)) {
        fputs("Invalid scaling parameters.\nIf not using equivalent scaling for height and width (-s) both height and width must be supplied and greater than 0.\n", stderr);
        exit(1);
    }
    if(even_scaling) {
        conf->w_scaling = conf->scaling;
        conf->h_scaling = conf->scaling;
    }

}

int main(int argc, char** argv) {
    if(argc < 2) {
        print_help();
        return 0;
    }

    config conf = default_config();
    set_config(&conf, argc, argv);
    
    image_data img = open_image(conf.filename);
    if(conf.w_scaling != 1.0 || conf.h_scaling != 1.0)
        scale_image(&img, conf.w_scaling, conf.h_scaling);
    char* art = image_to_string(&img, conf.invert);
    if(!art) {
        fputs("Error creating art string... Unable to allocate memory\n", stderr);
        return 1;
    }
    puts(art);
    free(art);
    free(conf.filename);
    stbi_image_free(img.data);
    return 0;
}
