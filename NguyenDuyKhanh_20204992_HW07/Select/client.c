// client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8888
#define BUFFER_SIZE 1024

int main() {
    int sock;
    struct sockaddr_in server;
    char message[BUFFER_SIZE], server_reply[BUFFER_SIZE];

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    puts("Connected to server. Type 'exit' to end the connection.");

    while (1) {
        printf("Enter message : ");
        fgets(message, BUFFER_SIZE, stdin);

        if (send(sock, message, strlen(message), 0) < 0) {
            perror("send");
            exit(EXIT_FAILURE);
        }

        if (strcmp(message, "exit\n") == 0) {
            puts("Connection closed.");
            break;
        }

        if (recv(sock, server_reply, BUFFER_SIZE, 0) < 0) {
            perror("recv");
            break;
        }

        printf("Server reply : %s", server_reply);
    }

    close(sock);

    return 0;
}
