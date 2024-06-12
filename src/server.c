#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#include <signal.h>

#include "server.h"
#include "httpHandler.h"


char *HOST;
int PORT;
int sock;


typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;


void cleanupAndExit(int signal) {
    printf("\nReceived signal %d. Cleaning up and exiting...\n", signal);
    close(sock);
    exit(EXIT_SUCCESS);
}


void serveForever() {
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr(HOST);

    if (bind(sock, (sockaddr *) &serverAddr, sizeof(sockaddr_in))) {
        perror("bind");
        close(sock);
        exit(EXIT_FAILURE);
    }

    if (listen(sock, MAX_CONNECTIONS_COUNT)) {
        perror("listen");
        close(sock);
        exit(EXIT_FAILURE);
    } else {
        printf("[+] Starting server on %s:%d\n", HOST, PORT);
    }

    int clientSocket;
    sockaddr_in clientArrd;
    socklen_t socklen = sizeof(clientArrd);

    signal(SIGCHLD, SIG_IGN);
    signal(SIGINT, cleanupAndExit);

    while (1) {
        clientSocket = accept(sock, (sockaddr *) &clientArrd, &socklen);

        if (clientSocket < 0) {
            puts("[-] Error handling client");
            exit(1);
        }

        printf("[+] Received connection from %s:%d\n",
               inet_ntoa(clientArrd.sin_addr),
               ntohs(clientArrd.sin_port));

        pid_t pid = fork();

        if (pid == -1) {
            perror("fork");
            close(clientSocket);
            close(sock);
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            close(sock);
            handleRequest(clientSocket);
            close(clientSocket);
            exit(EXIT_SUCCESS);
        } else {
            close(clientSocket);
        }
    }
}


int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("usage: %s <ip> <port>", argv[0]);
        exit(1);
    }

    HOST = argv[1];
    PORT = atoi(argv[2]);

    serveForever();
}
