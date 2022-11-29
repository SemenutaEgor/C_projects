#include "s21_grep_funcs.h"

int main(int argc, char **argv) {

  const char *short_options = "eivclnhsfo";

  dflag flag = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  char *patterns = malloc(sizeof(char));
  strcpy(patterns, "");

  int optind = get_flags(short_options, argc, argv, &flag, patterns);
  files_controller(optind, argc, argv, flag, patterns);
  
  /*while (optind < argc) {
    printf("%s\n", argv[optind]);
    optind++;
  }*/

  /*regex_t reegex;
  int value;
  value = regcomp(&reegex, "test", 0);
  value = regexec(&reegex, "test string", 0, NULL, 0);
  print_result(value);*/

  free(patterns);

  return 0;
}