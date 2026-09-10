#ifndef ASCIICONFIG_H
#define ASCIICONFIG_H

#include <stdbool.h>

#define VERSION "1.6"

struct config {
    char *filename;
    char *character_set;
    bool invert;
    double w_scaling;
    double h_scaling;
    double scaling;
};

void default_config(struct config *conf);
void set_config(struct config *conf, int argc, char **argv);
void print_help(void);
void print_version(void);

#endif