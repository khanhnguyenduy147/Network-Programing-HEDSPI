#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 5500
#define BACKLOG 20
#define BUFF_SIZE 1024
#define FD_SETSIZE 20

void processData(char *in, char *out);
int receiveData(int s, char *buff, int size, int flags);
int sendData(int s, char *buff, int size, int flags);

int main() {
    int i, maxi, maxfd, listenfd, connfd, sockfd;
    int nready, client[FD_SETSIZE];
    ssize_t ret;
    fd_set readfds, allset;
    char sendBuff[BUFF_SIZE], rcvBuff[BUFF_SIZE];
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("\nError creating socket");
        return 0;
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    if (bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1) {
        perror("\nError binding");
        return 0;
    }

    if (listen(listenfd, BACKLOG) == -1) {
        perror("\nError listening");
        return 0;
    }

    maxfd = listenfd;
    maxi = -1;
    for (i = 0; i < FD_SETSIZE; i++)
        client[i] = -1;
    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);

    while (1) {
        readfds = allset;
        nready = select(maxfd + 1, &readfds, NULL, NULL, NULL);

        if (nready < 0) {
            perror("\nError in select");
            return 0;
        }

        if (FD_ISSET(listenfd, &readfds)) {
            clilen = sizeof(cliaddr);
            if ((connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &clilen)) < 0)
                perror("\nError accepting connection");
            else {
                printf("You got a connection from %s\n", inet_ntoa(cliaddr.sin_addr));
                for (i = 0; i < FD_SETSIZE; i++)
                    if (client[i] < 0) {
                        client[i] = connfd;
                        break;
                    }
                if (i == FD_SETSIZE) {
                    printf("\nToo many clients");
                    close(connfd);
                }
                FD_SET(connfd, &allset);
                if (connfd > maxfd)
                    maxfd = connfd;
                if (i > maxi)
                    maxi = i;

                if (--nready <= 0)
                    continue;
            }
        }

        for (i = 0; i <= maxi; i++) {
            if ((sockfd = client[i]) < 0)
                continue;
            if (FD_ISSET(sockfd, &readfds)) {
                ret = receiveData(sockfd, rcvBuff, BUFF_SIZE, 0);
                if (ret <= 0) {
                    FD_CLR(sockfd, &allset);
                    close(sockfd);
                    
                    client[i] = -1;
                } else {
                    processData(rcvBuff, sendBuff);
                    ret = sendData(sockfd, sendBuff, ret, 0);
                    if (ret < 0) {
                        FD_CLR(sockfd, &allset);
                        close(sockfd);
                        client[i] = -1;
                    }
                }
                if (--nready <= 0)
                    break;
            }
        }
    }

    return 0;
}

void processData(char *in, char *out) {
    strcpy(out, in);
}

int receiveData(int s, char *buff, int size, int flags) {
    int n;
    n = recv(s, buff, size, flags);
    if (n < 0)
        perror("Error receiving data");
    else if (n == 0)
        printf("Connection closed!");
    return n;
}

int sendData(int s, char *buff, int size, int flags) {
    int n;
    n = send(s, buff, size, flags);
    if (n < 0)
        perror("Error sending data");
    return n;
}
