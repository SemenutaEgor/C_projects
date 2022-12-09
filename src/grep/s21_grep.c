#include "s21_grep_funcs.h"

int main(int argc, char **argv) {
  if (argc > 2) {
    const char *short_options = "eivclnhsfo";

    dflag flag = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    char *patterns = NULL;
    //char *patterns = (char*)malloc(sizeof(char));
    //strcpy(patterns, "\0");

    int optind =
        get_flags(short_options, argc, argv, &flag, &patterns);

    /*while (optind < argc) {
      printf("%s\n", argv[optind]);
      optind++;
    }*/
    //printf("PATTERNS IN MAIN: %s\n", patterns);

    files_controller(optind, argc, argv, flag, patterns);

    free(patterns);
  } else {
    fprintf(stderr, "not enough arguments\n");
  }

  return 0;
}
