/*
* asciigen - CLI Ascii Art generator from image files 
* Copyright (c) 2025 Patrick Seute
*/


#include <stdio.h>
#include <stdlib.h>

#include "ascgen.h"
#include "config.h"


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
    free(img.data);
    return 0;
}
