#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "s21_cat_funcs.h"

int get_flags(const char* short_options, const struct option long_options[], int argc, char **argv, int* flag_b, int* flag_e, int* flag_n, int* flag_s, int* flag_t) {
    
    int option_index;
    int res;

    while ((res = getopt_long(argc, argv, short_options, long_options, &option_index)) != -1) {
        switch (res) {
            case 'b': {
                printf("It was flag --number-nonblank\n");
                *flag_b = 1;
                break;
            }
            case 'n': {
                *flag_n = 1;
                break;
            }
            case 's': {
                printf("It was flag squeeze-blank\n");
                *flag_s = 1;
                break;
            }
            case 'e': {
                printf("It was flag e\n");
                *flag_e = 1;
                break;
            }
            case 't': {
                printf("It was flag t\n");
                *flag_t = 1;
                break;
            }

            case '?': default: {
                printf("found unknown option");
                break;
            }
        }
    }
    return optind;
}

void flags_controller(int optind, int argc, char** argv, int flag_b, int flag_e, int flag_n, int flag_s, int flag_t) {
    while (optind < argc) {
        if (flag_b + flag_e + flag_n + flag_s + flag_t == 0) {
            output(argv[optind]);
        } else {
            if (flag_n) {
                flag_n_app(argv[optind]);
                output("buffer.txt");
            }
        }
        optind++;
    }
}

void flag_n_app(char *filename) {
    char *line_buf = NULL;
    size_t line_buf_size = 0;
    int line_count = 0;
    ssize_t line_size;
    FILE *fp = fopen(filename, "r");
    FILE *fbuf = fopen("buffer.txt", "w+");
    if (fp) {
        line_size = getline(&line_buf, &line_buf_size, fp);
        while (line_size >= 0) {
            line_count++;
            fprintf(fbuf, "%6d\t", line_count);
            ssize_t i = 0;
            while(i < line_size) {
                putc(line_buf[i++], fbuf);
            }
            line_size = getline(&line_buf, &line_buf_size, fp);
        }
        free(line_buf);
        line_buf = NULL;
        fclose(fp);
        fclose(fbuf);
    } else {
        fprintf(stderr, "Error opening file '%s'\n", filename);
    }
}

void output(char *filename) {
    char *line_buf = NULL;
    size_t line_buf_size = 0;
    int line_count = 0;
    ssize_t line_size; //ssize_t able to represent -1 for errors
    FILE *fp = fopen(filename, "r");
    if (fp) {
        line_size = getline(&line_buf, &line_buf_size, fp);
        while (line_size >= 0) {
            line_count++;
            ssize_t i = 0;
            while(i < line_size) {
                putc(line_buf[i++], stdout);
            }
            line_size = getline(&line_buf, &line_buf_size, fp);
        }
        free(line_buf);
        line_buf = NULL;
        fclose(fp);
    } else {
        fprintf(stderr, "Error opening file '%s'\n", filename);
    }
}