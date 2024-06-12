#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

#include "files.h"
#include "strings.h"

typedef struct dirent dirent;


int getFileInfo(char *path) {
    struct stat info;

    stat(path, &info);
    int mode = (int)info.st_mode;

    return mode;
}

tree_t *Tree() {
    tree_t *tree = malloc(sizeof(*tree));

    tree->name = NULL;
    tree->size = 0;
    tree->elementsCount = 0;
    tree->left = NULL;
    tree->right = NULL;

    return tree;
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

file_content_t *readFile(char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = (char *)malloc(file_size);
    if (buffer == NULL) {
        perror("Error allocating memory");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    size_t bytes_read = fread(buffer, 1, file_size, file);
    if (bytes_read != file_size) {
        perror("Error reading file");
        fclose(file);
        free(buffer);
        exit(EXIT_FAILURE);
    }

    fclose(file);

    file_content_t *fileContent = malloc(sizeof(*fileContent));
    fileContent->data = buffer;
    fileContent->size = file_size;

    return fileContent;
}
