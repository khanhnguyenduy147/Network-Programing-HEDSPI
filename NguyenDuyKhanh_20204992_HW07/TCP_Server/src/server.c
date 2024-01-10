#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include "../inc/limit_client.h"
#include "../inc/linklist_data_stream.h"
#include "../inc/client_handle.h"

struct LinkedList session_client;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <Port_Number>\n", argv[0]);
        return 1;
    }

    int listen_fd;
    session_client.head = NULL;
    struct sockaddr_in server;

    // Create a socket
    if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    // Initialize server structure
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[1]));
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    // Bind the socket to the server address
    if (bind(listen_fd, (struct sockaddr*)&server, sizeof(server)) == -1) {
        perror("bind");
        exit(1);
    }

    // Start listening for incoming connections
    if (listen(listen_fd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    // Open the account file for reading
    FILE *file = fopen("account.txt", "r");
    if (file == NULL) {
        perror("Cannot open file");
        return 0; // Error opening file
    }

    clientHandle(file, listen_fd, session_client);

    return 0;
}
