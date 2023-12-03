#include "s21_grep.h"

int main(int argc, char *argv[]) {
  grep(argc, argv);
  return 0;
}

void grep(int argc, char **argv) {
  option flagPosition = {0};
  logics Logics = {0};
  regex_t reg = {0};
  regmatch_t reg_match = {0};
  int cflag = REG_EXTENDED;
  if (!take_flags(&flagPosition, &Logics, argc, argv)) {
    i_flag_checker(&cflag, &flagPosition);
    regcomp(&reg, Logics.str_template, cflag);
    if (argc - optind > 1) Logics.many_files = 1;
    for (int i = optind; i < argc; i++) {
      sprintf(Logics.file_name, "%s", argv[i]);
      fileReader(&flagPosition, &Logics, &reg, &reg_match);
    }
    regfree(&reg);
  } else
    fprintf(stderr,
            "usage: grep [-abcDEFGHhIiJLlmnOoqRSsUVvwxZ] [-A num] [-B num] "
            "[-C[num]]\n[-e pattern] [-f file] [--binary-files=value] "
            "[--color=when]\n[--context[=num]] [--directories=action] "
            "[--label] [--line-buffered]\n[--null] [pattern] [file ...]");
}

void f_flag_w(option *flagPosition, logics *Logics) {
  if (flagPosition->f && !flagPosition->e) {
    FILE *fp = fopen(Logics->file_name, "r");
    if (fp != NULL) {
      int i = 0;
      char ch = 0;
      char template[BUFFER] = {0};
      while ((ch = fgetc(fp)) != EOF) {
        if (ch == '\n') {
          ch = '|';
        }
        template[i] = ch;
        i++;
      }
      strcat(Logics->str_template, template);
      fclose(fp);
    } else {
      print_error_fileReader(flagPosition, Logics);
    }
  }
}

void i_flag_checker(int *cflag, option *flagPosition) {
  if (flagPosition->i) *cflag = REG_ICASE | REG_EXTENDED;
}

void v_flag_execute(int *RegexecStatus, option *flagPosition) {
  if (flagPosition->v && !flagPosition->o) {
    *RegexecStatus = !(*RegexecStatus);
  }
}

void c_flag_print(option *flagPosition, logics *Logics) {
  if (Logics->many_files && flagPosition->c && !flagPosition->l) {
    printf("%s:%d\n", Logics->file_name, Logics->c_count);
  }
  if (!Logics->many_files && flagPosition->c && !flagPosition->l) {
    printf("%d\n", Logics->c_count);
  }
}

void l_flag_subfuntion(option *flagPosition, logics *Logics) {
  if (flagPosition->l && !flagPosition->c && !Logics->print_or_not &&
      Logics->many_files) {
    printf("%s\n", Logics->file_name);
    Logics->print_or_not++;
  }

  if (flagPosition->l && !flagPosition->c && !Logics->print_or_not &&
      !Logics->many_files) {
    printf("%s\n", Logics->file_name);
    Logics->print_or_not++;
  }
}

void n_flag_print(option *flagPosition, logics *Logics) {
  if (flagPosition->n && !flagPosition->c && !flagPosition->l) {
    printf("%d:%s", Logics->n_count, Logics->str);
    if (Logics->str[strlen(Logics->str) - 1] != '\n') printf("\n");
  }
}

void o_flag_print(option *flagPosition, logics *Logics) {
  if (flagPosition->o && !flagPosition->c && !flagPosition->l) {
    int len = 0;
    char p[BUFFER] = {0};
    strcat(p, Logics->str);
    char *str = p;
    len = strlen(Logics->str_template);
    while (*str) {
      char *temp;
      if (flagPosition->i)
        temp = strcasestr(str, Logics->str_template);
      else
        temp = strstr(str, Logics->str_template);
      if (temp != NULL) {
        printf("%s\n", Logics->str_template);
        str = temp + len;
      } else {
        break;
      }
    }
  }
}

void print_without_flags(option *flagPosition, logics *Logics) {
  if (Logics->many_files && !flagPosition->c && !flagPosition->l &&
      !flagPosition->n && !flagPosition->h) {
    printf("%s:", Logics->file_name);
  }
  if (Logics->many_files && !flagPosition->c && !flagPosition->l &&
      !flagPosition->n && !flagPosition->h && !flagPosition->o) {
    printf("%s", Logics->str);
    if (Logics->str[strlen(Logics->str) - 1] != '\n') printf("\n");
  }
  if (Logics->many_files && !flagPosition->c && !flagPosition->l &&
      !flagPosition->n && flagPosition->h && !flagPosition->o) {
    printf("%s", Logics->str);
    if (Logics->str[strlen(Logics->str) - 1] != '\n') printf("\n");
  }
  if (!Logics->many_files && !flagPosition->c && !flagPosition->l &&
      !flagPosition->n && !flagPosition->o) {
    printf("%s", Logics->str);
  }
}

void reset_logics_flags(logics *Logics) {
  Logics->c_count = 0;
  Logics->print_or_not = 0;
  Logics->n_count = 0;
}

void fileReader(option *flagPosition, logics *Logics, regex_t *reg,
                regmatch_t *reg_match) {
  FILE *fp = fopen(Logics->file_name, "r");
  if (fp != NULL) {
    while ((fgets(Logics->str, BUFFER, fp)) != NULL) {
      int RegexecStatus = 0;
      Logics->n_count++;

      RegexecStatus = regexec(reg, Logics->str, 1, reg_match, 0);

      v_flag_execute(&RegexecStatus, flagPosition);
      if (!RegexecStatus) {
        Logics->c_count++;

        l_flag_subfuntion(flagPosition, Logics);

        print_without_flags(flagPosition, Logics);
        o_flag_print(flagPosition, Logics);

        n_flag_print(flagPosition, Logics);
      }
    }

    c_flag_print(flagPosition, Logics);
    reset_logics_flags(Logics);
    fclose(fp);
  } else {
    print_error_fileReader(flagPosition, Logics);
  }
}

int take_flags(option *flagPosition, logics *Logics, int argc, char **argv) {
  if (argc > 2) {
    char opt = 0;
    while ((opt = getopt(argc, argv, "e:ivclnhsf:o")) != -1) {
      switch (opt) {
        case 'e':
          flagPosition->e = 1;
          strcat(Logics->str_template, optarg);
          strcat(Logics->str_template, "|\0");
          break;
        case 'i':
          flagPosition->i = 1;
          break;
        case 'v':
          flagPosition->v = 1;
          break;
        case 'c':
          flagPosition->c = 1;
          break;
        case 'l':
          flagPosition->l = 1;
          break;
        case 'n':
          flagPosition->n = 1;
          break;
        case 'h':
          flagPosition->h = 1;
          break;
        case 's':
          flagPosition->s = 1;
          break;
        case 'f':
          flagPosition->f = 1;
          strcat(Logics->file_name, optarg);
          f_flag_w(flagPosition, Logics);
          break;
        case 'o':
          flagPosition->o = 1;
          break;
        default:
          print_error_grep(flagPosition, Logics);
          break;
      }
    }
    if (!flagPosition->e && !flagPosition->f) {
      if (argc > optind) {
        strcat(Logics->str_template, argv[optind]);
      }
      optind++;
    }
    if (flagPosition->e && !flagPosition->f) {
      Logics->str_template[strlen(Logics->str_template) - 1] = '\0';
    }
  } else {
    Logics->correct_work = 1;
  }
  return Logics->correct_work;
}

void print_error_grep(option *flagPosition, logics *Logics) {
  Logics->correct_work = 1;
  if (!flagPosition->s) fprintf(stderr, "usage: grep [-benstuv] [file ...]");
}

void print_error_fileReader(option *flagPosition, logics *Logics) {
  Logics->correct_work = 1;
  if (!flagPosition->s) printf("No such file in directory\n");
}