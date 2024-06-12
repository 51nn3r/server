#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <dirent.h>
#include <sys/stat.h>

#include <sys/socket.h>

#include "httpHandler.h"
#include "server.h"
#include "files.h"
#include "strings.h"


void readLine(int fd, char *buf, size_t limit) {
    char byte;
    for (size_t offset = 0; offset < limit; offset++) {
        read(fd, &byte, 1);
        if (byte == 0x00 || byte == '\n')
            return;
        else
            buf[offset] = byte;
    }
}

string *filterUri(char *uri) {
    string *strUri = String(uri);

    int realLength = indexOf(strUri, String("?"));

    if (realLength == 0) {
        del(strUri);
        return String("/");
    } else if (realLength > 0) {
        strUri = split(strUri, 0, realLength - 1);
    }

    printf("%s\n", strUri->ptr);
    while (strUri->size > 1 && strUri->ptr[strUri->size - 1] == '/') {
        strUri = split(strUri, 0, strUri->size - 1);
    }
    printf("%s\n", strUri->ptr);

    while (1) {
        int index = indexOf(strUri, String(".."));
        if (index == -1) {
            break;
        }
        strUri = split(strUri, 0, strUri->size - 3);
    }

    if (strUri->size == 0) {
        del(strUri);
        return String("/");
    }

    return strUri;
}

void sendDir(int sock, string *uri) {
    tree_t *dirsTree = getTree(uri->ptr, DT_DIR);
    tree_t *filesTree = getTree(uri->ptr, DT_REG);

    string **dirs = treeToList(dirsTree);
    string **files = treeToList(filesTree);

    char *port = calloc(8, 1);
    sprintf(port, "%d", PORT);

    string *response = String("");

    merge(response, String(HTML_PART1));
    merge(response, String(uri->ptr));
    merge(response, String(HTML_PART2));
    merge(response, String(uri->ptr));
    merge(response, String(HTML_PART3));

    for (int index = 0; index < dirsTree->elementsCount; index++) {
        merge(response, String(LINK_PART1));
        merge(response, String(port));
        merge(response, String(uri->ptr));
        if (uri->size > 1 || uri->ptr[0] != '/')
            merge(response, String("/"));
        merge(response, String(dirs[index]->ptr));
        merge(response, String("/"));
        merge(response, String(LINK_PART2));
        merge(response, dirs[index]);
        merge(response, String("/"));
        merge(response, String(LINK_PART3));
    }

    for (int index = 0; index < filesTree->elementsCount; index++) {
        merge(response, String(LINK_PART1));
        merge(response, String(port));
        merge(response, String(uri->ptr));
        if (uri->size > 1 || uri->ptr[0] != '/')
            merge(response, String("/"));
        merge(response, String(files[index]->ptr));
        merge(response, String(LINK_PART2));
        merge(response, files[index]);
        merge(response, String(LINK_PART3));
    }

    merge(response, String(HTML_PART4));

    // Send headers
    char *strInt = calloc(0x20, 1);
    sprintf(strInt, "%ld", response->size);

    string *headers = String("");
    merge(headers, String(RESPONSE_LINE_200));
    merge(headers, String(RESPONSE_HEADERS));
    merge(headers, String(CONTENT_LENGTH));
    merge(headers, String(strInt));
    merge(headers, String("\r\n\r\n"));
    send(sock, headers->ptr, headers->size, 0);

    // Send response
    send(sock, response->ptr, response->size, 0);
}

void sendFile(int sock, string *uri) {
    file_content_t *fileContent = readFile(uri->ptr);

    // Send headers
    char *strInt = calloc(0x20, 1);
    sprintf(strInt, "%ld", fileContent->size);

    string *headers = String("");
    merge(headers, String(RESPONSE_LINE_200));
    merge(headers, String(RESPONSE_FILE_HEADERS));
    merge(headers, String(CONTENT_LENGTH));
    merge(headers, String(strInt));
    merge(headers, String("\r\n\r\n"));
    send(sock, headers->ptr, headers->size, 0);

    // Send response
    send(sock, fileContent->data, fileContent->size, 0);
    free(fileContent);
}

void sendFileNoFound(int sock) {
    string *response = String("");
    merge(response, String(FILE_NOT_FOUND));

    // Send headers
    char *strInt = calloc(0x20, 1);
    sprintf(strInt, "%ld", response->size);

    string *headers = String("");
    merge(headers, String(RESPONSE_LINE_200));
    merge(headers, String(RESPONSE_HEADERS));
    merge(headers, String(CONTENT_LENGTH));
    merge(headers, String(strInt));
    merge(headers, String("\r\n\r\n"));
    send(sock, headers->ptr, headers->size, 0);

    // Send response
    send(sock, response->ptr, response->size, 0);
}

void handleRequest(int sock) {
    char requestLine[REQUEST_SIZE], method[REQUEST_SIZE], uri[REQUEST_SIZE], version[REQUEST_SIZE];
    readLine(sock, requestLine, REQUEST_SIZE);
    sscanf(requestLine, "%s %s %s", method, uri, version);

    string *strUri = filterUri(uri);
    if (IS_DIR(strUri->ptr)) {
        sendDir(sock, strUri);
    } else if (IS_FILE(strUri->ptr)) {
        sendFile(sock, strUri);
    } else {
        sendFileNoFound(sock);
    }

    close(sock);
}
