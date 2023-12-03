#ifndef S21_GREP
#define S21_GREP
#define _GNU_SOURCE

#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <string.h>

#define BUFFER 10000

typedef struct {
  int e;
  int i;
  int v;
  int c;
  int l;
  int n;
  int h;
  int s;
  int f;
  int o;
} option;

typedef struct {
  int c_count;
  int n_count;
  int c_print;
  int many_files;
  int print_or_not;
  int correct_work;
  char str[BUFFER];
  char str_template[BUFFER];
  char file_name[BUFFER];
} logics;

int take_flags(option *flagPosition, logics *Logics, int argc, char **argv);
void grep(int argc, char **argv);
void fileReader(option *flagPosition, logics *Logics, regex_t *reg,
                regmatch_t *reg_match);
void print_error_grep(option *flagPosition, logics *Logics);
void print_error_fileReader(option *flagPosition, logics *Logics);
void i_flag_checker(int *cflag, option *flagPosition);

#endif