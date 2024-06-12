#pragma once

#include <sys/stat.h>

#include "strings.h"

#define FILE_BATCH_SIZE 0x10

#define IS_FILE(path)   S_ISREG(getFileInfo(path))
#define IS_DIR(path)    S_ISDIR(getFileInfo(path))

int getFileInfo(char *path);

typedef struct tree {
    string *name;
    int size;
    int elementsCount;
    struct tree *left;
    struct tree *right;
} tree_t;

tree_t *Tree();
void addNode(tree_t *node, string *name);
tree_t *getTree(char *path, int dType);
string **treeToList(tree_t *tree);

typedef struct file_content {
    char *data;
    size_t size;
} file_content_t;

file_content_t *readFile(char *path);
