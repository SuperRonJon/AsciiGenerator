#pragma once
#define VERSION "1.3"

typedef struct config {
    char* filename;
    bool invert;
    double w_scaling;
    double h_scaling;
    double scaling;
} config;

config default_config();
void print_version();
void print_help();
void set_config(config* conf, int argc, char** argv);