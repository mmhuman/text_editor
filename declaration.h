#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h> 
#include <locale.h>
#include <stdint.h>

struct schar {
    char c;
    struct schar *next, *prev;
};
typedef struct schar schar_t;

struct line {
    int len;
    schar_t st;
    struct line *next, *prev;
};
typedef struct line line_t;

struct text {
    int lines_cnt;
    line_t *lines, *lines_en;
    line_t st;
    schar_t *c, *c_en;
    int tab_width;
    int numbers_mode, wrap_mode;
    int is_saved;
    char save_to[1024];
};
typedef struct text text_t;

struct command {
    char *title;
    void (*function)(text_t *, char *);
};
typedef struct command command_t;

schar_t * new_c(text_t *text);

schar_t * del_c(schar_t *c, text_t *text);

schar_t * add_c(schar_t *pos, text_t *text, char letter);

line_t * new_l(text_t *text);

line_t * del_l(line_t *l, text_t *text);

line_t * add_l(line_t *pos, text_t *text);

int read_line(char **str, FILE *f);

int next_string(char **buffer, char *destinatin);

void skip_spaces(char **buffer);

void __exit(text_t *text, char *buffer);

void set_tabwidth(text_t *text, char *buffer);

void set_numbers(text_t *text, char *buffer);

void set_wrap(text_t *text, char *buffer);

void print_pages(text_t *text, char *buffer);

void __open(text_t *text, char *buffer);

void __read(text_t *text, char *buffer);

void print_range(text_t *text, char *buffer);

void edit_string(text_t *text, char *buffer);

void insert_symbol(text_t *text, char *buffer);

void delete_range(text_t *text, char *buffer);

void set_name(text_t *text, char *buffer);

void __write(text_t *text, char *buffer);

void __help(text_t *text, char *buffer);

void delete_braces(text_t *text, char *buffer);

void insert_after(text_t *text, char *buffer);

void replace_substring(text_t *text, char *buffer);