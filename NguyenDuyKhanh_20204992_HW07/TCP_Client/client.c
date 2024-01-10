#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFF_SIZE 2048

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <IP_Addr> <Port_Number>\n", argv[0]);
        return 1;
    }

    int client_fd;
    struct sockaddr_in server;
    char buffer[BUFF_SIZE];
    int bytes_received;

    // Create a socket
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    // Initialize server address structure
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[2]));
    server.sin_addr.s_addr = inet_addr(argv[1]);  // IP address of the server

    // Connect to the server
    if (connect(client_fd, (struct sockaddr*)&server, sizeof(server)) == -1) {
        perror("connect");
        exit(1);
    }

    // Receive initial server response
    bytes_received = recv(client_fd, buffer, BUFF_SIZE, 0);
    if (strstr(buffer, "100") == buffer) {
        printf("Server: 110: Successfully connected to the service\n");
    }

    while (1) {
        printf("\n========================================================\n");
        printf("Required structures:\nUSER <username> : Login\nPOST <article> : Post an article\nBYE : Sign out\n");
        printf("\nEnter request: ");
        char *input_buffer = NULL;
        size_t input_buffer_size = 0;

        // Read user input
        ssize_t input_length = getline(&input_buffer, &input_buffer_size, stdin);

        if (input_length <= 0) {
            free(input_buffer);
            break;
        }

        // Prepare the request for sending
        char *send_buffer = (char *)malloc(input_length + 3); // +3 for '\r', '\n', and '\0'
        if (send_buffer == NULL) {
            perror("malloc");
            free(input_buffer);
            break;
        }
        snprintf(send_buffer, input_length + 3, "%s\r\n", input_buffer);

        // Send the request to the server
        if (send(client_fd, send_buffer, strlen(send_buffer), 0) == -1) {
            perror("send");
            exit(1);
        }

        // Receive server response
        bytes_received = recv(client_fd, buffer, BUFF_SIZE, 0);
        if (bytes_received == 0) {
            printf("Server disconnected\n");
            break;
        } else if (bytes_received == -1) {
            perror("recv");
            exit(1);
        }

        // Print out the reply message
        printf("=> Result:");

        if (strstr(buffer, "110") == buffer) {
            printf("110: Logged in successfully\n");
        }
        if (strstr(buffer, "211") == buffer) {
            printf("211: Account locked\n");
        }
        if (strstr(buffer, "212") == buffer) {
            printf("212: Account does not exist\n");
        }
        if (strstr(buffer, "213") == buffer) {
            printf("213: Error you have logged in\n");
        }

        if (strstr(buffer, "120") == buffer) {
            printf("120: Posted successfully\n");
        }

        if (strstr(buffer, "221") == buffer) {
            printf("221: Not logged in yet\n");
        }

        if (strstr(buffer, "130") == buffer) {
            printf("130: Signed out successfully\n");
        }

        if (strstr(buffer, "300") == buffer) {
            printf("300: The requested format cannot be determined\n");
        }
    }

    // Close the client socket
    close(client_fd);
    return 0;
}
