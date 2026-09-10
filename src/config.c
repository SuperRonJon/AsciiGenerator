#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"

static char* str_dup(const char *s)
{
    if (s == NULL) {
        return NULL;
    }
    size_t size = strlen(s) + 1;
    char *new_str = malloc(size);
    if (new_str == NULL) {
        return NULL;
    }
    strncpy(new_str, s, size);
    return new_str;
}

void default_config(struct config *conf)
{
    conf->filename = str_dup("");
    conf->character_set = str_dup("@%#*+=-:. ");
    conf->invert = false;
    conf->h_scaling = -1.0;
    conf->w_scaling = -1.0;
    conf->scaling = 1.0;
}

void print_version(void)
{
    printf("asciigen - v%s\n", VERSION);
}

void print_help(void)
{
    puts("Usage:\n       asciigen [options] image.png");
    puts("Options:");
    puts("  -i              inverts light and dark colors. Brightest pixels use densest characters");
    puts("  -w scale        Width scaling factor. Output's width will be original_width * scale");
    puts("  -h scale        Height scaling factor. Output's height will be original_height * scale");
    puts("  -s scale        Even scaling factor. Output's dimensions will be original * scale");
    puts("  -c \"chars\"      Custom character set chars will be used rather than the default of \"@%#*+=-:. \"");
    puts("  -v, --version   Prints version");
    puts("  -H, --help      Prints help");
}

void set_config(struct config *conf, int argc, char **argv)
{
    default_config(conf);
    bool get_filename = true;
    int scaling_token_index = -1;
    int h_scaling_token_index = -1;
    int w_scaling_token_index = -1;
    int custom_characters_index = -1;
    for (int i = 1; i < argc; i++) {
        char *token = argv[i];
        int index_mod = 1;
        if (strcmp(token, "--help") == 0) {
            get_filename = false;
            print_help();
            exit(0);
        } else if (strcmp(token, "--version") == 0) {
            get_filename = false;
            print_version();
            exit(0);
        } else if (token[0] == '-') {
            for (size_t j = 1; j < strlen(token); j++) {
                char currOpt = token[j];
                switch (currOpt) {
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
                    case 'c':
                        custom_characters_index = i+index_mod;
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
        } else if (i == scaling_token_index && i != argc-1) {
            conf->scaling = strtod(argv[i], NULL);
        } else if (i == w_scaling_token_index && i != argc-1) {
            conf->w_scaling = strtod(argv[i], NULL);
        } else if (i == h_scaling_token_index && i != argc-1) {
            conf->h_scaling = strtod(argv[i], NULL);
        } else if (i == custom_characters_index && i != argc-1) {
            if (conf->character_set != NULL)
                free(conf->character_set);
            conf->character_set = str_dup(argv[i]);
        }
    }
    if (get_filename) {
        free(conf->filename);
        conf->filename = str_dup(argv[argc-1]);
        if (!conf->filename) {
            fputs("Error allocating memory for filename...\n", stderr);
            exit(1);
        }
    }
    const bool width_valid = conf->w_scaling > 0.0;
    const bool height_valid = conf->h_scaling > 0.0;
    const bool even_scaling = !width_valid && !height_valid;
    if (!even_scaling && (!width_valid || !height_valid)) {
        fputs("Invalid scaling parameters.\nIf not using equivalent scaling for height and width (-s) both height and width must be supplied and greater than 0.\n", stderr);
        exit(1);
    }
    if (even_scaling) {
        conf->w_scaling = conf->scaling;
        conf->h_scaling = conf->scaling;
    }

}