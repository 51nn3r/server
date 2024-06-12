#include <stdlib.h>
#include <string.h>

#include "strings.h"


string *String(char *ptr) {
    size_t size = strlen(ptr);

    char *newPtr = malloc(size + 1);
    memcpy(newPtr, ptr, size + 1);

    string *str = malloc(sizeof(*str));
    str->ptr = newPtr;
    str->size = size;

    return str;
}

void del(string *string) {
    free(string->ptr);
    free(string);
}

string *concat(string *s1, string *s2) {
    size_t size = s1->size + s2->size;
    char *ptr = malloc(size + 1);

    memcpy(ptr, s1->ptr, s1->size);
    memcpy(ptr + s1->size, s2->ptr, s2->size + 1);

    return String(ptr);
}

string *merge(string *s1, string *s2) {
    size_t size = s1->size + s2->size;
    char *ptr = malloc(size + 1);

    memcpy(ptr, s1->ptr, s1->size);
    memcpy(ptr + s1->size, s2->ptr, s2->size + 1);

    free(s1->ptr);
    del(s2);

    s1->ptr = ptr;
    s1->size = size;

    return s1;
}

string *split(string *str, int from, int to) {
    size_t size = to - from;

    if (from < 0 || to < 0 || from > to || size > str->size)
        return NULL;

    char *ptr = malloc(size + 1);

    memcpy(ptr, str->ptr + from, size);
    ptr[size] = 0x00;

    free(str->ptr);

    str->ptr = ptr;
    str->size = size;


    return str;
}

int indexOf(string *str, string *seq) {
    for (size_t i = 0; i < str->size; i++) {
        if (str->size - i < seq->size) {
            return -1;
        }
        if (strcmp(split(String(str->ptr), i, i + seq->size - 1)->ptr, seq->ptr) == 0) {
            return i;
        }
    }
    return -1;
}
