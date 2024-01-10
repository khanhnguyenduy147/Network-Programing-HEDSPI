#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 5500
#define BUFF_SIZE 1024

int main() {
    int sockfd;
    char sendBuff[BUFF_SIZE], rcvBuff[BUFF_SIZE];

    struct sockaddr_in servaddr;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);

    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1) {
        perror("Error connecting to server");
        exit(EXIT_FAILURE);
    }

    while (1) {
        printf("Enter message: ");
        fgets(sendBuff, BUFF_SIZE, stdin);

        send(sockfd, sendBuff, strlen(sendBuff), 0);

        if (strcmp(sendBuff, "exit\n") == 0) {
            printf("Connection closed.\n");
            break;
        }

        recv(sockfd, rcvBuff, BUFF_SIZE, 0);
        printf("Server reply: %s", rcvBuff);
    }

    close(sockfd);

    return 0;
}
