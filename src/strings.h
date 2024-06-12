#pragma once

typedef struct string {
    char *ptr;
    size_t size;
} string;

string *String(char *ptr);
int indexOf(string *str, string *seq);
void del(string *string);
string *cut(string *str, int from, int to);
string *merge(string *s1, string *s2);
string *concat(string *s1, string *s2);
string *split(string *str, int from, int to);
