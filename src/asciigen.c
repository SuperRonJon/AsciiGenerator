/*
* asciigen - CLI Ascii Art generator from image files 
* Copyright (c) 2025 Patrick Seute
*/

#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "imgmanip.h"

int main(int argc, char **argv) 
{
    if (argc < 2) {
        print_help();
        return 0;
    }

    struct config conf;
    set_config(&conf, argc, argv);
    
    struct image_data img;
    open_image(&img, conf.filename);
    free(conf.filename);
    if (conf.w_scaling != 1.0 || conf.h_scaling != 1.0)
        scale_image(&img, conf.w_scaling, conf.h_scaling);
    char *art = image_to_string(&img, conf.invert, conf.character_set);
    free(conf.character_set);
    free_image(&img);
    if (!art) {
        fputs("Error creating art string... Unable to allocate memory\n", stderr);
        return 1;
    }
    puts(art);
    free(art);
    return 0;
}
