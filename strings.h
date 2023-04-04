#pragma once

typedef struct string {
    char *ptr;
    size_t size;

    struct string (*concat)(struct string *s1, struct string *s2);

    struct string (*split)(struct string *str, int from, int to);

    void (*delete)(struct string *str);
} string;
