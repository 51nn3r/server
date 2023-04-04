#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/wait.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#include <sys/socket.h>

#include "server.h"
#include "httpHandler.c"


typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;


void serveForever() {
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

    sockaddr_in *serverAddr = malloc(sizeof(sockaddr_in));
    serverAddr->sin_family = AF_INET;
    serverAddr->sin_port = htons(PORT);
    serverAddr->sin_addr.s_addr = inet_addr(HOST);

    if (bind(sock, (sockaddr *) serverAddr, sizeof(sockaddr_in))) {
        printf("[-] Error starting server on %s:%d\n", HOST, PORT);
        exit(1);
    }

    if (listen(sock, MAX_CONNECTIONS_COUNT)) {
        printf("[-] Error starting server on %s:%d\n", HOST, PORT);
        exit(1);
    } else {
        printf("[+] Starting server on %s:%d\n", HOST, PORT);
    }

    pid_t cid[MAX_CONNECTIONS_COUNT];
    memset(cid, 0x00, sizeof(cid));
    int nextConnectionIndex, clientSocket;
    sockaddr_in *clientArrd = malloc(sizeof(sockaddr_in));
    socklen_t *socklen = malloc(sizeof(socklen));

    while (1) {
        // Check child processes
        nextConnectionIndex = -1;

        while (nextConnectionIndex == -1) {
            for (int index = 0; index < MAX_CONNECTIONS_COUNT; index++) {
                if (cid[index] == 0) {
                    nextConnectionIndex = index;
                    continue;
                }
                int status;
                pid_t retPid = waitpid(cid[index], &status, WNOHANG);
                if (retPid < -1) {
                    puts("[-] Error while waiting processes finish");
                } else if (retPid == 0) {
                    cid[index] = 0;
                    nextConnectionIndex = index;
                }
            }
        }

        // Handle new connection
        clientSocket = accept(sock, (sockaddr *) clientArrd, socklen);

        if (clientSocket < 0) {
            puts("[-] Error handling client");
            exit(1);
        }

        printf("[+] Received connection from %s:%d\n",
               inet_ntoa(clientArrd->sin_addr),
               ntohs(clientArrd->sin_port));

        pid_t pid = fork();

        if (pid == 0) {
            handleRequest(clientSocket);
            exit(0);
        } else {
            // sleep(2);
            close(clientSocket);
            cid[nextConnectionIndex] = pid;
        }
    }
}

int main(int argc, char *argv[]) {
    serveForever();
    return 0;
}
