#include "s21_grep_funcs.h"

static char *add_to_string(char *string, char *word) {
  size_t new_length = strlen(string) + strlen(word) + 2;  // \0 + |
  char *new_string = realloc(string, sizeof(char) * new_length);
  if (new_string) {
    if (strlen(new_string)) {
      strcat(new_string, "|");
      strcat(new_string, word);
    } else {
      strcat(new_string, word);
    }
  } else {
    fprintf(stderr, "Error with allocating memory for patterns\n");
    exit(0);
  }
  return new_string;
}

static char *cut_pattern(char *line_buf, ssize_t line_size) {
  char *new_line_buf = malloc(sizeof(char) * (line_size - 1));
  if (!new_line_buf) {
    return NULL;
  }
  int i;
  for (i = 0; i < line_size - 1; i++) {
    new_line_buf[i] = line_buf[i];
  }
  new_line_buf[i] = 0;
  free(line_buf);
  return new_line_buf;
}

static char *load_patterns(char *patterns, char *filename, dflag flag) {
  FILE *patfile;
  patfile = fopen(filename, "r");
  if (patfile) {
    char *line_buf = NULL;
    size_t line_buf_size = 0;
    ssize_t line_size = getline(&line_buf, &line_buf_size, patfile);
    while (line_size >= 0) {
      line_buf = cut_pattern(line_buf, line_size);
      if (!line_buf) {
        return NULL;
      }
      patterns = add_to_string(patterns, line_buf);
      line_size = getline(&line_buf, &line_buf_size, patfile);
    }
    free(line_buf);
    line_buf = NULL;
    fclose(patfile);
  } else if (!flag.s) {
    fprintf(stderr, "No such file or directory '%s'\n", filename);
  }
  return patterns;
}

int get_flags(const char *short_options, int argc, char **argv, dflag *flag,
              char *patterns, char *patfiles) {
  int res;
  while ((res = getopt_long(argc, argv, short_options, 0, 0)) != -1) {
    switch (res) {
      case 'e': {
        patterns = add_to_string(patterns, argv[optind]);
        break;
      }
      case 'i': {
        flag->i = 1;
        break;
      }
      case 'v': {
        flag->v = 1;
        break;
      }
      case 'c': {
        flag->c = 1;
        break;
      }
      case 'l': {
        flag->l = 1;
        break;
      }
      case 'n': {
        flag->n = 1;
        break;
      }
      case 'h': {
        flag->h = 1;
        break;
      }
      case 's': {
        flag->s = 1;
        break;
      }
      case 'f': {
        flag->f = 1;
        patterns = load_patterns(patterns, argv[optind], *flag);
        patfiles = add_to_string(patfiles, argv[optind]);
        break;
      }
      case 'o': {
        flag->o = 1;
        printf("flag o\n");
        break;
      }

      case '?':
      default: {
        printf("found unknown option");
        break;
      }
    }
  }
  /*for (size_t i = 0; i < strlen(patterns); i++) {
    putchar(patterns[i]);
  }*/
  return optind;
}

int compile(regex_t *regex, dflag flag, char *patterns) {
  int cflags = REG_EXTENDED;
  if (flag.i) {
    cflags = REG_EXTENDED | REG_ICASE;
  }
  return regcomp(regex, patterns, cflags);
}

int multifile_check(int argc, int optind) {
  int multifile = 0;
  while (optind++ < argc) {
    // printf("%s\n", argv[optind]);
    // optind++;
    multifile++;
  }
  return multifile;
}

void files_controller(int optind, int argc, char **argv, dflag flag,
                      char *patterns, char *patfiles) {
  FILE *src;
  regex_t regex;
  int result;
  printf("len = %ld\n", strlen(patterns));
  printf("patterns = %s\n", patterns);
  printf("argv[] = %s\n", argv[optind]);
  if (!strlen(patterns)) {
    add_to_string(patterns, argv[optind++]);
  }

  /*check for multyfile*/
  int multifile = multifile_check(argc, optind);
  // printf("there is %d files\n", multyfile);

  printf("patterns = %s\n", patterns);
  result = compile(&regex, flag, patterns);
  // printf("result = %d\n", result);
  if (result) {
    if (result == REG_ESPACE) {
      fprintf(stderr, "%s\n", strerror(ENOMEM));
    } else {
      fprintf(stderr, "Syntax error in the regular expression %s\n", patterns);
    }
    return;
  }

  while (optind < argc) {
    char *check_pattern = strstr(patterns, argv[optind]);
    char *check_patfile = strstr(patfiles, argv[optind]);
    // printf("check = %s\n", check);
    if (check_pattern || check_patfile) {
    } else {
      printf("now in %s\n", argv[optind]);
      printf("patfiles is %s\n", patfiles);
      printf("check_pattern is %s\n", check_pattern);
      printf("check_patfile is %s\n", check_patfile);
      src = fopen(argv[optind], "r");
      if (src) {
        flags_controller(src, flag, &regex, &result, argv[optind], multifile);
        fclose(src);
      } else if (!flag.s) {
        fprintf(stderr, "No such file or directory '%s'\n", argv[optind]);
      }
    }
    optind++;
  }
  regfree(&regex);
}

void flags_controller(FILE *src, dflag flag, regex_t *regex, int *result,
                      char *filename, int multifile) {
  int line_counter = 0, output_suppress = 0, file_match = 0, line = 1;
  char *line_buf = NULL;
  size_t line_buf_size = 0;
  ssize_t line_size = getline(&line_buf, &line_buf_size, src);
  dbuf buffer = {line_buf, line_size};
  *result = regexec(regex, buffer.data, 0, NULL, 0);
  while (line_size >= 0) {
    if (flag.v) {
      *result = !regexec(regex, buffer.data, 0, NULL, 0);
      if (!flag.c) {
        output(regex, result, buffer, filename, multifile, flag, line);
      }
      output_suppress = 1;
    }
    if (flag.c && !(*result)) {
      line_counter++;
      output_suppress = 1;
    }
    if (flag.l && !(*result)) {
      output_suppress = 1;
      file_match = 1;
    }
    if (!output_suppress) {
      output(regex, result, buffer, filename, multifile, flag, line);
    }
    line_size = getline(&line_buf, &line_buf_size, src);
    buffer.data = line_buf;
    buffer.size = line_size;
    *result = regexec(regex, buffer.data, 0, NULL, 0);
    line++;
  }
  if (flag.c) {
    output_c(line_counter, filename, multifile);
  }
  if (flag.l && file_match) {
    printf("%s\n", filename);
    file_match = 0;
  }
  free(line_buf);
  line_buf = NULL;
}

void output_c(int line_counter, char *filename, int multifile) {
  if (multifile > 1) {
    printf("%s:%d\n", filename, line_counter);
  } else {
    printf("%d\n", line_counter);
  }
}

void output(regex_t *regex, int *result, dbuf buffer, char *filename,
            int multifile, dflag flag, int line) {
  if (!(*result)) {
    if (multifile > 1 && !flag.h) {
      printf("%s:", filename);
    }
    if (flag.n) {
      printf("%d:", line);
    }
    printf("%s", buffer.data);
    /*if (multifile > 1) {
      printf("%s:%s", filename, buffer.data);
    } else {
      printf("%s", buffer.data);
    }*/
  } else if (*result != REG_NOMATCH) {
    size_t length = regerror(*result, regex, NULL, 0);
    print_regerror(*result, length, regex);
    return;
  }
}

void print_regerror(int errcode, size_t length, regex_t *compiled) {
  char buffer[length];
  regerror(errcode, compiled, buffer, length);
  fprintf(stderr, "Regex match failed: %s\n", buffer);
}
