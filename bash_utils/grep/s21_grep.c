#include "s21_grep_funcs.h"

int main(int argc, char **argv) {
  if (argc > 2) {
    const char *short_options = "eivclnhsfo";
    dflag flag = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    char *patterns = NULL;
    int optind = get_flags(short_options, argc, argv, &flag, &patterns);
    files_controller(optind, argc, argv, flag, &patterns);
    free(patterns);
  } else {
    fprintf(stderr, "s21_grep: not enough arguments\n");
  }
  return 0;
}
