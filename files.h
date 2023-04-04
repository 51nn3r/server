#pragma once

#include <sys/stat.h>

int getFileInfo(char *path) {
    struct stat info;

    stat(path, &info);
    int mode = (int) info.st_mode;

    return mode;
}

#define IS_FILE(path)   S_ISREG(getFileInfo(path))
#define IS_DIR(path)    S_ISDIR(getFileInfo(path))

#define FILE_BATCH_SIZE 0x10
