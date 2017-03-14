#include "declaration.h"

const int MAX_LINES_CNT = 1e5;
const int MAX_QUERY_LEN = 1e5;
const int MAX_CHARS_CNT = 1e5;

#define COMMANDS_CNT 17
#define MAX_COMMAND_LEN 100

char *buffer, *buffer_st;
char command_title[MAX_COMMAND_LEN];
command_t commands[COMMANDS_CNT];
text_t text;

int read_line(char **str, FILE *f) {
    int len = 0;
    int c_int;
    for (c_int = fgetc(f); c_int != EOF && c_int != '\n'; ++len, c_int = fgetc(f)) {
        (*str)[len] = c_int;
    } 
    (*str)[len] = 0;
    if (c_int == EOF && len == 0)
        return -1;
    else
        return 0;
}

int next_string(char **buffer, char *destinatin) {
    while (**buffer == ' ' || **buffer == '\t')
        ++*buffer;
    if (**buffer == 0)
        return -1;
    while (**buffer != ' ' && **buffer != '\t' && **buffer != 0) {
        *destinatin = **buffer;
        ++destinatin;
        ++*buffer;
    }
    *destinatin = 0;
    return 0;
}

void skip_spaces(char **buffer) {
    while (**buffer == ' ' || **buffer == '\t')
        ++*buffer;
}

int is_empty(char *buffer) {
    while (*buffer != 0 && *buffer != '#') {
        if (*buffer != ' ' && *buffer != '\t')
            return 0;
        ++buffer;
    }
    return 1;
}

schar_t * new_c(text_t *text) {
    return text->c_en++;
}

void upd_c(schar_t *c) {
    if (c->prev != NULL)
        c->prev->next = c;
    if (c->next != NULL)
        c->next->prev = c;
}

schar_t * del_c(schar_t *c, text_t *text) {
    schar_t tmp = *c;
    upd_c(&tmp);
    --text->c_en;
    *c = *text->c_en;
    upd_c(c);
    if (tmp.prev != NULL)
        tmp.prev->next = tmp.next;
    if (tmp.next != NULL)
        tmp.next->prev = tmp.prev;
    return tmp.prev;
}

schar_t * add_c(schar_t *pos, text_t *text, char letter) {
    schar_t * c = &*new_c(text);
    c->prev = &*pos;
    c->next = &*pos->next;
    if (c->next != NULL)
        c->next->prev = &*c;
    pos->next = &*c;
    c->c = letter;
    return &*c;
}

line_t * new_l(text_t *text) {
    return text->lines_en++;
}

void upd_l(line_t *l) {
    if (l->prev != NULL)
        l->prev->next = l;
    if (l->next != NULL)
        l->next->prev = l;
}

line_t * del_l(line_t *l, text_t *text) {
    line_t tmp = *l;
    upd_l(&tmp);
    --text->lines_en;
    *l = *text->lines_en;
    upd_l(l);
    if (tmp.prev != NULL)
        tmp.prev->next = tmp.next;
    if (tmp.next != NULL)
        tmp.next->prev = tmp.prev;
    return tmp.prev;
}

line_t * add_l(line_t *pos, text_t *text) {
    line_t * l = &*new_l(text);
    l->prev = &*pos;
    l->next = &*pos->next;
    if (l->next != NULL)
        l->next->prev = &*l;
    pos->next = &*l;
    return &*l;
}

void error_wrong_command() {
    printf("editor: error, wrong command\n");
}

void listener() {
    while (1) {
        int flag = 0;
        int i;

        printf("editor: ");
        buffer = &*buffer_st;
        read_line(&buffer, stdin);
        if (is_empty(buffer))
            continue;
        if (next_string(&buffer, command_title) == -1) {
            error_wrong_command();
            continue;
        }
        for (i = 0; i < COMMANDS_CNT; ++i) {
            if (!strcmp(command_title, commands[i].title)) {
                commands[i].function(&text, buffer);
                flag = 1;
                break;
            }
        }
        if (flag == 1)
            continue;
        strcat(command_title, " ");
        if (next_string(&buffer, command_title + strlen(command_title)) == -1) {
            error_wrong_command();
            continue;
        }
        for (i = 0; i < COMMANDS_CNT; ++i) {
            if (!strcmp(command_title, commands[i].title)) {
                commands[i].function(&text, buffer);
                flag = 1;
                break;
            }
        }
        if (flag == 1)
            continue;
        error_wrong_command();
    }
}

void init() {
    setlocale(LC_ALL, "ru_RU.utf8");
    text.lines_cnt = 0;
    text.lines = (line_t*)malloc(sizeof(line_t) * MAX_LINES_CNT);
    text.c = (schar_t*)malloc(sizeof(schar_t) * MAX_CHARS_CNT);
    text.lines_en = &*text.lines;
    text.c_en = &*text.c;
    text.tab_width = 8;
    text.numbers_mode = 1;
    text.wrap_mode = 1;
    text.is_saved = 1;
    buffer = (char*)malloc(sizeof(char) * MAX_QUERY_LEN);
    buffer_st = &*buffer;
    commands[0].title = "exit";
    commands[0].function = &__exit;
    commands[1].title = "set tabwidth";
    commands[1].function = &set_tabwidth;
    commands[2].title = "set numbers";
    commands[2].function = &set_numbers;
    commands[3].title = "set wrap";
    commands[3].function = &set_wrap;
    commands[4].title = "print pages";
    commands[4].function = &print_pages;
    commands[5].title = "read";
    commands[5].function = &__read;
    commands[6].title = "open";
    commands[6].function = &__open;
    commands[7].title = "print range";
    commands[7].function = &print_range;
    commands[8].title = "edit string";
    commands[8].function = &edit_string;
    commands[9].title = "insert symbol";
    commands[9].function = &insert_symbol;
    commands[10].title = "delete range";
    commands[10].function = &delete_range;
    commands[11].title = "set name";
    commands[11].function = &set_name;
    commands[12].title = "write";
    commands[12].function = &__write;
    commands[13].title = "help";
    commands[13].function = &__help;
    commands[14].title = "delete braces";
    commands[14].function = &delete_braces;
    commands[15].title = "insert after";
    commands[15].function = &insert_after;
    commands[16].title = "replace substring";
    commands[16].function = &replace_substring;
}

int main(int argc, char *argv[]) {
    init();
    if (argc > 1) {
        strcpy(buffer, argv[1]);
        __open(&text, buffer);
    }
    listener();
    return 0;
}