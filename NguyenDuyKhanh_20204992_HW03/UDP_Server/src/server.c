#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "../inc/checkIpOrDomain.h"
#include "../inc/getDomainNamesForIP.h"
#include "../inc/getIPAddressesForDomain.h"

#define BUF_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <PortNumber>\n", argv[0]);
        exit(1);
    }

    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    char buffer[BUF_SIZE];

    // Create a socket
    if ((serverSocket = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("Error creating a socket");
        exit(1);
    }

    // Configure the server address
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(atoi(argv[1]));
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Error binding");
        exit(1);
    }

    printf("Waiting for connections from clients...\n");

    while (1) {
        memset(buffer, 0, sizeof(buffer));

        // Receive data from the client
        if (recvfrom(serverSocket, buffer, BUF_SIZE, 0, (struct sockaddr *)&clientAddr, &addrLen) == -1) {
            perror("Error receiving data");
            exit(1);
        }

        printf("Received from the client: %s\n", buffer);

        // Backup data from buffer to input
        char input[BUF_SIZE];
        strcpy(input, buffer);
        
        // Handle domain resolution
        memset(buffer, 0, sizeof(buffer));
    
        // Error if the content is empty
        if (strlen(input) == 0) {
            sprintf(buffer, "-Not found information");
            logRequest(input, buffer);
            continue;
        }

        //Check if the content is an IPv4 or IPv6 address or a domain name
        //Returns 1 if IPv4, returns 2 if IPv6, returns 3 if domain name, returns 0 if invalid
        int check = checkIpOrDomain(input);

        //Based on the check value, call appropriate processing functions
        switch (check){
            case 0:
                sprintf(buffer, "-Not found information");
                logRequest(input, buffer);
                break;
            case 1:
                getDomainNamesForIP(input, buffer);
                break;

            case 2:
                getDomainNamesForIP(input, buffer);
                break;
                
            case 3:
                getIPAddressesForDomain(input, buffer);
                break;

            default:
                break;
        }
        
        // Respond to the client with the data
        if (sendto(serverSocket, buffer, strlen(buffer), 0, (struct sockaddr *)&clientAddr, addrLen) == -1) {
            perror("Error sending data");
            exit(1);
        }
    }

    close(serverSocket);

    return 0;
}
