#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER 10000

typedef struct {
    int e; //- задать образцы "" для поиска
    int i; //+ игнорирует регистр
    int v; //+ только строки без совпадений
    int c; //+ количество совпадающих строк
    int l; //+ вывод только имя файлов
    int n; //+ вывод с номером строки в ф.
    int h; //+ вывод без имени файлов
    int s; //+ без мсдж о "ноу экзист"
    int o; //+ вывод только совпадающей части
    int f; //- паттерны из файла
    int many_files; //+ если файлов несколько
    char str[BUFFER];
    char str_o[BUFFER]; // для совп части
    char filename[BUFFER]; // название файла для -f
    char patterns[BUFFER]; //паттерны
} flags;

void parser (int argc,char *argv[], flags *fl);
int f_flag (flags *fl);
void output (char *argv[], flags *fl);