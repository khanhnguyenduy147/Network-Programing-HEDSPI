#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUF_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <IPAddress> <PortNumber>\n", argv[0]);
        exit(1);
    }
    //
    int clientSocket;
    struct sockaddr_in serverAddr;
    char buffer[BUF_SIZE];

    // Create a socket
    if ((clientSocket = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("Error creating a socket");
        exit(1);
    }

    // Configure the server address
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &(serverAddr.sin_addr));

    while (1) {
        printf("Enter content (to exit, enter an empty string): ");
        fgets(buffer, BUF_SIZE, stdin);

        // Remove newline character from the input data
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }

        // Send the request to the server
        if (sendto(clientSocket, buffer, strlen(buffer), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
            perror("Error sending data");
            exit(1);
        }

        if (strcmp(buffer, "") == 0) {
            break;
        }

        // Receive the result from the server
        memset(buffer, 0, sizeof(buffer));
        if (recvfrom(clientSocket, buffer, BUF_SIZE, 0, NULL, NULL) == -1) {
            perror("Error receiving data");
            exit(1);
        }

        printf("Result from the server: %s\n", buffer);
    }

    close(clientSocket);

    return 0;
}
