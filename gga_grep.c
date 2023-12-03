#include "gga_grep.h"

int main (int argc, char *argv[]) {
flags fl={0};
parser (argc,argv,&fl);
if ((fl.f)!=-1) {
    while (optind < argc) {
        output (argv, &fl);
        optind++; // первый арг - не флаг
    }
}
    return 0;
}

void parser (int argc,char *argv[], flags *fl) {
 int opt =0;
 while ((opt=getopt_long(argc,argv,"e:ivclnhsf:o",NULL,0))!=-1) {
    switch (opt)
    {
    case 'e':
        fl->e=1;
        strcat(fl->patterns,optarg); // текущий поттерн после "е"
        strcat(fl->patterns,"|");
        break;
    case 'i':
        fl->i=1;
        break;
    case 'v':
        fl->v=1;
        break;
    case 'c':
        fl->c=1;
        break;
    case 'l':
        fl->l=1;
        break;
    case 'n':
        fl->n=1;
        break;
    case 'h':
        fl->h=1;
        break;
    case 's':
        fl->s=1;
        break;
    case 'o':
        fl->o=1;
        break;
    case 'f':
        fl->f=1;
        strcat(fl->filename,optarg);
        break;
    default:
        break;
    }
 }

 if (!fl->e && !fl->f) { // считываем паттерн и го след
    if (argc > optind) {
        strcat(fl->patterns,argv[optind]);
    } 
    optind++;
 }

    if (fl->f) {
        fl->f = f_flag(fl);
    }


 if (fl->e || fl->f) {
    fl->patterns[strlen(fl->patterns)-1]='\0';
 }
 if (argc-optind>1) fl->many_files=1;
 //printf("patterns: %s\n", fl->patterns);
 //printf("e=%d i=%d v=%d c=%d l=%d n=%d h=%d s=%d o=%d f=%d\n", 
 //fl->e,fl->i,fl->v,fl->c,fl->l,fl->n,fl->h,fl->s,fl->o,fl->f);
}

void output (char *argv[], flags *fl) {
    FILE *file;
    regex_t reg;
    regmatch_t start;
    int count = 0;
    int str_num = 0;
    int match =0;
    int flag_i = REG_EXTENDED;
    if (fl->i) {
        flag_i = REG_EXTENDED | REG_ICASE;
    }
    regcomp(&reg, fl->patterns, flag_i); // ловит регулярные выражения

    file=fopen(argv[optind], "r");
    if (file != NULL) {
        while (fgets(fl->str, BUFFER, file) != NULL) {
            if (fl->o && fl->v) break; 
            str_num++;
            if (fl->o) strcpy(fl->str_o,fl->str);
            match = regexec(&reg, fl->str, 1,&start,0); // сравнивает рег выр со строкой
            
            if (fl->v) match=!match;
            if (!match) count++;
            //if (fl->many_files && !match && !fl->h && !fl->c && !fl->l && !fl->o) printf("%s:",argv[optind]);
            if (fl->many_files && !match && !fl->h && !fl->c && !fl->l) {
                printf("%s:",argv[optind]);
            }

            if (fl->n && !match && !fl->c && !fl->l) printf("%d:",str_num);



            if (!match && !fl->c && !fl->l) {
                if (!fl->o) {
                    printf("%s", fl->str); // собственно вывод строчки
                    if (fl->str[strlen(fl->str)-1] != '\n') printf ("\n"); // и сброс строки
                }   
                else {
                        
                    char *ptr = fl->str_o;
                    while (regexec(&reg,ptr,1,&start,0)==0) {
                        printf("%.*s\n", (int)(start.rm_eo-start.rm_so), ptr+start.rm_so);
                        ptr+=start.rm_eo;
                    }
                }
            }
        }
        regfree(&reg);
    
        if (fl->c && !fl->l) {
            if (fl->many_files && !fl->h) printf("%s:%d\n",argv[optind],count);
            else printf("%d\n", count);
        }
        if (fl->l) printf("%s\n",argv[optind]);


        fclose(file);
    } else {
        regfree(&reg);
        if (!fl->s) fprintf(stderr,"grep: %s: Нет такого файла или каталога\n",argv[optind]);
    }
}

int f_flag (flags *fl) {
    FILE *file = fopen(fl->filename, "r");
    if (file != NULL) {
    while (fgets(fl->str, BUFFER, file) != NULL) {
        //printf("из файла: %s ",fl->patterns);

        if (((fl->str[strlen(fl->str)-1])=='\n') && (strlen(fl->str)!=1)) 
            fl->str[strlen(fl->str) - 1] = '\0';
        strcat(fl->patterns,fl->str);
        strcat(fl->patterns,"|");
    }
    //printf("\n");
    fclose(file);
    } else {
        fprintf(stderr,"grep: %s: Нет такого файла или каталога\n",fl->filename);
        return -1;
    }
    return 1;
}
