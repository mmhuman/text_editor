#include "declaration.h"

#define small_prime 269 
#define hash_mod 1000000007

void error_exit_saving() {
    printf("editor: text isn't saved, if you want quit anyway print 'exit force'\n");
}

void __exit(text_t *text, char *buffer) {
    char force[256];
    next_string(&buffer, force);
    if (text->is_saved || !strcmp(force, "force")) {
        printf("editor: Goodbye!)\n");
        exit(0);
    } else {
        error_exit_saving();
    }
}

void error_wrong_tabwidth() {
    printf("editor: error, not valid tabwidth (valid: 0 ... 1000)\n");
}

int next_uint(char **buffer) {
    int x = 0;
    while (**buffer == ' ' || **buffer == '\t')
        ++*buffer;
    if (**buffer == 0)
        return -1;
    while (**buffer != 0 && **buffer != ' ' && **buffer != '\t') {
        if (**buffer < '0' || **buffer > '9') {
            return -1;
        }
        x = 10 * x + (**buffer - '0');
        ++*buffer;
    }
    return x;
}

int next_char(char **buffer) {
    while (**buffer == ' ' || **buffer == '\t')
        ++*buffer;
    if (**buffer == 0)
        return -1;
    return **buffer;
}

void set_tabwidth(text_t *text, char *buffer) {
    int x = 0;
    if ((x = next_uint(&buffer)) != -1 && x <= 1000) {
        text->tab_width = x;
    } else {
        error_wrong_tabwidth();
    }
}

void error_set_mode() {
    printf("editor: error, not valid mode (valid: yes, no)\n");
}

void set_numbers(text_t *text, char *buffer) {
    char mode[256];
    next_string(&buffer, mode);
    if (!strcmp(mode, "yes"))
        text->numbers_mode = 1;
    else
    if (!strcmp(mode, "no"))
        text->numbers_mode = 0;
    else
        error_set_mode();
}

void set_wrap(text_t *text, char *buffer) {
    char mode[256];
    next_string(&buffer, mode);
    if (!strcmp(mode, "yes"))
        text->wrap_mode = 1;
    else
    if (!strcmp(mode, "no"))
        text->wrap_mode = 0;
    else
        error_set_mode();
}

void print_range(text_t *text, char *buffer) {
    int st = next_uint(&buffer);
    int en = next_uint(&buffer);
    struct line *l = &text->st;
    int i, j;

    if (en == -1 || en > text->lines_cnt)
        en = text->lines_cnt;
    if (st < 1)
        st = 1;
    if (st > text->lines_cnt)
        st = text->lines_cnt;

    for (i = 0; i < st; ++i, l = &*l->next) {}

    for (i = st; i <= en; ++i, l = &*l->next) {
        schar_t *c = &l->st;
        if (l->len > 0)
            c = &*c->next;
        for (j = 0; j < l->len; ++j, c = &*c->next) {
            printf("%c", c->c);
        }
        printf("\n");
    }
}

void print_pages(text_t *text, char *buffer) {
    int i, j;
    line_t *l = &text->st;

    if (text->lines_cnt > 0)
        l = &*l->next;
    for (i = 0; i < text->lines_cnt; ++i, l = &*l->next) {
        schar_t *c = &l->st;
        if (l->len > 0)
            c = &*c->next;
        for (j = 0; j < l->len; ++j, c = &*c->next) {
            printf("%c", c->c);
        }
        printf("\n");
    }
}

void error_reading_file() {
    printf("editor: error, can't read file\n");
}

void __open(text_t *text, char *buffer) {
    char filename[1024];
    FILE *f;
    int i;
    line_t *l = &text->st;

    next_string(&buffer, filename);
    if ((f = fopen(filename, "r")) == NULL) {
        error_reading_file();
        return;
    }
    strcpy(text->save_to, filename);

    text->lines_en = &*text->lines;
    text->c_en = &*text->c;
    text->lines_cnt = 0;

    text->st.next = NULL;
    while (read_line(&buffer, f) != -1) {
        schar_t *c;

        ++text->lines_cnt;
        l = add_l(l, text);
        l->st.next = NULL;
        l->len = 0;
        c = &l->st;
        for (i = 0; buffer[i] != 0; ++i) {
            ++l->len;
            c = add_c(c, text, buffer[i]);
        }
    }
}

void __read(text_t *text, char *buffer) {
    char filename[1024];
    strcpy(filename, text->save_to);
    __open(text, buffer);
    strcpy(text->save_to, filename);
}

void error_edit_string_lines_cnt(int l_pos, int cnt) {
    printf("editor: error, trying to edit line #%d, but there are only %d lines in text\n", l_pos, cnt);
}

void error_edit_string_line_len(int l_pos, int c_pos, int len) {
    printf("editor: error, trying to edit line #%d, character #%d, but there are only %d characters in this line\n", l_pos, c_pos, len);
}

schar_t * finding_c(text_t *text, char **buffer, line_t **ret_l) {
    int l_pos = next_uint(&*buffer);
    int c_pos = next_uint(&*buffer);
    line_t *l;
    schar_t *c;
    int i;

    if (l_pos <= 0 || l_pos > text->lines_cnt) {
        error_edit_string_lines_cnt(l_pos, text->lines_cnt);
        return NULL;
    }
    l = &text->st;
    for (i = 1; i <= l_pos; ++i, l = &*l->next) {}
    if (c_pos <= 0 || c_pos > l->len) {
        error_edit_string_line_len(l_pos, c_pos, l->len);
        return NULL;
    }
    c = &l->st;
    for (i = 1; i <= c_pos; ++i, c = &*c->next) {}

    *ret_l = &*l;
    return c;
}

void edit_string(text_t *text, char *buffer) {
    line_t *l;
    schar_t *c = finding_c(text, &buffer, &l);
    if (c != NULL) {
        text->is_saved = 0;
        c->c = next_char(&buffer);
    }
}

void insert_symbol(text_t *text, char *buffer) {
    line_t *l;
    schar_t *c = finding_c(text, &buffer, &l);
    if (c != NULL) {
        text->is_saved = 0;
        add_c(c->prev, text, next_char(&buffer));
        ++l->len;
    }
}

void delete_range(text_t *text, char *buffer) {
    int st = next_uint(&buffer);
    int en = next_uint(&buffer);
    line_t *l;
    int i, j;

    text->is_saved = 0;
    if (en == -1 || en > text->lines_cnt)
        en = text->lines_cnt;
    if (st < 1)
        st = 1;
    if (st > text->lines_cnt)
        st = text->lines_cnt;
    text->lines_cnt -= en - st + 1;
    l = &text->st;
    for (i = 0; i < st; ++i, l = &*l->next) {}

    for (i = st; i <= en; ++i) {
        schar_t *c = &l->st;
        for (j = 0; j < l->len; ++j) {
            c = del_c(c, text)->next;
        }
        l = del_l(l, text)->next;
    }
}

void set_name(text_t *text, char *buffer) {
    char filename[1024];
    filename[0] = 0;
    next_string(&buffer, filename);
    strcpy(text->save_to, filename);
}

void error_write_save_to() {
    printf("editor: error, there is no destination for saving\n");
}

void __write(text_t *text, char *buffer) {
    char filename[1024];
    FILE *f;
    int i, j;
    line_t *l = &text->st;

    filename[0] = 0;
    next_string(&buffer, filename);
    if (filename[0] == 0) {
        if (text->save_to[0] == 0) {
            error_write_save_to();
        } else {
             strcpy(filename, text->save_to);
        }
    }
    if ((f = fopen(filename, "w")) == NULL) {
        error_reading_file();
        return;
    }
    text->is_saved = 1;

    if (text->lines_cnt > 0)
        l = &*l->next;
    for (i = 0; i < text->lines_cnt; ++i, l = &*l->next) {
        schar_t *c = &l->st;
        if (l->len > 0)
            c = &*c->next;
        for (j = 0; j < l->len; ++j, c = &*c->next) {
            fprintf(f, "%c", c->c);
        }
        fprintf(f, "\n");
    }
    fclose(f);
}

void __help(text_t *text, char *buffer) {
    printf("editor: read documentation.pdf\n");
}

void delete_braces(text_t *text, char *buffer) {
    int st = next_uint(&buffer);
    int en = next_uint(&buffer);
    line_t *l, *next_l;
    int open_braces_cnt = 0;
    schar_t *last_c;
    int i, j;

    text->is_saved = 0;
    if (en == -1 || en > text->lines_cnt)
        en = text->lines_cnt;
    if (st < 1)
        st = 1;
    if (st > text->lines_cnt)
        st = text->lines_cnt;

    l = &text->st;
    for (i = 0; i < st; ++i, l = &*l->next) {}

    for (i = st; i <= en; ++i, l = next_l) {
        schar_t *c = &l->st, *next_c;
        int len = l->len;

        next_l = l->next;
        if (l->len > 0)
            c = &*c->next;
        if (open_braces_cnt > 0) {
            if (l->len > 0) {
                last_c->next = l->st.next;
                l->st.next->prev = last_c;
            }
            --text->lines_cnt;
            next_l = del_l(l, text)->next;
        }
        for (j = 1; j <= len; ++j, c = next_c) {
            next_c = c->next;
            if (c->c == '{')
                ++open_braces_cnt;
            if (open_braces_cnt == 0)
                last_c = &*c;
            if (c->c == '}')
                --open_braces_cnt;
            if (open_braces_cnt > 0 || c->c == '}') {
                last_c = del_c(c, text);
                next_c = last_c->next;
            }
        }
    }


    l = &text->st;
    if (text->lines_cnt > 0)
        l = &*l->next;
    for (i = 0; i < text->lines_cnt; ++i, l = &*l->next) {
        schar_t *c = &l->st;
        for (l->len = -1; c != NULL; c = c->next, ++l->len) {}
    }

}

void error_insert_after_wrong_pos() {
    printf("editor: error, didn't succeed to read a position\n");
}

void error_insert_after_wrong_string_format() {
    printf("editor: error, didn't succeed to read a string/strings\n");
}


void insert_after(text_t *text, char *buffer) {
    int st = text->lines_cnt;
    char *end_st = malloc(sizeof(char) * 5);
    int i, j;
    line_t *l = &text->st;

    skip_spaces(&buffer);
    if (*buffer >= '0' && *buffer <= '9')
        st = next_uint(&buffer);
    if (st == -1) {
        error_insert_after_wrong_pos();
        return;
    }
    skip_spaces(&buffer);
    if (*buffer != '"') {
        error_insert_after_wrong_string_format();
        return;
    }
    if (*(buffer + 1) != '"' || *(buffer + 2) != '"') {
        char c = 0;
        for (i = 0; buffer[i] != 0; ++i) {
            if (buffer[i] != ' ' && buffer[i] != '\t')
                c = buffer[i];
        }
        if (c != '"') {
            error_insert_after_wrong_string_format();
            return;
        }
        end_st = "\"";
        buffer += 1;
    } else {
        end_st = "\"\"\"";
        buffer += 3;
    }
    for (i = 0; i < st; ++i, l = l->next) {}
    while (1) {
        schar_t *c = &l->st;
        l = add_l(l, text);
        l->len = 0;
        ++text->lines_cnt;
        for (i = 0; buffer[i] != 0; ++i) {
            int is_end = 1;
            for (j = 0; end_st[j] != 0; ++j) {
                if (buffer[i + j] != end_st[j]) {
                    is_end = 0;
                    break;
                }
            }
            if (is_end == 1 && (i == 0 || buffer[i - 1] != '\\'))
                return;
            c = add_c(c, text, buffer[i]);
            ++l->len;
        }
        read_line(&buffer, stdin);
    }

}

void error_replace_substring_wrong_pos() {
    printf("editor: error, didn't succeed to read a position\n");
}

void error_replace_substring_wrong_string_format() {
    printf("editor: error, didn't succeed to read a string\n");
}



void replace_substring(text_t *text, char *buffer) {
    int st = 1;
    int en = text->lines_cnt;
    int st1, en1;
    int to_begin = 0, to_end = 0;
    int st2, en2;
    int64_t pow_len, hash, hash_to_find = 0;
    int hash_len;
    int i, j, k;
    line_t *l = &text->st;

    while (*buffer == ' ' || *buffer == '\t')
        ++buffer;
    if (*buffer >= '0' && *buffer <= '9') {
        st = next_uint(&buffer);
        en = next_uint(&buffer);
        if (st == -1 || en == -1) {
            error_replace_substring_wrong_pos();
            return;
        }
    }

    st1 = 0;
    skip_spaces(&buffer);
    if (*buffer == 0) {
        read_line(&buffer, stdin);
        skip_spaces(&buffer);
    }
    en1 = st1;
    if (buffer[st1] == '^') {
        to_begin = 1;
    } else
    if (buffer[st1] == '$') {
        to_end = 1;
    } else {
        if (buffer[st1] != '"') {
            error_replace_substring_wrong_string_format();
            return;
        }
        ++st1;
        en1 = st1;
        while (buffer[en1] != 0 && (buffer[en1] != '"' || buffer[en1 - 1] == '\\'))
            ++en1;
        if (buffer[en1] == 0) {
            error_replace_substring_wrong_string_format();
            return;
        }
        buffer[en1] = 0;
    }



    st2 = en1 + 1;
    while (buffer[st2] == ' ' || buffer[st2] == '\t')
        ++st2;
    if (buffer[st2] == 0) {
        char *tmp = buffer + st2;
        read_line(&tmp, stdin);
        while (buffer[st2] == ' ' || buffer[st2] == '\t')
            ++st2;
    }
    if (buffer[st2] != '"') {
        error_replace_substring_wrong_string_format();
        return;
    }
    ++st2;
    en2 = st2;
    while (buffer[en2] != 0 && (buffer[en2] != '"' || buffer[en2 - 1] == '\\'))
        ++en2;
    if (buffer[en2] == 0) {
        error_replace_substring_wrong_string_format();
        return;
    }
    buffer[en2] = 0;


    pow_len = 1;

    for (i = st1; i < en1; ++i) {
        pow_len *= small_prime;
        pow_len %= hash_mod;
        hash_to_find *= small_prime;
        hash_to_find += buffer[i];
        hash_to_find %= hash_mod;
    }

    for (i = 0; i < st; ++i, l = l->next) {}
    for (i = st; i <= en; ++i, l = l->next) {
        int len = l->len;
        schar_t *begin_c = &l->st;
        schar_t *c = &l->st;

        if (to_begin) {
            for (j = st2; j < en2; ++j)
                c = add_c(c, text, buffer[j]);
            l->len += en2 - st2;
            continue;
        } 
        if (to_end) {
            for (j = 0; j < l->len; ++j, c = c->next) {}
            for (j = st2; j < en2; ++j)
                c = add_c(c, text, buffer[j]);
            l->len += en2 - st2;
            continue;
        }
        hash = 0;
        hash_len = 0;

        begin_c->c = 0;
        c = c->next;
        for (j = 1; j <= len; ++j, c = c->next) {
            hash *= small_prime;
            hash += c->c;
            hash %= hash_mod;
            ++hash_len;
            if (hash_len == en1 - st1 + 1) {
                begin_c = begin_c->next;
                --hash_len;
/*                printf("%lld\n", pow_len * begin_c->c);
                printf("%lld\n", hash - c->c); */
                hash -= pow_len * begin_c->c;
                hash = (hash % hash_mod + hash_mod) % hash_mod;
            }
/*            printf("%c, %c, %lld\n", begin_c->c, c->c, hash);
*/
            if (hash_len == en1 - st1 && hash == hash_to_find) {
                int flag = 1;
                schar_t *it = begin_c->next;

                for (k = st1; k < en1; ++k, it = it->next) {
                    if (it->c != buffer[k]) {
                        flag = 0;
                        break;
                    }
                }
                if (!flag)
                    continue;
                hash_len = 0;
                hash = 0;
                l->len -= en1 - st1;

                it = begin_c->next;
                for (k = st1; k < en1; ++k, it = it->next) {
                    it = del_c(it, text);
                }
                it = it->prev;
                l->len += en2 - st2;
                for (k = st2; k < en2; ++k) {
                    it = add_c(it, text, buffer[k]);
                }
                begin_c = it;
                c = it;
            } 
        }

    }

}