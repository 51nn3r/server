#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#include "files.h"
#include "strings.c"


typedef struct dirent dirent;

typedef struct tree {
    string *name;
    int size;
    int elementsCount;
    struct tree *left;
    struct tree *right;
} tree_t;

tree_t *Tree() {
    tree_t *tree = malloc(sizeof(tree_t));

    tree->name = NULL;
    tree->size = 0;
    tree->elementsCount = 0;
    tree->left = NULL;
    tree->right = NULL;

    return tree;
}

typedef struct batch {
    char *data;
    size_t size;
    struct batch *next;
} batch_t;

batch_t *Batch(size_t size) {
    batch_t *batch = malloc(sizeof(batch_t));

    batch->size = size;
    batch->data = malloc(size);
    batch->next = NULL;

    return batch;
}

void *alloc(int size) {
    void *ptr = malloc(size);
    memset(ptr, 0x00, size);
    return ptr;
}

void addNode(tree_t *node, string *name) {
    node->elementsCount += 1;
    node->size += name->size;

    if (node->name == NULL) {
        node->name = name;
    } else if (strcmp(node->name->ptr, name->ptr) > 0) {
        if (node->left == NULL) {
            node->left = Tree();
        }
        addNode(node->left, name);
    } else {
        if (node->right == NULL) {
            node->right = Tree();
        }
        addNode(node->right, name);
    }
}

tree_t *getTree(char *path, int dType) {
    DIR *dir = opendir(path);
    if (dir == NULL)
        return Tree();

    dirent *dirInfo;

    tree_t *tree = Tree();

    while ((dirInfo = readdir(dir)) != NULL) {
        if (dirInfo->d_type == dType) {
            addNode(tree, String(dirInfo->d_name));
        }
    }

    return tree;
}

void convertTree(string **files, tree_t *node, int index) {
    if (node->left) {
        convertTree(files, node->left, index);
        index += node->left->elementsCount;
    }

    files[index] = String(node->name->ptr);

    if (node->right)
        convertTree(files, node->right, ++index);
}

string **treeToList(tree_t *tree) {
    if (!tree || tree->elementsCount == 0) {
        return NULL;
    }
    string **files = malloc(sizeof(tree_t *) * tree->elementsCount);
    convertTree(files, tree, 0);
    return files;
}


batch_t *readFile(char *path) {
    FILE *fd = fopen(path, "r");
    if (!fd) {
        return NULL;
    }

    batch_t *firstBatch = Batch(FILE_BATCH_SIZE);
    batch_t *batch = firstBatch;

    long long counter = 0;
    while (fread(batch->data + (counter % FILE_BATCH_SIZE), 1, 1, fd) != 0) {
        counter++;

        if (counter % FILE_BATCH_SIZE == 0) {
            batch->next = Batch(FILE_BATCH_SIZE);
            batch = batch->next;
        }
    }
    batch->size = counter % FILE_BATCH_SIZE;

    fclose(fd);

    return firstBatch;
}
