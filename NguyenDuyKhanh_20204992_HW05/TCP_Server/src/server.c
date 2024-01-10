#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "../inc/menu.h"

#define BACKLOG 20
#define BUFF_SIZE 2048

// Signal handler for child processes to prevent zombie processes
void sig_chld(int signo) {
    pid_t pid;
    int stat;
    while ((pid = waitpid(-1, &stat, WNOHANG)) > 0) {
        printf("Child %d terminated\n", pid);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <Port_Number>\n", argv[0]);
        return 1;
    }

    int listen_fd, conn_fd;
    struct sockaddr_in server, client;
    socklen_t client_len;
    char buffer[BUFF_SIZE];
    int bytes_received;

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

    // Handle child processes to prevent zombie processes
    signal(SIGCHLD, sig_chld);

    while (1) {
        client_len = sizeof(client);

        // Accept incoming connection
        if ((conn_fd = accept(listen_fd, (struct sockaddr*)&client, &client_len)) == -1) {
            perror("accept");
            continue;
        }

        if (fork() == 0) {  // Child process
            close(listen_fd);  // Child doesn't need the listening socket

            int login = 0;
            char *partial_data = NULL;
            int partial_data_length = 0;

            // Connected with client reply
            memset(buffer, 0, sizeof(buffer));
            strcpy(buffer, "100");
            send(conn_fd, buffer, sizeof(buffer), 0);

            while (1) {
                bytes_received = recv(conn_fd, buffer, BUFF_SIZE, 0);
                if (bytes_received <= 0) {
                    break;  // Connection closed or error
                }

                // Append received data to the existing partial_data
                partial_data = realloc(partial_data, partial_data_length + bytes_received);
                if (partial_data == NULL) {
                    // Handle memory allocation error
                    break;
                }
                memcpy(partial_data + partial_data_length, buffer, bytes_received);
                partial_data_length += bytes_received;

                // Process the data until a "\r\n" is found
                char *crlf_position;
                while ((crlf_position = strstr(partial_data, "\r\n")) != NULL) {
                    *crlf_position = '\0'; // Null-terminate the line
                    // Process the complete line

                    // Create a buffer to store the complete data
                    int buffer_full_size = partial_data_length;
                    char *buffer_full = (char *)malloc(buffer_full_size);
                    if (buffer_full == NULL) {
                        perror("malloc");
                        // Handle memory allocation error
                    }
                    strcpy(buffer_full, partial_data);
                    if (strstr(buffer_full, "USER") == buffer_full) {
                        loginAccount(&login, buffer_full, buffer_full_size, buffer, sizeof(buffer), file);
                    } else if (strstr(buffer_full, "POST") == buffer_full) {
                        postMessage(login, buffer_full, buffer_full_size, buffer, sizeof(buffer));
                    } else if (strstr(buffer_full, "BYE") == buffer_full) {
                        logoutAccount(&login, buffer, sizeof(buffer));
                    } else {
                        memset(buffer, 0, sizeof(buffer));
                        strcpy(buffer, "300");
                    }
                    send(conn_fd, buffer, sizeof(buffer), 0);

                    // Move the remaining data to the beginning of the buffer
                    int remaining_length = partial_data_length - (crlf_position - partial_data) - 2;
                    memmove(partial_data, crlf_position + 2, remaining_length);
                    partial_data_length = remaining_length;
                }
            }

            close(conn_fd);
            exit(0);  // Child process exits
        }

        close(conn_fd);  // Parent process doesn't need the connected socket
    }

    return 0;
}
